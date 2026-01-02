#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "WavetableOscillator.h"
#include "SubOscillator.h"
#include "NoiseOscillator.h"
#include "FilterBank.h"

// Voice class - Phase 3.3: Added Sub, Noise, and Filter Bank
// Complete subtractive synthesis path: Oscillators → Mix → Filter → Amp Envelope
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
        sampleRate = sr;

        // Calculate base frequency from MIDI note
        frequency = 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);

        // Reset oscillators
        oscA.reset();
        oscB.reset();
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
        oscA.setWavetable(wavetable);
        oscA.setPosition(position / 100.0f); // Convert 0-100% to 0.0-1.0
        oscA.setWarpMode(warpMode);
        oscA.setWarpAmount(warpAmount / 100.0f); // Convert 0-100% to 0.0-1.0

        // Update frequency with tuning
        oscA.setFrequency(frequency, octave - 4, semitone, fine, sampleRate); // octave param is index 0-8, convert to -4 to +4

        // Store level and pan for mixing
        oscA_level = level / 100.0f; // Convert 0-100% to 0.0-1.0
        oscA_pan = pan / 100.0f; // Convert -100 to +100 to -1.0 to +1.0
    }

    // Update oscillator B parameters
    void updateOscillatorB(int wavetable, float position, float level, float pan,
                           int octave, int semitone, int fine,
                           int warpMode, float warpAmount)
    {
        oscB.setWavetable(wavetable);
        oscB.setPosition(position / 100.0f);
        oscB.setWarpMode(warpMode);
        oscB.setWarpAmount(warpAmount / 100.0f);

        oscB.setFrequency(frequency, octave - 4, semitone, fine, sampleRate);

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

    // Generate next audio sample (mono - for backward compatibility)
    float getNextSample()
    {
        if (!isActive)
            return 0.0f;

        // Generate samples from both oscillators
        float sampleA = oscA.getNextSample() * oscA_level;
        float sampleB = oscB.getNextSample() * oscB_level;

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

    // Generate next stereo sample pair (Phase 3.3 - Added Sub, Noise, Filter)
    void getNextSampleStereo(float& leftOut, float& rightOut)
    {
        if (!isActive)
        {
            leftOut = 0.0f;
            rightOut = 0.0f;
            return;
        }

        // Generate samples from both wavetable oscillators
        float sampleA = oscA.getNextSample() * oscA_level;
        float sampleB = oscB.getNextSample() * oscB_level;

        // Constant-power panning for Osc A
        // Convert pan from -1..1 to 0..1 range
        float panA = (oscA_pan + 1.0f) * 0.5f;
        // Apply constant-power law using cos/sin (maintains perceived loudness)
        float gainL_A = std::cos(panA * juce::MathConstants<float>::halfPi);
        float gainR_A = std::sin(panA * juce::MathConstants<float>::halfPi);

        // Constant-power panning for Osc B
        float panB = (oscB_pan + 1.0f) * 0.5f;
        float gainL_B = std::cos(panB * juce::MathConstants<float>::halfPi);
        float gainR_B = std::sin(panB * juce::MathConstants<float>::halfPi);

        // Mix wavetable oscillators with panning applied
        float leftMix = (sampleA * gainL_A) + (sampleB * gainL_B);
        float rightMix = (sampleA * gainR_A) + (sampleB * gainR_B);

        // Add sub oscillator (mono, centered - no pan)
        float subSample = subOsc.getNextSample() * sub_level;
        leftMix += subSample;
        rightMix += subSample;

        // Add noise oscillator (stereo, same noise to both channels)
        float noiseSample = noiseOsc.getNextSample() * noise_level;
        leftMix += noiseSample;
        rightMix += noiseSample;

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

        oscA.reset();
        oscB.reset();
        subOsc.reset();
        noiseOsc.reset();
        filter.reset();
        ampEnvelope.reset();
    }

private:
    bool isActive = false;
    bool inRelease = false;
    int midiNote = -1;
    float velocity = 0.0f;
    float frequency = 0.0f;
    double sampleRate = 44100.0;

    // Oscillators (Phase 3.2a: Wavetable oscillators)
    WavetableOscillator oscA;
    WavetableOscillator oscB;

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
};
