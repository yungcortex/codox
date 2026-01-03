#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include <vector>
#include "Voice.h"
#include "EffectsChain.h"
#include "LFO.h"
#include "ModulationMatrix.h"

class CodoxAudioProcessor : public juce::AudioProcessor
{
public:
    CodoxAudioProcessor();
    ~CodoxAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Codox"; }
    bool acceptsMidi() const override { return true; }  // Synth requires MIDI
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

    // Modulation matrix - public for WebView access
    ModulationMatrix modMatrix;

    // Get modulated parameter value (applies all active modulations)
    float getModulatedParam(const juce::String& paramId);

private:
    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Voice management (Phase 3.1)
    static constexpr int numVoices = 16;
    std::vector<std::unique_ptr<Voice>> voices;
    int nextVoiceIndex = 0; // Round-robin voice allocation

    // Phase 3.5: Effects Chain
    EffectsChain effectsChain;

    // Phase 3.6: LFOs (4 global LFOs - NOT per-voice)
    // NOTE: v1.0 has NO routing (no mod matrix) - LFOs generate signals but don't modulate anything
    LFO lfo1;
    LFO lfo2;
    LFO lfo3;
    LFO lfo4;

    // Phase 3.6: Glide/Portamento state
    float glideTime = 0.0f; // Current glide time parameter (0-10 seconds)
    int lastMidiNote = -1; // Last triggered MIDI note (for glide transitions)

    // v2.1: MIDI state for modulation sources
    float currentModWheel = 0.0f;      // CC1 (0-1)
    float currentPitchBend = 0.0f;     // -1 to +1
    float currentAftertouch = 0.0f;    // 0-1
    float currentVelocity = 0.0f;      // Last note velocity (0-1)

    // Helper methods
    void allocateVoice(int midiNote, float velocity, double sampleRate, float glideTime = 0.0f);
    void releaseVoice(int midiNote);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodoxAudioProcessor)
};
