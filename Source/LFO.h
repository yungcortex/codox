#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>

// LFO.h - Phase 3.6: LFO Generation
// Low-frequency oscillator with 5 waveform shapes
// NOTE: v1.0 has NO routing (no mod matrix) - LFOs generate signals but don't modulate anything
class LFO
{
public:
    enum Shape
    {
        Sine = 0,
        Triangle = 1,
        Saw = 2,
        Square = 3,
        SampleAndHold = 4
    };

    LFO()
    {
        reset();
    }

    void prepareToPlay(double sr)
    {
        sampleRate = sr;
        updatePhaseIncrement();
    }

    void reset()
    {
        phase = 0.0f;
        lastSampleAndHoldValue = 0.0f;
    }

    // Set LFO shape (0-4: Sine, Triangle, Saw, Square, S&H)
    void setShape(int shapeIndex)
    {
        currentShape = static_cast<Shape>(juce::jlimit(0, 4, shapeIndex));
    }

    // Set LFO rate in Hz (0.01-100 Hz)
    void setRate(float rateHz)
    {
        rate = juce::jlimit(0.01f, 100.0f, rateHz);
        updatePhaseIncrement();
    }

    // Set tempo sync (RESERVED for v1.1 - currently inactive)
    void setTempoSync(bool sync)
    {
        tempoSyncEnabled = sync;
        // NOTE: Tempo sync NOT implemented in v1.0 (no host BPM info)
        // This parameter exists but has no effect
    }

    // Get next LFO sample (bipolar output: -1.0 to +1.0)
    float getNextSample()
    {
        float output = 0.0f;

        switch (currentShape)
        {
            case Sine:
                output = std::sin(phase);
                break;

            case Triangle:
                // Triangle wave: piecewise linear
                output = (2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(phase));
                break;

            case Saw:
                // Saw wave: ramp up from -1 to +1
                output = (phase / (2.0f * juce::MathConstants<float>::pi)) * 2.0f - 1.0f;
                break;

            case Square:
                // Square wave: -1 or +1
                output = phase < juce::MathConstants<float>::pi ? 1.0f : -1.0f;
                break;

            case SampleAndHold:
                // Sample & Hold: random value held for entire cycle
                // Update random value when phase wraps
                if (phase < phaseIncrement) // Wrapped from 2π to 0
                {
                    lastSampleAndHoldValue = randomGenerator.nextFloat() * 2.0f - 1.0f; // -1.0 to +1.0
                }
                output = lastSampleAndHoldValue;
                break;
        }

        // Advance phase
        phase += phaseIncrement;

        // Wrap phase to [0, 2π)
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;

        return output;
    }

    // Get current LFO output without advancing phase (for visualization)
    float getCurrentValue() const
    {
        float output = 0.0f;

        switch (currentShape)
        {
            case Sine:
                output = std::sin(phase);
                break;

            case Triangle:
                output = (2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(phase));
                break;

            case Saw:
                output = (phase / (2.0f * juce::MathConstants<float>::pi)) * 2.0f - 1.0f;
                break;

            case Square:
                output = phase < juce::MathConstants<float>::pi ? 1.0f : -1.0f;
                break;

            case SampleAndHold:
                output = lastSampleAndHoldValue;
                break;
        }

        return output;
    }

private:
    Shape currentShape = Sine;
    float rate = 1.0f; // Hz
    bool tempoSyncEnabled = false; // RESERVED for v1.1 (currently inactive)

    float phase = 0.0f; // Current phase (0 to 2π)
    float phaseIncrement = 0.0f; // Phase increment per sample
    double sampleRate = 44100.0;

    // Sample & Hold state
    float lastSampleAndHoldValue = 0.0f;
    juce::Random randomGenerator;

    // Update phase increment based on rate and sample rate
    void updatePhaseIncrement()
    {
        // phase_increment = (2π × frequency) / sampleRate
        phaseIncrement = (2.0f * juce::MathConstants<float>::pi * rate) / static_cast<float>(sampleRate);
    }
};
