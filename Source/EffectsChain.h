#pragma once
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

// EffectsChain.h - Phase 3.5: Effects Chain
// 8 effects in serial processing chain with fixed internal parameters (only mix exposed)
// Order: Distortion → Chorus → Phaser → Flanger → Delay → Reverb → EQ → Compressor
class EffectsChain
{
public:
    EffectsChain()
    {
        // Initialize fixed parameters for each effect (will be set in prepareToPlay)
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
        spec.numChannels = 2; // Stereo

        currentSampleRate = sampleRate;

        // Prepare all effects
        chorus.prepare(spec);
        phaser.prepare(spec);
        reverb.setSampleRate(sampleRate); // Classic juce::Reverb uses setSampleRate
        compressor.prepare(spec);

        // Delay line preparation (2 seconds max delay)
        int maxDelaySamples = static_cast<int>(sampleRate * 2.0);
        delayLine.prepare(spec);
        delayLine.setMaximumDelayInSamples(maxDelaySamples);
        delayLine.reset();

        // Flanger delay line (short delay for flanger effect)
        int maxFlangerDelaySamples = static_cast<int>(sampleRate * 0.02); // 20ms max
        flangerDelayLine.prepare(spec);
        flangerDelayLine.setMaximumDelayInSamples(maxFlangerDelaySamples);
        flangerDelayLine.reset();

        // EQ filter preparation (3 bands)
        lowShelf.prepare(spec);
        midPeak.prepare(spec);
        highShelf.prepare(spec);

        // Configure fixed parameters for each effect
        configureFixedParameters(sampleRate);

        // Reset all effects
        chorus.reset();
        phaser.reset();
        reverb.reset();
        compressor.reset();
        lowShelf.reset();
        midPeak.reset();
        highShelf.reset();

        // Reset LFOs for modulation effects
        flangerLFOPhase = 0.0f;
    }

    void reset()
    {
        compressor.reset();
        delayLine.reset();
        flangerDelayLine.reset();
        lowShelf.reset();
        midPeak.reset();
        highShelf.reset();

        // Reset LFO phases
        flangerLFOPhase = 0.0f;
        chorusLFOPhase = 0.0f;
        phaserLFOPhase = 0.0f;

        // Reset phaser all-pass filter states
        for (int i = 0; i < 4; ++i)
        {
            phaserStateL[i] = 0.0f;
            phaserStateR[i] = 0.0f;
        }

        // Clear delay feedback buffers
        delayFeedbackLeft = 0.0f;
        delayFeedbackRight = 0.0f;
    }

    // Process stereo sample through entire effects chain
    void processStereo(float& leftSample, float& rightSample)
    {
        // Effect 1: Distortion (tanh waveshaping)
        if (distortionMix > 0.0f)
        {
            float dryLeft = leftSample;
            float dryRight = rightSample;

            float wetLeft = std::tanh(leftSample * distortionDrive);
            float wetRight = std::tanh(rightSample * distortionDrive);

            leftSample = dryLeft * (1.0f - distortionMix) + wetLeft * distortionMix;
            rightSample = dryRight * (1.0f - distortionMix) + wetRight * distortionMix;
        }

        // Effect 2: Chorus (simple delay-based, similar to flanger but longer delay)
        if (chorusMix > 0.0f)
        {
            float dryLeft = leftSample;
            float dryRight = rightSample;

            // Update chorus LFO phase (0.5 Hz rate)
            chorusLFOPhase += (0.5f * 2.0f * juce::MathConstants<float>::pi) / static_cast<float>(currentSampleRate);
            if (chorusLFOPhase > 2.0f * juce::MathConstants<float>::pi)
                chorusLFOPhase -= 2.0f * juce::MathConstants<float>::pi;

            // Modulated delay time for chorus (7-20ms)
            float lfoValue = std::sin(chorusLFOPhase);
            float delayMs = 13.5f + lfoValue * 6.5f; // 7ms to 20ms
            float delaySamples = (delayMs / 1000.0f) * static_cast<float>(currentSampleRate);

            // Use delay line for chorus (re-purpose flanger delay with longer time)
            // Simple chorus: mix delayed signal with dry
            float wetLeft = delayLine.popSample(0, delaySamples, true);
            float wetRight = delayLine.popSample(1, delaySamples, true);

            leftSample = dryLeft * (1.0f - chorusMix) + wetLeft * chorusMix * 0.5f + dryLeft * 0.5f;
            rightSample = dryRight * (1.0f - chorusMix) + wetRight * chorusMix * 0.5f + dryRight * 0.5f;
        }

        // Effect 3: Phaser (simple all-pass based)
        if (phaserMix > 0.0f)
        {
            float dryLeft = leftSample;
            float dryRight = rightSample;

            // Update phaser LFO phase (0.3 Hz rate)
            phaserLFOPhase += (0.3f * 2.0f * juce::MathConstants<float>::pi) / static_cast<float>(currentSampleRate);
            if (phaserLFOPhase > 2.0f * juce::MathConstants<float>::pi)
                phaserLFOPhase -= 2.0f * juce::MathConstants<float>::pi;

            // Simple phaser: modulated coefficient
            float lfoValue = std::sin(phaserLFOPhase);
            float coef = 0.3f + lfoValue * 0.2f; // 0.1 to 0.5 all-pass coefficient

            // 4-stage all-pass chain (simple phaser)
            float wetLeft = leftSample;
            float wetRight = rightSample;
            for (int stage = 0; stage < 4; ++stage)
            {
                float outL = coef * wetLeft + phaserStateL[stage];
                phaserStateL[stage] = wetLeft - coef * outL;
                wetLeft = outL;

                float outR = coef * wetRight + phaserStateR[stage];
                phaserStateR[stage] = wetRight - coef * outR;
                wetRight = outR;
            }

            leftSample = dryLeft * (1.0f - phaserMix) + wetLeft * phaserMix;
            rightSample = dryRight * (1.0f - phaserMix) + wetRight * phaserMix;
        }

        // Effect 4: Flanger (custom delay-based with LFO modulation)
        if (flangerMix > 0.0f)
        {
            float dryLeft = leftSample;
            float dryRight = rightSample;

            // Update LFO phase (0.2 Hz rate)
            flangerLFOPhase += (flangerLFORate * 2.0f * juce::MathConstants<float>::pi) / static_cast<float>(currentSampleRate);
            if (flangerLFOPhase > 2.0f * juce::MathConstants<float>::pi)
                flangerLFOPhase -= 2.0f * juce::MathConstants<float>::pi;

            // Calculate modulated delay time (1-10ms)
            float lfoValue = std::sin(flangerLFOPhase); // -1.0 to +1.0
            float delayMs = 5.5f + lfoValue * 4.5f; // 1ms to 10ms
            float delaySamples = (delayMs / 1000.0f) * static_cast<float>(currentSampleRate);

            // Push input to delay line
            flangerDelayLine.pushSample(0, leftSample);
            flangerDelayLine.pushSample(1, rightSample);

            // Read delayed sample
            float delayedLeft = flangerDelayLine.popSample(0, delaySamples, true); // true = interpolate
            float delayedRight = flangerDelayLine.popSample(1, delaySamples, true);

            // Apply feedback
            float wetLeft = delayedLeft + leftSample * flangerFeedback;
            float wetRight = delayedRight + rightSample * flangerFeedback;

            leftSample = dryLeft * (1.0f - flangerMix) + wetLeft * flangerMix;
            rightSample = dryRight * (1.0f - flangerMix) + wetRight * flangerMix;
        }

        // Effect 5: Delay (stereo delay with feedback)
        if (delayMix > 0.0f)
        {
            float dryLeft = leftSample;
            float dryRight = rightSample;

            // Calculate delay time in samples (500ms fixed)
            float delaySamples = (delayTime / 1000.0f) * static_cast<float>(currentSampleRate);

            // Push input + feedback to delay line
            delayLine.pushSample(0, leftSample + delayFeedbackLeft);
            delayLine.pushSample(1, rightSample + delayFeedbackRight);

            // Read delayed sample
            float delayedLeft = delayLine.popSample(0, delaySamples);
            float delayedRight = delayLine.popSample(1, delaySamples);

            // Update feedback buffer
            delayFeedbackLeft = delayedLeft * delayFeedback;
            delayFeedbackRight = delayedRight * delayFeedback;

            leftSample = dryLeft * (1.0f - delayMix) + delayedLeft * delayMix;
            rightSample = dryRight * (1.0f - delayMix) + delayedRight * delayMix;
        }

        // Effect 6: Reverb (using JUCE Reverb with single-sample wrapper)
        if (reverbMix > 0.0f)
        {
            float dryLeft = leftSample;
            float dryRight = rightSample;

            // Use single-sample arrays for JUCE reverb
            float wetLeftArr[1] = { leftSample };
            float wetRightArr[1] = { rightSample };
            reverb.processStereo(wetLeftArr, wetRightArr, 1);

            leftSample = dryLeft * (1.0f - reverbMix) + wetLeftArr[0] * reverbMix;
            rightSample = dryRight * (1.0f - reverbMix) + wetRightArr[0] * reverbMix;
        }

        // Effect 7: EQ (3-band parametric)
        if (eqMix > 0.0f)
        {
            float dryLeft = leftSample;
            float dryRight = rightSample;

            // Process through 3-band EQ (serial chain)
            float wetLeft = leftSample;
            float wetRight = rightSample;

            // Low shelf
            wetLeft = lowShelf.processSample(wetLeft);
            wetRight = lowShelf.processSample(wetRight);

            // Mid peak (bypassed in v1.0: 0dB gain)
            wetLeft = midPeak.processSample(wetLeft);
            wetRight = midPeak.processSample(wetRight);

            // High shelf
            wetLeft = highShelf.processSample(wetLeft);
            wetRight = highShelf.processSample(wetRight);

            leftSample = dryLeft * (1.0f - eqMix) + wetLeft * eqMix;
            rightSample = dryRight * (1.0f - eqMix) + wetRight * eqMix;
        }

        // Effect 8: Compressor (juce::dsp::Compressor) - Simple soft knee compression
        if (compressorMix > 0.0f)
        {
            float dryLeft = leftSample;
            float dryRight = rightSample;

            // Use compressor's processSample for single sample processing
            float wetLeft = compressor.processSample(0, leftSample);
            float wetRight = compressor.processSample(1, rightSample);

            leftSample = dryLeft * (1.0f - compressorMix) + wetLeft * compressorMix;
            rightSample = dryRight * (1.0f - compressorMix) + wetRight * compressorMix;
        }
    }

    // Update mix parameters (0.0-1.0 normalized values from parameters)
    void setDistortionMix(float mix) { distortionMix = juce::jlimit(0.0f, 1.0f, mix); }
    void setChorusMix(float mix) { chorusMix = juce::jlimit(0.0f, 1.0f, mix); }
    void setPhaserMix(float mix) { phaserMix = juce::jlimit(0.0f, 1.0f, mix); }
    void setFlangerMix(float mix) { flangerMix = juce::jlimit(0.0f, 1.0f, mix); }
    void setDelayMix(float mix) { delayMix = juce::jlimit(0.0f, 1.0f, mix); }
    void setReverbMix(float mix) { reverbMix = juce::jlimit(0.0f, 1.0f, mix); }
    void setEQMix(float mix) { eqMix = juce::jlimit(0.0f, 1.0f, mix); }
    void setCompressorMix(float mix) { compressorMix = juce::jlimit(0.0f, 1.0f, mix); }

    // Effect timing parameters
    void setDelayTime(float timeMs) {
        delayTime = juce::jlimit(10.0f, 2000.0f, timeMs);
    }

    void setDelayFeedback(float feedback) {
        delayFeedback = juce::jlimit(0.0f, 0.95f, feedback);
    }

    void setReverbSize(float size) {
        reverbRoomSize = juce::jlimit(0.0f, 1.0f, size);
        updateReverbParams();
    }

    void setReverbDamping(float damping) {
        reverbDamping = juce::jlimit(0.0f, 1.0f, damping);
        updateReverbParams();
    }

    void setReverbWidth(float width) {
        reverbWidth = juce::jlimit(0.0f, 1.0f, width);
        updateReverbParams();
    }

    void setChorusRate(float rate) {
        chorus.setRate(juce::jlimit(0.1f, 10.0f, rate));
    }

    void setChorusDepth(float depth) {
        chorus.setDepth(juce::jlimit(0.0f, 1.0f, depth));
    }

    void setFlangerRate(float rate) {
        flangerLFORate = juce::jlimit(0.05f, 5.0f, rate);
    }

    void setPhaserRate(float rate) {
        phaser.setRate(juce::jlimit(0.1f, 5.0f, rate));
    }

private:
    // Mix parameters (0.0-1.0, controlled by APVTS)
    float distortionMix = 0.0f;
    float chorusMix = 0.0f;
    float phaserMix = 0.0f;
    float flangerMix = 0.0f;
    float delayMix = 0.0f;
    float reverbMix = 0.0f;
    float eqMix = 0.0f;
    float compressorMix = 0.0f;

    // Fixed effect parameters (set in configureFixedParameters)
    float distortionDrive = 5.0f;
    float delayTime = 500.0f; // ms
    float delayFeedback = 0.4f;
    float flangerLFORate = 0.2f; // Hz
    float flangerFeedback = 0.6f;

    // Reverb parameters (for classic juce::Reverb)
    float reverbRoomSize = 0.7f;
    float reverbDamping = 0.5f;
    float reverbWidth = 1.0f;

    void updateReverbParams() {
        juce::Reverb::Parameters params;
        params.roomSize = reverbRoomSize;
        params.damping = reverbDamping;
        params.width = reverbWidth;
        params.wetLevel = 1.0f;
        params.dryLevel = 0.0f;
        reverb.setParameters(params);
    }

    // JUCE DSP components
    juce::dsp::Chorus<float> chorus;
    juce::dsp::Phaser<float> phaser;
    juce::Reverb reverb; // Using classic juce::Reverb for sample-by-sample processing
    juce::dsp::Compressor<float> compressor;
    juce::dsp::DelayLine<float> delayLine { 192000 }; // Max 2 seconds at 96kHz
    juce::dsp::DelayLine<float> flangerDelayLine { 1920 }; // Max 20ms at 96kHz

    // EQ filters (3-band)
    juce::dsp::IIR::Filter<float> lowShelf;
    juce::dsp::IIR::Filter<float> midPeak;
    juce::dsp::IIR::Filter<float> highShelf;

    // Delay feedback buffers
    float delayFeedbackLeft = 0.0f;
    float delayFeedbackRight = 0.0f;

    // Flanger LFO state
    float flangerLFOPhase = 0.0f;

    // Chorus LFO state (for simple delay-based chorus)
    float chorusLFOPhase = 0.0f;

    // Phaser LFO state and all-pass filter states
    float phaserLFOPhase = 0.0f;
    float phaserStateL[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float phaserStateR[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Current sample rate (for LFO and delay calculations)
    double currentSampleRate = 44100.0;

    // Configure fixed parameters for all effects (called in prepareToPlay)
    void configureFixedParameters(double sampleRate)
    {
        // Distortion: drive = 5.0 (fixed)
        distortionDrive = 5.0f;

        // Chorus: rate = 0.5 Hz, depth = 0.3, centre delay = 7ms, feedback = 0.1
        chorus.setRate(0.5f);
        chorus.setDepth(0.3f);
        chorus.setCentreDelay(7.0f); // ms
        chorus.setFeedback(0.1f);
        chorus.setMix(1.0f); // 100% wet (we handle dry/wet in processStereo)

        // Phaser: rate = 0.3 Hz, depth = 0.5, centre frequency = 500 Hz, feedback = 0.4
        phaser.setRate(0.3f);
        phaser.setDepth(0.5f);
        phaser.setCentreFrequency(500.0f);
        phaser.setFeedback(0.4f);
        phaser.setMix(1.0f); // 100% wet

        // Reverb: room size = 0.7, damping = 0.5, width = 1.0, wet/dry handled externally
        juce::Reverb::Parameters reverbParams;
        reverbParams.roomSize = 0.7f;
        reverbParams.damping = 0.5f;
        reverbParams.width = 1.0f;
        reverbParams.wetLevel = 1.0f; // 100% wet (we handle dry/wet in processStereo)
        reverbParams.dryLevel = 0.0f; // No dry
        reverb.setParameters(reverbParams);

        // Compressor: threshold = -20 dB, ratio = 4:1, attack = 10ms, release = 100ms
        compressor.setThreshold(-20.0f);
        compressor.setRatio(4.0f);
        compressor.setAttack(10.0f);
        compressor.setRelease(100.0f);

        // Delay: time = 500ms, feedback = 0.4 (fixed)
        delayTime = 500.0f; // ms
        delayFeedback = 0.4f;

        // Flanger: LFO rate = 0.2 Hz, feedback = 0.6 (fixed)
        flangerLFORate = 0.2f; // Hz
        flangerFeedback = 0.6f;

        // EQ: Low shelf +3dB@100Hz, Mid peak 0dB@1kHz (bypass), High shelf +2dB@8kHz
        *lowShelf.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate, 100.0f, 0.707f, juce::Decibels::decibelsToGain(3.0f));

        *midPeak.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, 1000.0f, 0.707f, juce::Decibels::decibelsToGain(0.0f)); // 0dB = bypass

        *highShelf.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate, 8000.0f, 0.707f, juce::Decibels::decibelsToGain(2.0f));
    }
};
