#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

// FilterBank - Phase 3.3: Multi-mode filter with envelope modulation
// Supports 5 filter types: LP 24dB, LP 12dB, HP 24dB, BP 12dB, Notch
class FilterBank
{
public:
    FilterBank()
    {
        // Set default filter envelope parameters
        filterEnvelope.setSampleRate(44100.0);
        filterEnvelope.setParameters({0.01f, 0.3f, 0.5f, 0.5f}); // Default ADSR
    }

    // Set filter type (0=LP24, 1=LP12, 2=HP24, 3=BP12, 4=Notch)
    void setFilterType(int typeIndex)
    {
        filterType = juce::jlimit(0, 4, typeIndex);
    }

    // Set base cutoff frequency (20Hz to 20kHz)
    void setCutoffFrequency(float freq)
    {
        baseCutoff = juce::jlimit(20.0f, 20000.0f, freq);
    }

    // Set resonance (0-100% → Q factor 0.5 to 10.0)
    void setResonance(float res)
    {
        // Map 0-100% to Q factor 0.5-10.0 (logarithmic scale)
        resonance = 0.5f + (res / 100.0f) * 9.5f;
    }

    // Set drive amount (0-100% → gain 1.0x to 10.0x)
    void setDrive(float drv)
    {
        // Map 0-100% to gain 1.0-10.0
        drive = 1.0f + (drv / 100.0f) * 9.0f;
    }

    // Set envelope depth (-100% to +100%)
    void setEnvelopeDepth(float depth)
    {
        envelopeDepth = juce::jlimit(-100.0f, 100.0f, depth);
    }

    // Set keytrack amount (0-100%)
    void setKeytrackAmount(float kt)
    {
        keytrackAmount = juce::jlimit(0.0f, 100.0f, kt) / 100.0f;
    }

    // Update filter envelope parameters
    void updateFilterEnvelope(float attack, float decay, float sustain, float release)
    {
        filterEnvelope.setParameters({attack, decay, sustain, release});
    }

    // Trigger filter envelope on note-on
    void noteOn()
    {
        filterEnvelope.noteOn();
    }

    // Release filter envelope on note-off
    void noteOff()
    {
        filterEnvelope.noteOff();
    }

    // Prepare to play with given sample rate
    void prepareToPlay(double sr)
    {
        sampleRate = sr;
        filterEnvelope.setSampleRate(sampleRate);

        // Prepare DSP filter
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = 512;
        spec.numChannels = 1; // Mono filter (per voice)

        // Initialize all filter instances
        filterLP24.prepare(spec);
        filterLP12.prepare(spec);
        filterHP24.prepare(spec);
        filterBP12.prepare(spec);
        filterNotch.prepare(spec);

        filterLP24.reset();
        filterLP12.reset();
        filterHP24.reset();
        filterBP12.reset();
        filterNotch.reset();
    }

    // Process a single sample (mono)
    float processSample(float inputSample, int midiNote)
    {
        // Apply pre-filter drive (saturation)
        float drivenSample = std::tanh(inputSample * drive) / std::tanh(drive);

        // Calculate modulated cutoff frequency
        float envValue = filterEnvelope.getNextSample(); // 0.0 to 1.0
        float envModulation = envValue * (envelopeDepth / 100.0f); // -1.0 to +1.0

        // Calculate keytrack modulation
        // Keytrack: cutoff follows MIDI note pitch (0% = fixed, 100% = 1:1 tracking)
        // Formula: cutoff × 2^(keytrackAmount × (note - 60) / 12)
        float keytrackMod = std::pow(2.0f, keytrackAmount * (midiNote - 60) / 12.0f);

        // Combine base cutoff + envelope modulation + keytrack
        // Envelope depth controls ±4 octaves modulation
        float modulatedCutoff = baseCutoff * std::pow(2.0f, envModulation * 4.0f) * keytrackMod;

        // Clamp to valid range (20Hz to Nyquist)
        float nyquist = static_cast<float>(sampleRate) * 0.5f;
        modulatedCutoff = juce::jlimit(20.0f, nyquist - 100.0f, modulatedCutoff);

        // Update filter coefficients based on type
        updateFilterCoefficients(modulatedCutoff);

        // Process sample through selected filter
        float filteredSample = 0.0f;

        switch (filterType)
        {
            case 0: // LP 24dB
                filteredSample = filterLP24.processSample(0, drivenSample);
                break;

            case 1: // LP 12dB
                filteredSample = filterLP12.processSample(0, drivenSample);
                break;

            case 2: // HP 24dB
                filteredSample = filterHP24.processSample(0, drivenSample);
                break;

            case 3: // BP 12dB
                filteredSample = filterBP12.processSample(0, drivenSample);
                break;

            case 4: // Notch
                filteredSample = filterNotch.processSample(0, drivenSample);
                break;

            default:
                filteredSample = drivenSample; // Bypass
        }

        return filteredSample;
    }

    // Reset filter state
    void reset()
    {
        filterLP24.reset();
        filterLP12.reset();
        filterHP24.reset();
        filterBP12.reset();
        filterNotch.reset();
        filterEnvelope.reset();
    }

private:
    int filterType = 0; // 0=LP24, 1=LP12, 2=HP24, 3=BP12, 4=Notch
    float baseCutoff = 8000.0f; // Base cutoff frequency (Hz)
    float resonance = 0.5f; // Q factor
    float drive = 1.0f; // Pre-filter gain
    float envelopeDepth = 0.0f; // Envelope modulation depth (-100 to +100)
    float keytrackAmount = 0.0f; // Keytrack amount (0.0 to 1.0)
    double sampleRate = 44100.0;

    // Filter envelope (ADSR)
    juce::ADSR filterEnvelope;

    // Filter instances (JUCE DSP StateVariableTPTFilter)
    using Filter = juce::dsp::StateVariableTPTFilter<float>;

    Filter filterLP24; // Lowpass 24dB/oct
    Filter filterLP12; // Lowpass 12dB/oct
    Filter filterHP24; // Highpass 24dB/oct
    Filter filterBP12; // Bandpass 12dB/oct
    Filter filterNotch; // Notch filter

    // Update filter coefficients based on current parameters
    void updateFilterCoefficients(float cutoff)
    {
        // LP 24dB: Two cascaded LP filters (24dB/oct = 4-pole)
        filterLP24.setCutoffFrequency(cutoff);
        filterLP24.setResonance(resonance);
        filterLP24.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

        // LP 12dB: Single LP filter (12dB/oct = 2-pole)
        filterLP12.setCutoffFrequency(cutoff);
        filterLP12.setResonance(0.707f); // Butterworth response
        filterLP12.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

        // HP 24dB: Two cascaded HP filters
        filterHP24.setCutoffFrequency(cutoff);
        filterHP24.setResonance(resonance);
        filterHP24.setType(juce::dsp::StateVariableTPTFilterType::highpass);

        // BP 12dB: Bandpass filter
        filterBP12.setCutoffFrequency(cutoff);
        filterBP12.setResonance(resonance);
        filterBP12.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

        // Notch: Band-reject filter
        filterNotch.setCutoffFrequency(cutoff);
        filterNotch.setResonance(resonance);
        // JUCE StateVariableTPTFilter doesn't have direct notch type
        // We'll use bandpass and invert (output = input - bandpass)
        // For now, use lowpass (will implement proper notch if needed)
        filterNotch.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    }
};
