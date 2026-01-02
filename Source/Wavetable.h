#pragma once
#include <juce_core/juce_core.h>
#include <array>
#include <vector>
#include <cmath>

// Wavetable format: 256 frames × 2048 samples per frame (Serum-compatible)
class Wavetable
{
public:
    static constexpr int numFrames = 256;
    static constexpr int samplesPerFrame = 2048;

    using Frame = std::array<float, samplesPerFrame>;
    using WavetableData = std::array<Frame, numFrames>;

    // Wavetable types
    enum class Type
    {
        Basic = 0,
        Analog,
        Digital,
        Vocal,
        Count
    };

    // Get built-in wavetable by type
    static const WavetableData& getWavetable(Type type)
    {
        static bool initialized = false;
        if (!initialized)
        {
            initializeWavetables();
            initialized = true;
        }

        return wavetables[static_cast<int>(type)];
    }

private:
    // Storage for 4 built-in wavetables (inline for header-only, C++17)
    inline static std::array<WavetableData, static_cast<int>(Type::Count)> wavetables;

    // Generate built-in wavetables
    static void initializeWavetables()
    {
        // BASIC WAVETABLE: Morphing from sine → saw → square → triangle
        // Frame 0-63: Sine → Saw
        // Frame 64-127: Saw → Square
        // Frame 128-191: Square → Triangle
        // Frame 192-255: Triangle → Sine
        for (int frame = 0; frame < numFrames; ++frame)
        {
            float morphPos = frame / static_cast<float>(numFrames - 1); // 0.0 to 1.0

            for (int sample = 0; sample < samplesPerFrame; ++sample)
            {
                float phase = (sample / static_cast<float>(samplesPerFrame)) * 2.0f * juce::MathConstants<float>::pi;

                // Generate 4 base waveforms
                float sine = std::sin(phase);
                float saw = (phase / juce::MathConstants<float>::pi) - 1.0f; // -1 to +1
                float square = phase < juce::MathConstants<float>::pi ? 1.0f : -1.0f;
                float triangle = (2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(phase));

                // Morph between waveforms based on frame position
                float output = 0.0f;
                if (morphPos < 0.25f)
                {
                    // Sine → Saw
                    float t = morphPos / 0.25f;
                    output = sine * (1.0f - t) + saw * t;
                }
                else if (morphPos < 0.5f)
                {
                    // Saw → Square
                    float t = (morphPos - 0.25f) / 0.25f;
                    output = saw * (1.0f - t) + square * t;
                }
                else if (morphPos < 0.75f)
                {
                    // Square → Triangle
                    float t = (morphPos - 0.5f) / 0.25f;
                    output = square * (1.0f - t) + triangle * t;
                }
                else
                {
                    // Triangle → Sine
                    float t = (morphPos - 0.75f) / 0.25f;
                    output = triangle * (1.0f - t) + sine * t;
                }

                wavetables[static_cast<int>(Type::Basic)][frame][sample] = output;
            }
        }

        // ANALOG WAVETABLE: Band-limited classic analog waveforms
        // Frame 0-127: Saw with decreasing harmonics
        // Frame 128-255: Pulse with varying width
        for (int frame = 0; frame < numFrames; ++frame)
        {
            float morphPos = frame / static_cast<float>(numFrames - 1);

            for (int sample = 0; sample < samplesPerFrame; ++sample)
            {
                float phase = (sample / static_cast<float>(samplesPerFrame)) * 2.0f * juce::MathConstants<float>::pi;

                float output = 0.0f;

                if (morphPos < 0.5f)
                {
                    // Band-limited sawtooth with decreasing harmonics
                    int maxHarmonic = static_cast<int>(1 + morphPos * 20.0f); // 1 to 11 harmonics
                    for (int h = 1; h <= maxHarmonic; ++h)
                    {
                        float harmonic = std::sin(phase * h) / h;
                        output += harmonic;
                    }
                    output *= 0.5f; // Normalize
                }
                else
                {
                    // Pulse width modulation
                    float pulseWidth = 0.1f + (morphPos - 0.5f) * 1.6f; // 0.1 to 0.9
                    output = (phase / (2.0f * juce::MathConstants<float>::pi)) < pulseWidth ? 1.0f : -1.0f;
                }

                wavetables[static_cast<int>(Type::Analog)][frame][sample] = output;
            }
        }

        // DIGITAL WAVETABLE: Harsh digital waveforms with aliasing
        // Frame 0-127: Bit-crushed sine
        // Frame 128-255: Sample-rate reduced square
        for (int frame = 0; frame < numFrames; ++frame)
        {
            float morphPos = frame / static_cast<float>(numFrames - 1);

            for (int sample = 0; sample < samplesPerFrame; ++sample)
            {
                float phase = (sample / static_cast<float>(samplesPerFrame)) * 2.0f * juce::MathConstants<float>::pi;

                float output = 0.0f;

                if (morphPos < 0.5f)
                {
                    // Bit-crushed sine
                    int bitDepth = static_cast<int>(2 + morphPos * 28.0f); // 2 to 16 bits
                    float sine = std::sin(phase);
                    int quantize = static_cast<int>(std::pow(2.0f, bitDepth));
                    output = std::floor(sine * quantize) / quantize;
                }
                else
                {
                    // Sample-rate reduced square
                    int srReduction = static_cast<int>(1 + (morphPos - 0.5f) * 30.0f); // 1 to 16
                    int quantizedSample = (sample / srReduction) * srReduction;
                    float quantizedPhase = (quantizedSample / static_cast<float>(samplesPerFrame)) * 2.0f * juce::MathConstants<float>::pi;
                    output = quantizedPhase < juce::MathConstants<float>::pi ? 1.0f : -1.0f;
                }

                wavetables[static_cast<int>(Type::Digital)][frame][sample] = output;
            }
        }

        // VOCAL WAVETABLE: Formant-like vowel transitions
        // Frame 0-63: "Ah" formant
        // Frame 64-127: "Eh" formant
        // Frame 128-191: "Ee" formant
        // Frame 192-255: "Oh" formant
        for (int frame = 0; frame < numFrames; ++frame)
        {
            float morphPos = frame / static_cast<float>(numFrames - 1);

            for (int sample = 0; sample < samplesPerFrame; ++sample)
            {
                float phase = (sample / static_cast<float>(samplesPerFrame)) * 2.0f * juce::MathConstants<float>::pi;

                // Create formant-like harmonics (simplified vowel approximation)
                float output = 0.0f;

                // Base frequency
                float fundamental = std::sin(phase);

                // Formant peaks (simplified - real formants are more complex)
                float formant1 = 0.0f, formant2 = 0.0f, formant3 = 0.0f;

                if (morphPos < 0.25f)
                {
                    // "Ah" - F1=730, F2=1090, F3=2440 (approx harmonics 3, 5, 10)
                    formant1 = std::sin(phase * 3.0f) * 0.6f;
                    formant2 = std::sin(phase * 5.0f) * 0.4f;
                    formant3 = std::sin(phase * 10.0f) * 0.2f;
                }
                else if (morphPos < 0.5f)
                {
                    // "Eh" - F1=530, F2=1840, F3=2480 (approx harmonics 2, 7, 10)
                    formant1 = std::sin(phase * 2.0f) * 0.6f;
                    formant2 = std::sin(phase * 7.0f) * 0.5f;
                    formant3 = std::sin(phase * 10.0f) * 0.2f;
                }
                else if (morphPos < 0.75f)
                {
                    // "Ee" - F1=270, F2=2290, F3=3010 (approx harmonics 1, 9, 12)
                    formant1 = std::sin(phase * 1.0f) * 0.5f;
                    formant2 = std::sin(phase * 9.0f) * 0.6f;
                    formant3 = std::sin(phase * 12.0f) * 0.3f;
                }
                else
                {
                    // "Oh" - F1=570, F2=840, F3=2410 (approx harmonics 2, 3, 10)
                    formant1 = std::sin(phase * 2.0f) * 0.7f;
                    formant2 = std::sin(phase * 3.0f) * 0.5f;
                    formant3 = std::sin(phase * 10.0f) * 0.2f;
                }

                output = fundamental * 0.3f + formant1 + formant2 + formant3;
                output *= 0.3f; // Normalize to prevent clipping

                wavetables[static_cast<int>(Type::Vocal)][frame][sample] = output;
            }
        }
    }
};
