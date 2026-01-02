#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include "WavetableOscillator.h"
#include "SubOscillator.h"
#include "NoiseOscillator.h"
#include "FilterBank.h"

// Voice class - Phase 3.4: Added Unison Processing
// Complete subtractive synthesis path: Oscillators → Mix → Unison Expansion → Filter → Amp Envelope
class Voice
{
public:
    Voice()
    {
        ampEnvelope.setSampleRate(44100.0);
        ampEnvelope.setParameters({0.01f, 0.1f, 1.0f, 0.2f}); // Default ADSR

        // Initialize unison parameters
        unisonCount = 1; // Default: no unison (1 voice)
        detuneAmount = 0.0f;
        stereoSpread = 0.5f;

        // Pre-calculate detune factors for max unison count (will use subset based on unisonCount)
        calculateDetuneFactors();
    }

    // Note-on: trigger voice
    void noteOn(int noteNumber, float vel, double sr)
    {
        midiNote = noteNumber;
        velocity = vel;
        isActive = true;
        sampleRate = sr;

        // Calculate base frequency from MIDI note
        frequency = 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);

        // Reset all unison oscillators
        for (int i = 0; i < maxUnisonVoices; ++i)
        {
            unisonOscA[i].reset();
            unisonOscB[i].reset();
        }
        subOsc.reset();
        noiseOsc.reset();

        // Set sub oscillator frequency
        subOsc.setFrequency(frequency, sampleRate);

        // Reset filter and trigger filter envelope
        filter.reset();
        filter.noteOn();

        // Reset release flag and trigger envelope
        inRelease = false;
        ampEnvelope.noteOn();
    }

    // Note-off: release voice
    void noteOff()
    {
        ampEnvelope.noteOff();
        filter.noteOff();
        inRelease = true;
    }

    // Check if voice is playing
    bool isPlaying() const
    {
        return isActive;
    }

    // Get MIDI note this voice is playing
    int getMidiNote() const
    {
        return midiNote;
    }

    // Update oscillator A parameters
    void updateOscillatorA(int wavetable, float position, float level, float pan,
                           int octave, int semitone, int fine,
                           int warpMode, float warpAmount)
    {
        // Update all unison oscillators with same parameters
        // Frequency will be detuned individually during sample generation
        for (int i = 0; i < maxUnisonVoices; ++i)
        {
            unisonOscA[i].setWavetable(wavetable);
            unisonOscA[i].setPosition(position / 100.0f); // Convert 0-100% to 0.0-1.0
            unisonOscA[i].setWarpMode(warpMode);
            unisonOscA[i].setWarpAmount(warpAmount / 100.0f); // Convert 0-100% to 0.0-1.0

            // Calculate detuned frequency for this unison voice
            float detunedFreq = frequency * detuneFactors[i];
            unisonOscA[i].setFrequency(detunedFreq, octave - 4, semitone, fine, sampleRate); // octave param is index 0-8, convert to -4 to +4
        }

        // Store level and pan for mixing
        oscA_level = level / 100.0f; // Convert 0-100% to 0.0-1.0
        oscA_pan = pan / 100.0f; // Convert -100 to +100 to -1.0 to +1.0
    }

    // Update oscillator B parameters
    void updateOscillatorB(int wavetable, float position, float level, float pan,
                           int octave, int semitone, int fine,
                           int warpMode, float warpAmount)
    {
        // Update all unison oscillators with same parameters
        for (int i = 0; i < maxUnisonVoices; ++i)
        {
            unisonOscB[i].setWavetable(wavetable);
            unisonOscB[i].setPosition(position / 100.0f);
            unisonOscB[i].setWarpMode(warpMode);
            unisonOscB[i].setWarpAmount(warpAmount / 100.0f);

            // Calculate detuned frequency for this unison voice
            float detunedFreq = frequency * detuneFactors[i];
            unisonOscB[i].setFrequency(detunedFreq, octave - 4, semitone, fine, sampleRate);
        }

        oscB_level = level / 100.0f;
        oscB_pan = pan / 100.0f;
    }

    // Update sub oscillator parameters
    void updateSubOscillator(int shape, int octaveIndex, float level)
    {
        subOsc.setShape(shape); // 0=Sine, 1=Triangle, 2=Square
        subOsc.setOctaveOffset(octaveIndex); // 0=-2, 1=-1, 2=0
        subOsc.setFrequency(frequency, sampleRate);

        sub_level = level / 100.0f; // Convert 0-100% to 0.0-1.0
    }

    // Update noise oscillator parameters
    void updateNoiseOscillator(int noiseType, float level)
    {
        noiseOsc.setType(noiseType); // 0=White, 1=Pink, 2=Digital

        noise_level = level / 100.0f; // Convert 0-100% to 0.0-1.0
    }

    // Update filter parameters
    void updateFilter(int filterType, float cutoff, float resonance, float drive,
                      float envDepth, float keytrack)
    {
        filter.setFilterType(filterType); // 0=LP24, 1=LP12, 2=HP24, 3=BP12, 4=Notch
        filter.setCutoffFrequency(cutoff); // 20-20000 Hz
        filter.setResonance(resonance); // 0-100%
        filter.setDrive(drive); // 0-100%
        filter.setEnvelopeDepth(envDepth); // -100 to +100%
        filter.setKeytrackAmount(keytrack); // 0-100%
    }

    // Update filter envelope parameters
    void updateFilterEnvelope(float attack, float decay, float sustain, float release)
    {
        filter.updateFilterEnvelope(attack, decay, sustain, release);
    }

    // Update unison parameters (Phase 3.4)
    void updateUnisonParameters(int unisonIndex, float detune, float spread)
    {
        // Convert unison index (0-4) to actual voice count (1, 2, 4, 8, 16)
        static const int unisonCounts[] = {1, 2, 4, 8, 16};
        unisonCount = unisonCounts[unisonIndex];

        // Convert detune parameter from 0-100% to 0.0-1.0, then scale to semitones
        // Max detune is ±0.5 semitones (50 cents) at 100%
        detuneAmount = (detune / 100.0f) * 0.5f; // 0-100% → 0.0-0.5 semitones

        // Convert spread parameter from 0-100% to 0.0-1.0
        stereoSpread = spread / 100.0f;

        // Recalculate detune factors for new unison count
        calculateDetuneFactors();
    }

    // Generate next audio sample (mono - for backward compatibility)
    float getNextSample()
    {
        if (!isActive)
            return 0.0f;

        // Generate samples from first unison oscillators (mono fallback)
        float sampleA = unisonOscA[0].getNextSample() * oscA_level;
        float sampleB = unisonOscB[0].getNextSample() * oscB_level;

        // Mix oscillators (mono sum)
        float mixedSample = sampleA + sampleB;

        // Apply velocity
        mixedSample *= velocity;

        // Apply amp envelope
        float envValue = ampEnvelope.getNextSample();
        mixedSample *= envValue;

        // Check if envelope finished
        if (inRelease && envValue < 0.0001f)
            isActive = false;

        return mixedSample;
    }

    // Generate next stereo sample pair (Phase 3.4 - Added Unison Processing)
    void getNextSampleStereo(float& leftOut, float& rightOut)
    {
        if (!isActive)
        {
            leftOut = 0.0f;
            rightOut = 0.0f;
            return;
        }

        // Initialize accumulator for unison voices
        float leftMix = 0.0f;
        float rightMix = 0.0f;

        // Process each active unison voice
        for (int unisonIndex = 0; unisonIndex < unisonCount; ++unisonIndex)
        {
            // Generate samples from detuned wavetable oscillators (already configured with detuned frequencies)
            float sampleA = unisonOscA[unisonIndex].getNextSample() * oscA_level;
            float sampleB = unisonOscB[unisonIndex].getNextSample() * oscB_level;

            // Calculate stereo panning for this unison voice
            float unisonPan = panFactors[unisonIndex] * stereoSpread; // -1.0 to +1.0

            // Apply oscillator-specific panning FIRST, then unison stereo spread
            // Convert pan from -1..1 to 0..1 range for constant-power law
            float panA = (oscA_pan + unisonPan + 1.0f) * 0.5f;
            panA = juce::jlimit(0.0f, 1.0f, panA); // Clamp to valid range
            float gainL_A = std::cos(panA * juce::MathConstants<float>::halfPi);
            float gainR_A = std::sin(panA * juce::MathConstants<float>::halfPi);

            float panB = (oscB_pan + unisonPan + 1.0f) * 0.5f;
            panB = juce::jlimit(0.0f, 1.0f, panB);
            float gainL_B = std::cos(panB * juce::MathConstants<float>::halfPi);
            float gainR_B = std::sin(panB * juce::MathConstants<float>::halfPi);

            // Mix wavetable oscillators with panning applied for this unison voice
            float leftSample = (sampleA * gainL_A) + (sampleB * gainL_B);
            float rightSample = (sampleA * gainR_A) + (sampleB * gainR_B);

            // Add sub oscillator (mono, centered - only once for first unison voice to avoid excessive bass)
            if (unisonIndex == 0)
            {
                float subSample = subOsc.getNextSample() * sub_level;
                leftSample += subSample;
                rightSample += subSample;
            }

            // Add noise oscillator (stereo, same noise - only once for first unison voice)
            if (unisonIndex == 0)
            {
                float noiseSample = noiseOsc.getNextSample() * noise_level;
                leftSample += noiseSample;
                rightSample += noiseSample;
            }

            // Accumulate this unison voice to the mix
            leftMix += leftSample;
            rightMix += rightSample;
        }

        // Normalize by unison count to maintain consistent loudness
        // Use sqrt to maintain constant power (energy preservation)
        float unisonGain = 1.0f / std::sqrt(static_cast<float>(unisonCount));
        leftMix *= unisonGain;
        rightMix *= unisonGain;

        // Apply velocity to mixed oscillators
        leftMix *= velocity;
        rightMix *= velocity;

        // Process through filter (per-channel)
        // Filter processes left and right independently
        leftMix = filter.processSample(leftMix, midiNote);
        rightMix = filter.processSample(rightMix, midiNote);

        // Apply amp envelope AFTER filter (standard subtractive synthesis order)
        float envValue = ampEnvelope.getNextSample();
        leftMix *= envValue;
        rightMix *= envValue;

        // Check if envelope finished
        if (inRelease && envValue < 0.0001f)
            isActive = false;

        // Output stereo samples
        leftOut = leftMix;
        rightOut = rightMix;
    }

    // Update envelope parameters
    void updateEnvelope(float attack, float decay, float sustain, float release)
    {
        ampEnvelope.setParameters({attack, decay, sustain, release});
    }

    // Prepare to play (update sample rate)
    void prepareToPlay(double sr)
    {
        sampleRate = sr;
        ampEnvelope.setSampleRate(sampleRate);

        // Prepare filter with new sample rate
        filter.prepareToPlay(sampleRate);

        // Update oscillator frequencies with new sample rate
        if (isActive)
        {
            // Recalculate frequencies (tuning parameters stored in oscillators)
            // This will be called during voice update cycle
            subOsc.setFrequency(frequency, sampleRate);
        }
    }

    // Reset voice state
    void reset()
    {
        isActive = false;
        inRelease = false;
        midiNote = -1;
        velocity = 0.0f;
        frequency = 0.0f;
        sampleRate = 44100.0;

        oscA_level = 1.0f;
        oscA_pan = 0.0f;
        oscB_level = 1.0f;
        oscB_pan = 0.0f;
        sub_level = 0.0f;
        noise_level = 0.0f;

        for (int i = 0; i < maxUnisonVoices; ++i)
        {
            unisonOscA[i].reset();
            unisonOscB[i].reset();
        }
        subOsc.reset();
        noiseOsc.reset();
        filter.reset();
        ampEnvelope.reset();
    }

private:
    // Phase 3.4: Unison constant (must be declared before arrays that use it)
    static constexpr int maxUnisonVoices = 16;

    bool isActive = false;
    bool inRelease = false;
    int midiNote = -1;
    float velocity = 0.0f;
    float frequency = 0.0f;
    double sampleRate = 44100.0;

    // Oscillators (Phase 3.4: Arrays for unison expansion)
    std::array<WavetableOscillator, maxUnisonVoices> unisonOscA;
    std::array<WavetableOscillator, maxUnisonVoices> unisonOscB;

    // Oscillator levels and panning
    float oscA_level = 1.0f;
    float oscA_pan = 0.0f;
    float oscB_level = 1.0f;
    float oscB_pan = 0.0f;

    // Phase 3.3: Sub oscillator, Noise oscillator, Filter
    SubOscillator subOsc;
    NoiseOscillator noiseOsc;
    FilterBank filter;

    // Sub and Noise levels
    float sub_level = 0.0f;
    float noise_level = 0.0f;

    juce::ADSR ampEnvelope;

    // Phase 3.4: Unison processing
    int unisonCount = 1; // 1, 2, 4, 8, or 16 voices
    float detuneAmount = 0.0f; // 0.0-0.5 semitones (0-50 cents)
    float stereoSpread = 0.5f; // 0.0-1.0 (0-100%)

    // Pre-calculated detune and pan factors for each unison voice
    std::array<float, maxUnisonVoices> detuneFactors;
    std::array<float, maxUnisonVoices> panFactors;

    // Calculate detune and pan factors for current unison count
    void calculateDetuneFactors()
    {
        if (unisonCount == 1)
        {
            // No unison: single voice, no detune, centered
            detuneFactors[0] = 1.0f;
            panFactors[0] = 0.0f;
            return;
        }

        // Calculate symmetric detune spread
        // Formula: detune = (voiceIndex / (unisonCount-1) - 0.5) * 2.0 * detuneAmount
        // This creates symmetric spread: voice 0 at -detuneAmount, voice N-1 at +detuneAmount
        for (int i = 0; i < unisonCount; ++i)
        {
            // Symmetric offset: -0.5 to +0.5
            float offset = (static_cast<float>(i) / (unisonCount - 1)) - 0.5f;

            // Scale to detune range: -detuneAmount to +detuneAmount (in semitones)
            float detuneSemitones = offset * 2.0f * detuneAmount;

            // Convert semitones to frequency multiplier: freq × 2^(semitones/12)
            detuneFactors[i] = std::pow(2.0f, detuneSemitones / 12.0f);

            // Calculate stereo pan position for this unison voice
            // Voice 0 far left (-1.0), Voice N-1 far right (+1.0)
            panFactors[i] = offset * 2.0f; // -1.0 to +1.0
        }
    }
};
