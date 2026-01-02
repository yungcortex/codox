#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

// Simple Voice class for Phase 3.1 - Basic sine oscillator with amp envelope
// This validates the voice management architecture before implementing the full wavetable engine
class Voice
{
public:
    Voice()
    {
        ampEnvelope.setSampleRate(44100.0);
        ampEnvelope.setParameters({0.01f, 0.1f, 1.0f, 0.2f}); // Default ADSR
    }

    // Note-on: trigger voice
    void noteOn(int noteNumber, float vel, double sr)
    {
        midiNote = noteNumber;
        velocity = vel;
        isActive = true;

        // Calculate frequency from MIDI note
        frequency = 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);

        // Reset phase
        phase = 0.0f;
        phaseIncrement = (2.0f * juce::MathConstants<float>::pi * frequency) / static_cast<float>(sr);

        // Reset release flag and trigger envelope
        inRelease = false;
        ampEnvelope.noteOn();
    }

    // Note-off: release voice
    void noteOff()
    {
        ampEnvelope.noteOff();
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

    // Generate next audio sample (simple sine wave for Phase 3.1)
    float getNextSample()
    {
        if (!isActive)
            return 0.0f;

        // Simple sine oscillator
        float sample = std::sin(phase);

        // Apply velocity
        sample *= velocity;

        // Apply amp envelope - get envelope value
        float envValue = ampEnvelope.getNextSample();
        sample *= envValue;

        // Advance phase
        phase += phaseIncrement;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;

        // Check if envelope is finished - ADSR returns 0 after release phase completes
        // Only check after noteOff has been called (inRelease flag)
        if (inRelease && envValue < 0.0001f)
            isActive = false;

        return sample;
    }

    // Update envelope parameters
    void updateEnvelope(float attack, float decay, float sustain, float release)
    {
        ampEnvelope.setParameters({attack, decay, sustain, release});
    }

    // Prepare to play (update sample rate)
    void prepareToPlay(double sampleRate)
    {
        ampEnvelope.setSampleRate(sampleRate);

        // Recalculate phase increment if voice is active
        if (isActive)
            phaseIncrement = (2.0f * juce::MathConstants<float>::pi * frequency) / static_cast<float>(sampleRate);
    }

    // Reset voice state
    void reset()
    {
        isActive = false;
        inRelease = false;
        midiNote = -1;
        velocity = 0.0f;
        frequency = 0.0f;
        phase = 0.0f;
        phaseIncrement = 0.0f;
        ampEnvelope.reset();
    }

private:
    bool isActive = false;
    bool inRelease = false;
    int midiNote = -1;
    float velocity = 0.0f;
    float frequency = 0.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;

    juce::ADSR ampEnvelope;
};
