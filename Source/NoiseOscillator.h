#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

// NoiseOscillator - Phase 3.3: Noise generator with type selection
// Generates white, pink, or digital noise
class NoiseOscillator
{
public:
    NoiseOscillator()
    {
        // Initialize pink noise filter state
        pinkState[0] = 0.0f;
        pinkState[1] = 0.0f;
        pinkState[2] = 0.0f;
        pinkState[3] = 0.0f;
        pinkState[4] = 0.0f;
        pinkState[5] = 0.0f;
        pinkState[6] = 0.0f;
    }

    // Set noise type (0=White, 1=Pink, 2=Digital)
    void setType(int typeIndex)
    {
        noiseType = juce::jlimit(0, 2, typeIndex);
    }

    // Generate next noise sample
    float getNextSample()
    {
        float output = 0.0f;

        switch (noiseType)
        {
            case 0: // White noise
                output = generateWhiteNoise();
                break;

            case 1: // Pink noise (1/f spectrum)
                output = generatePinkNoise();
                break;

            case 2: // Digital noise (bit-crushed white noise)
                output = generateDigitalNoise();
                break;

            default:
                output = 0.0f;
        }

        return output;
    }

    // Reset oscillator state
    void reset()
    {
        // Reset pink noise filter state
        for (int i = 0; i < 7; ++i)
            pinkState[i] = 0.0f;

        digitalNoiseCounter = 0;
        digitalNoiseHold = 0.0f;
    }

private:
    int noiseType = 0; // 0=White, 1=Pink, 2=Digital

    // White noise: uniform random distribution [-1, +1]
    float generateWhiteNoise()
    {
        return random.nextFloat() * 2.0f - 1.0f;
    }

    // Pink noise: Paul Kellet's algorithm (7-pole IIR filter)
    // Approximates 1/f spectrum (equal energy per octave)
    float pinkState[7];

    float generatePinkNoise()
    {
        float white = generateWhiteNoise();

        // Apply 7-pole IIR filter to white noise
        pinkState[0] = 0.99886f * pinkState[0] + white * 0.0555179f;
        pinkState[1] = 0.99332f * pinkState[1] + white * 0.0750759f;
        pinkState[2] = 0.96900f * pinkState[2] + white * 0.1538520f;
        pinkState[3] = 0.86650f * pinkState[3] + white * 0.3104856f;
        pinkState[4] = 0.55000f * pinkState[4] + white * 0.5329522f;
        pinkState[5] = -0.7616f * pinkState[5] - white * 0.0168980f;

        float pink = pinkState[0] + pinkState[1] + pinkState[2] + pinkState[3] +
                     pinkState[4] + pinkState[5] + pinkState[6] + white * 0.5362f;

        pinkState[6] = white * 0.115926f;

        // Apply compensation gain
        return pink * 0.11f;
    }

    // Digital noise: bit-crushed/sample-and-hold white noise
    // Creates lo-fi, aliased, retro digital artifacts
    int digitalNoiseCounter = 0;
    float digitalNoiseHold = 0.0f;
    static constexpr int digitalNoiseRate = 16; // Hold sample for 16 samples

    float generateDigitalNoise()
    {
        // Generate new random value every N samples (sample-and-hold)
        if (digitalNoiseCounter <= 0)
        {
            digitalNoiseHold = generateWhiteNoise();
            digitalNoiseCounter = digitalNoiseRate;
        }

        digitalNoiseCounter--;
        return digitalNoiseHold;
    }

    juce::Random random;
};
