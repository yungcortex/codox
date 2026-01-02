#include "PluginEditor.h"

CodoxAudioProcessorEditor::CodoxAudioProcessorEditor(CodoxAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1200, 800);  // Size from mockup (will be adjusted in Stage 3 based on UI)
}

CodoxAudioProcessorEditor::~CodoxAudioProcessorEditor()
{
}

void CodoxAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("Codox - Stage 1", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(14.0f);
    g.drawFittedText("50 parameters implemented (47 audio + 3 UI-only)",
                     getLocalBounds().reduced(20).removeFromBottom(30),
                     juce::Justification::centred, 1);
}

void CodoxAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 3 (GUI)
}
