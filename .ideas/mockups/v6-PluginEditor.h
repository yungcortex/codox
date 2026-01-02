// This is an auto-generated TEMPLATE file for Stage 3 (GUI) integration
// DO NOT use this file directly - gui-agent will adapt it to your plugin structure

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
    WebView-based plugin editor for Codox
    
    ⚠️ CRITICAL: Member declaration order affects destruction order
    Members destroyed in REVERSE order → attachments must be destroyed BEFORE webView
*/
class CodoxAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CodoxAudioProcessorEditor (CodoxAudioProcessor&);
    ~CodoxAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Reference to processor
    CodoxAudioProcessor& audioProcessor;

    // ========================================================================
    // ⚠️ CRITICAL MEMBER DECLARATION ORDER ⚠️
    // Order: Relays → WebView → Attachments
    // Members destroyed in REVERSE order
    // ========================================================================

    // 1️⃣ RELAYS FIRST (no dependencies)
    // Header
    std::unique_ptr<juce::WebSliderRelay> masterVolumeRelay;
    
    // OSC A (9 parameters)
    std::unique_ptr<juce::WebComboBoxRelay> oscAWavetableRelay;
    std::unique_ptr<juce::WebSliderRelay> oscAPositionRelay;
    std::unique_ptr<juce::WebSliderRelay> oscALevelRelay;
    std::unique_ptr<juce::WebSliderRelay> oscAPanRelay;
    std::unique_ptr<juce::WebComboBoxRelay> oscAOctaveRelay;
    std::unique_ptr<juce::WebSliderRelay> oscASemitoneRelay;
    std::unique_ptr<juce::WebSliderRelay> oscAFineRelay;
    std::unique_ptr<juce::WebComboBoxRelay> oscAWarpModeRelay;
    std::unique_ptr<juce::WebSliderRelay> oscAWarpAmountRelay;
    
    // OSC B (9 parameters)
    std::unique_ptr<juce::WebComboBoxRelay> oscBWavetableRelay;
    std::unique_ptr<juce::WebSliderRelay> oscBPositionRelay;
    std::unique_ptr<juce::WebSliderRelay> oscBLevelRelay;
    std::unique_ptr<juce::WebSliderRelay> oscBPanRelay;
    std::unique_ptr<juce::WebComboBoxRelay> oscBOctaveRelay;
    std::unique_ptr<juce::WebSliderRelay> oscBSemitoneRelay;
    std::unique_ptr<juce::WebSliderRelay> oscBFineRelay;
    std::unique_ptr<juce::WebComboBoxRelay> oscBWarpModeRelay;
    std::unique_ptr<juce::WebSliderRelay> oscBWarpAmountRelay;
    
    // Sub + Noise (5 parameters)
    std::unique_ptr<juce::WebComboBoxRelay> subShapeRelay;
    std::unique_ptr<juce::WebComboBoxRelay> subOctaveRelay;
    std::unique_ptr<juce::WebSliderRelay> subLevelRelay;
    std::unique_ptr<juce::WebComboBoxRelay> noiseTypeRelay;
    std::unique_ptr<juce::WebSliderRelay> noiseLevelRelay;
    
    // Filter (6 parameters)
    std::unique_ptr<juce::WebComboBoxRelay> filterTypeRelay;
    std::unique_ptr<juce::WebSliderRelay> filterCutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> filterResonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> filterDriveRelay;
    std::unique_ptr<juce::WebSliderRelay> filterEnvDepthRelay;
    std::unique_ptr<juce::WebSliderRelay> filterKeytrackRelay;
    
    // Amp Envelope (4 parameters)
    std::unique_ptr<juce::WebSliderRelay> ampAttackRelay;
    std::unique_ptr<juce::WebSliderRelay> ampDecayRelay;
    std::unique_ptr<juce::WebSliderRelay> ampSustainRelay;
    std::unique_ptr<juce::WebSliderRelay> ampReleaseRelay;
    
    // Filter Envelope (4 parameters)
    std::unique_ptr<juce::WebSliderRelay> filtAttackRelay;
    std::unique_ptr<juce::WebSliderRelay> filtDecayRelay;
    std::unique_ptr<juce::WebSliderRelay> filtSustainRelay;
    std::unique_ptr<juce::WebSliderRelay> filtReleaseRelay;
    
    // LFOs (12 parameters - 4 LFOs x 3 params)
    std::unique_ptr<juce::WebComboBoxRelay> lfo1ShapeRelay;
    std::unique_ptr<juce::WebSliderRelay> lfo1RateRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> lfo1SyncRelay;
    
    std::unique_ptr<juce::WebComboBoxRelay> lfo2ShapeRelay;
    std::unique_ptr<juce::WebSliderRelay> lfo2RateRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> lfo2SyncRelay;
    
    std::unique_ptr<juce::WebComboBoxRelay> lfo3ShapeRelay;
    std::unique_ptr<juce::WebSliderRelay> lfo3RateRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> lfo3SyncRelay;
    
    std::unique_ptr<juce::WebComboBoxRelay> lfo4ShapeRelay;
    std::unique_ptr<juce::WebSliderRelay> lfo4RateRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> lfo4SyncRelay;
    
    // Macros + Voice (7 parameters)
    std::unique_ptr<juce::WebSliderRelay> macro1Relay;
    std::unique_ptr<juce::WebSliderRelay> macro2Relay;
    std::unique_ptr<juce::WebSliderRelay> macro3Relay;
    std::unique_ptr<juce::WebSliderRelay> macro4Relay;
    std::unique_ptr<juce::WebComboBoxRelay> unisonVoicesRelay;
    std::unique_ptr<juce::WebSliderRelay> unisonDetuneRelay;
    std::unique_ptr<juce::WebSliderRelay> glideTimeRelay;
    
    // Effects (8 parameters)
    std::unique_ptr<juce::WebSliderRelay> fxDistortionMixRelay;
    std::unique_ptr<juce::WebSliderRelay> fxChorusMixRelay;
    std::unique_ptr<juce::WebSliderRelay> fxPhaserMixRelay;
    std::unique_ptr<juce::WebSliderRelay> fxFlangerMixRelay;
    std::unique_ptr<juce::WebSliderRelay> fxDelayMixRelay;
    std::unique_ptr<juce::WebSliderRelay> fxReverbMixRelay;
    std::unique_ptr<juce::WebSliderRelay> fxEqMixRelay;
    std::unique_ptr<juce::WebSliderRelay> fxCompressorMixRelay;

    // 2️⃣ WEBVIEW SECOND (depends on relays)
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // 3️⃣ ATTACHMENTS LAST (depend on relays and webView)
    // Header
    std::unique_ptr<juce::WebSliderParameterAttachment> masterVolumeAttachment;
    
    // OSC A
    std::unique_ptr<juce::WebComboBoxParameterAttachment> oscAWavetableAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscAPositionAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscALevelAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscAPanAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> oscAOctaveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscASemitoneAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscAFineAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> oscAWarpModeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscAWarpAmountAttachment;
    
    // OSC B
    std::unique_ptr<juce::WebComboBoxParameterAttachment> oscBWavetableAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscBPositionAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscBLevelAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscBPanAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> oscBOctaveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscBSemitoneAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscBFineAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> oscBWarpModeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> oscBWarpAmountAttachment;
    
    // Sub + Noise
    std::unique_ptr<juce::WebComboBoxParameterAttachment> subShapeAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> subOctaveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> subLevelAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> noiseTypeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> noiseLevelAttachment;
    
    // Filter
    std::unique_ptr<juce::WebComboBoxParameterAttachment> filterTypeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filterResonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filterDriveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filterEnvDepthAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filterKeytrackAttachment;
    
    // Amp Envelope
    std::unique_ptr<juce::WebSliderParameterAttachment> ampAttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> ampDecayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> ampSustainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> ampReleaseAttachment;
    
    // Filter Envelope
    std::unique_ptr<juce::WebSliderParameterAttachment> filtAttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filtDecayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filtSustainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> filtReleaseAttachment;
    
    // LFOs
    std::unique_ptr<juce::WebComboBoxParameterAttachment> lfo1ShapeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> lfo1RateAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> lfo1SyncAttachment;
    
    std::unique_ptr<juce::WebComboBoxParameterAttachment> lfo2ShapeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> lfo2RateAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> lfo2SyncAttachment;
    
    std::unique_ptr<juce::WebComboBoxParameterAttachment> lfo3ShapeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> lfo3RateAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> lfo3SyncAttachment;
    
    std::unique_ptr<juce::WebComboBoxParameterAttachment> lfo4ShapeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> lfo4RateAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> lfo4SyncAttachment;
    
    // Macros + Voice
    std::unique_ptr<juce::WebSliderParameterAttachment> macro1Attachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> macro2Attachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> macro3Attachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> macro4Attachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> unisonVoicesAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> unisonDetuneAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> glideTimeAttachment;
    
    // Effects
    std::unique_ptr<juce::WebSliderParameterAttachment> fxDistortionMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> fxChorusMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> fxPhaserMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> fxFlangerMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> fxDelayMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> fxReverbMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> fxEqMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> fxCompressorMixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CodoxAudioProcessorEditor)
};
