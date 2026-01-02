#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

// SubOscillator - Phase 3.3: Simple oscillator with waveform selection
// Generates sine, triangle, or square waves at -2, -1, or 0 octaves below base note
class SubOscillator
{
public:
    SubOscillator() = default;

    // Set waveform shape (0=Sine, 1=Triangle, 2=Square)
    void setShape(int shapeIndex)
    {
        shape = juce::jlimit(0, 2, shapeIndex);
    }

    // Set octave offset (-2, -1, 0 relative to base note)
    void setOctaveOffset(int octaveIndex)
    {
        // octaveIndex: 0=-2, 1=-1, 2=0
        octaveOffset = -2 + octaveIndex;
        octaveOffset = juce::jlimit(-2, 0, octaveOffset);
    }

    // Set frequency (base note frequency, will be offset by octave)
    void setFrequency(float baseFreq, double sr)
    {
        sampleRate = sr;
        // Apply octave offset: freq × 2^octave
        frequency = baseFreq * std::pow(2.0f, static_cast<float>(octaveOffset));

        // Calculate phase increment per sample
        phaseIncrement = (frequency * juce::MathConstants<float>::twoPi) / static_cast<float>(sampleRate);
    }

    // Generate next sample
    float getNextSample()
    {
        float output = 0.0f;

        switch (shape)
        {
            case 0: // Sine
                output = std::sin(phase);
                break;

            case 1: // Triangle
                // Triangle wave using arcsin formula: output = (2/π) × arcsin(sin(phase))
                output = (2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(phase));
                break;

            case 2: // Square
                // Square wave: positive for first half of cycle, negative for second half
                output = phase < juce::MathConstants<float>::pi ? 1.0f : -1.0f;
                break;

            default:
                output = 0.0f;
        }

        // Increment phase and wrap
        phase += phaseIncrement;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        return output;
    }

    // Reset oscillator state
    void reset()
    {
        phase = 0.0f;
        frequency = 0.0f;
        phaseIncrement = 0.0f;
    }

private:
    int shape = 0;              // 0=Sine, 1=Triangle, 2=Square
    int octaveOffset = -1;      // -2, -1, or 0 octaves
    float phase = 0.0f;         // Current phase (0 to 2π)
    float frequency = 0.0f;     // Current frequency (Hz)
    float phaseIncrement = 0.0f; // Phase increment per sample
    double sampleRate = 44100.0;
};
