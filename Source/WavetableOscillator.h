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

    // Warp parameters (Phase 3.2c - not yet implemented)
    int warpMode = 0; // 0: Sync, 1: Bend+, 2: FM, 3: AM, 4: PWM
    float warpAmount = 0.0f;

    // Apply warp mode processing (Phase 3.2c placeholder - returns unmodified)
    float applyWarp(float sample)
    {
        // Phase 3.2c will implement warp modes here
        // For now, return unmodified sample
        juce::ignoreUnused(warpMode, warpAmount);
        return sample;
    }
};
