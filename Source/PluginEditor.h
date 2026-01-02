#pragma once
#include "PluginProcessor.h"

class CodoxAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit CodoxAudioProcessorEditor(CodoxAudioProcessor&);
    ~CodoxAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    CodoxAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodoxAudioProcessorEditor)
};
