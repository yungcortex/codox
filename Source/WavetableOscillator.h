#pragma once
#include "Wavetable.h"
#include <cmath>

// Wavetable oscillator with frame interpolation and warp modes
class WavetableOscillator
{
public:
    WavetableOscillator()
    {
        currentWavetable = &Wavetable::getWavetable(Wavetable::Type::Basic);
    }

    // Set wavetable type
    void setWavetable(int wavetableIndex)
    {
        wavetableIndex = juce::jlimit(0, static_cast<int>(Wavetable::Type::Count) - 1, wavetableIndex);
        currentWavetable = &Wavetable::getWavetable(static_cast<Wavetable::Type>(wavetableIndex));
    }

    // Set frame position (0.0 to 1.0 → frame 0 to 255)
    void setPosition(float pos)
    {
        position = juce::jlimit(0.0f, 1.0f, pos);
    }

    // Set pitch (note frequency with octave/semitone/fine tuning)
    void setFrequency(float baseFreq, int octave, int semitone, int fineCents, double sampleRate)
    {
        // Octave shift: frequency × 2^octave
        float octaveMultiplier = std::pow(2.0f, static_cast<float>(octave));

        // Semitone shift: frequency × 2^(semi/12)
        float semitoneMultiplier = std::pow(2.0f, semitone / 12.0f);

        // Fine tuning: frequency × 2^(cents/1200)
        float fineMultiplier = std::pow(2.0f, fineCents / 1200.0f);

        // Combined frequency
        frequency = baseFreq * octaveMultiplier * semitoneMultiplier * fineMultiplier;

        // Calculate phase increment
        phaseIncrement = (frequency / static_cast<float>(sampleRate)) * Wavetable::samplesPerFrame;
    }

    // Set warp mode and amount (Phase 3.2c implementation - not yet active)
    void setWarpMode(int mode)
    {
        warpMode = juce::jlimit(0, 4, mode); // 0-4: Sync, Bend+, FM, AM, PWM
    }

    void setWarpAmount(float amount)
    {
        warpAmount = juce::jlimit(0.0f, 1.0f, amount);
    }

    // Reset phase
    void reset()
    {
        phase = 0.0f;
        syncPhase = 0.0f;
    }

    // Get next sample (with frame interpolation)
    float getNextSample()
    {
        if (currentWavetable == nullptr)
            return 0.0f;

        // Calculate frame index from position (0.0 to 1.0 → 0 to 255)
        float frameIndexFloat = position * (Wavetable::numFrames - 1);
        int frame1 = static_cast<int>(frameIndexFloat);
        int frame2 = std::min(frame1 + 1, Wavetable::numFrames - 1);
        float frameFrac = frameIndexFloat - frame1; // Fractional part for interpolation

        // Get sample index within frame
        int sampleIndex = static_cast<int>(phase) % Wavetable::samplesPerFrame;

        // Read samples from both frames
        float sample1 = (*currentWavetable)[frame1][sampleIndex];
        float sample2 = (*currentWavetable)[frame2][sampleIndex];

        // Linear interpolation between frames
        float output = sample1 + frameFrac * (sample2 - sample1);

        // Advance phase
        phase += phaseIncrement;
        if (phase >= Wavetable::samplesPerFrame)
            phase -= Wavetable::samplesPerFrame;

        // Apply warp modes (Phase 3.2c - placeholder for now, returns unmodified output)
        output = applyWarp(output);

        return output;
    }

    // Get next sample with custom position (for morphing automation)
    float getNextSampleWithPosition(float pos)
    {
        setPosition(pos);
        return getNextSample();
    }

private:
    const Wavetable::WavetableData* currentWavetable = nullptr;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    float frequency = 440.0f;
    float position = 0.0f; // 0.0 to 1.0

    // Warp parameters (Phase 3.2c - implemented)
    int warpMode = 0; // 0: Sync, 1: Bend+, 2: FM, 3: AM, 4: PWM
    float warpAmount = 0.0f;
    float syncPhase = 0.0f; // Sync oscillator phase (for hard sync mode)

    // Apply warp mode processing (Phase 3.2c - implemented)
    float applyWarp(float sample)
    {
        if (warpAmount < 0.001f)
            return sample; // No warp applied

        switch (warpMode)
        {
            case 0: // Sync - Hard sync with phase resets
                return applySync(sample);

            case 1: // Bend+ - Asymmetric phase distortion (compress first half)
                return applyBendPlus(sample);

            case 2: // FM - Frequency modulation (self-modulation for now)
                return applyFM(sample);

            case 3: // AM - Amplitude modulation (self-modulation creates ring mod effect)
                return applyAM(sample);

            case 4: // PWM - Pulse width modulation
                return applyPWM(sample);

            default:
                return sample;
        }
    }

    // Warp mode implementations
    float applySync(float sample)
    {
        // Hard sync: Phase resets at warp_amount frequency
        // Higher warp amount = more frequent resets = more harmonics
        float syncFreq = frequency * (1.0f + warpAmount * 4.0f); // Up to 5x base frequency
        float syncPhaseIncrement = (syncFreq / static_cast<float>(getSampleRate())) * Wavetable::samplesPerFrame;

        syncPhase += syncPhaseIncrement;
        if (syncPhase >= Wavetable::samplesPerFrame)
        {
            syncPhase -= Wavetable::samplesPerFrame;
            phase = 0.0f; // Reset main oscillator phase (hard sync)
        }

        return sample;
    }

    float applyBendPlus(float sample)
    {
        // Bend+: Asymmetric phase distortion (compress first half of waveform)
        // This stretches the waveform, shifting harmonics upward
        float phaseNormalized = phase / Wavetable::samplesPerFrame; // 0.0 to 1.0

        // Apply non-linear phase warping (compress first half based on warp amount)
        float bendFactor = 1.0f + warpAmount * 2.0f; // 1.0 to 3.0
        float warpedPhase = phaseNormalized;

        if (phaseNormalized < 0.5f)
        {
            // Compress first half
            warpedPhase = phaseNormalized * (1.0f / bendFactor);
        }
        else
        {
            // Stretch second half to compensate
            warpedPhase = (1.0f / bendFactor) + (phaseNormalized - 0.5f) * (1.0f - 1.0f / bendFactor) / 0.5f;
        }

        // Read sample at warped phase position
        int warpedSampleIndex = static_cast<int>(warpedPhase * Wavetable::samplesPerFrame) % Wavetable::samplesPerFrame;

        if (currentWavetable == nullptr)
            return sample;

        float frameIndexFloat = position * (Wavetable::numFrames - 1);
        int frame1 = static_cast<int>(frameIndexFloat);
        int frame2 = std::min(frame1 + 1, Wavetable::numFrames - 1);
        float frameFrac = frameIndexFloat - frame1;

        float warpedSample1 = (*currentWavetable)[frame1][warpedSampleIndex];
        float warpedSample2 = (*currentWavetable)[frame2][warpedSampleIndex];
        float warpedSample = warpedSample1 + frameFrac * (warpedSample2 - warpedSample1);

        // Mix dry/wet based on warp amount
        return sample * (1.0f - warpAmount) + warpedSample * warpAmount;
    }

    float applyFM(float sample)
    {
        // FM: Frequency modulation (self-modulation)
        // Use current sample as modulator to shift phase
        float modDepth = warpAmount * 10.0f; // FM depth (up to 10 samples phase shift)
        float phaseOffset = sample * modDepth * Wavetable::samplesPerFrame;

        float modulatedPhase = phase + phaseOffset;
        while (modulatedPhase >= Wavetable::samplesPerFrame)
            modulatedPhase -= Wavetable::samplesPerFrame;
        while (modulatedPhase < 0.0f)
            modulatedPhase += Wavetable::samplesPerFrame;

        int modulatedSampleIndex = static_cast<int>(modulatedPhase) % Wavetable::samplesPerFrame;

        if (currentWavetable == nullptr)
            return sample;

        float frameIndexFloat = position * (Wavetable::numFrames - 1);
        int frame1 = static_cast<int>(frameIndexFloat);
        int frame2 = std::min(frame1 + 1, Wavetable::numFrames - 1);
        float frameFrac = frameIndexFloat - frame1;

        float modulatedSample1 = (*currentWavetable)[frame1][modulatedSampleIndex];
        float modulatedSample2 = (*currentWavetable)[frame2][modulatedSampleIndex];
        float modulatedSample = modulatedSample1 + frameFrac * (modulatedSample2 - modulatedSample1);

        // Mix dry/wet
        return sample * (1.0f - warpAmount) + modulatedSample * warpAmount;
    }

    float applyAM(float sample)
    {
        // AM: Amplitude modulation (ring modulation effect)
        // Modulate amplitude using oscillator's own output
        // At 100% warp, creates classic ring mod sound
        float modulator = sample; // Self-modulation
        float modulated = sample * (1.0f + modulator * warpAmount);

        return modulated;
    }

    float applyPWM(float sample)
    {
        // PWM: Pulse width modulation
        // Creates pulse-wave like effect by thresholding with variable width
        float phaseNormalized = phase / Wavetable::samplesPerFrame; // 0.0 to 1.0

        // Pulse width controlled by warp amount (0.1 to 0.9)
        float pulseWidth = 0.1f + warpAmount * 0.8f;

        // Create pulse wave
        float pulseWave = (phaseNormalized < pulseWidth) ? 1.0f : -1.0f;

        // Mix with original sample based on warp amount
        return sample * (1.0f - warpAmount) + pulseWave * warpAmount * 0.5f; // Scale down pulse to avoid clipping
    }

    // Helper to get sample rate (needed for sync mode)
    double getSampleRate() const
    {
        // Calculate from phase increment and frequency
        // phaseIncrement = (frequency / sampleRate) * samplesPerFrame
        // Therefore: sampleRate = (frequency * samplesPerFrame) / phaseIncrement
        if (phaseIncrement > 0.0f)
            return (frequency * Wavetable::samplesPerFrame) / phaseIncrement;
        return 44100.0; // Fallback
    }
};
