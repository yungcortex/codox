#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include <vector>
#include "Voice.h"

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

private:
    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Voice management (Phase 3.1)
    static constexpr int numVoices = 16;
    std::vector<std::unique_ptr<Voice>> voices;
    int nextVoiceIndex = 0; // Round-robin voice allocation

    // Helper methods
    void allocateVoice(int midiNote, float velocity, double sampleRate);
    void releaseVoice(int midiNote);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodoxAudioProcessor)
};
