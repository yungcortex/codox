#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
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
    juce::WebSliderRelay masterVolumeRelay;

    // OSC A (9 parameters)
    juce::WebComboBoxRelay oscAWavetableRelay;
    juce::WebSliderRelay oscAPositionRelay;
    juce::WebSliderRelay oscALevelRelay;
    juce::WebSliderRelay oscAPanRelay;
    juce::WebComboBoxRelay oscAOctaveRelay;
    juce::WebSliderRelay oscASemitoneRelay;
    juce::WebSliderRelay oscAFineRelay;
    juce::WebComboBoxRelay oscAWarpModeRelay;
    juce::WebSliderRelay oscAWarpAmountRelay;

    // OSC B (9 parameters)
    juce::WebComboBoxRelay oscBWavetableRelay;
    juce::WebSliderRelay oscBPositionRelay;
    juce::WebSliderRelay oscBLevelRelay;
    juce::WebSliderRelay oscBPanRelay;
    juce::WebComboBoxRelay oscBOctaveRelay;
    juce::WebSliderRelay oscBSemitoneRelay;
    juce::WebSliderRelay oscBFineRelay;
    juce::WebComboBoxRelay oscBWarpModeRelay;
    juce::WebSliderRelay oscBWarpAmountRelay;

    // Sub + Noise (5 parameters)
    juce::WebComboBoxRelay subShapeRelay;
    juce::WebComboBoxRelay subOctaveRelay;
    juce::WebSliderRelay subLevelRelay;
    juce::WebComboBoxRelay noiseTypeRelay;
    juce::WebSliderRelay noiseLevelRelay;

    // Filter (6 parameters)
    juce::WebComboBoxRelay filterTypeRelay;
    juce::WebSliderRelay filterCutoffRelay;
    juce::WebSliderRelay filterResonanceRelay;
    juce::WebSliderRelay filterDriveRelay;
    juce::WebSliderRelay filterEnvDepthRelay;
    juce::WebSliderRelay filterKeytrackRelay;

    // Amp Envelope (4 parameters)
    juce::WebSliderRelay ampAttackRelay;
    juce::WebSliderRelay ampDecayRelay;
    juce::WebSliderRelay ampSustainRelay;
    juce::WebSliderRelay ampReleaseRelay;

    // Filter Envelope (4 parameters)
    juce::WebSliderRelay filtAttackRelay;
    juce::WebSliderRelay filtDecayRelay;
    juce::WebSliderRelay filtSustainRelay;
    juce::WebSliderRelay filtReleaseRelay;

    // LFOs (12 parameters - 4 LFOs x 3 params)
    juce::WebComboBoxRelay lfo1ShapeRelay;
    juce::WebSliderRelay lfo1RateRelay;
    juce::WebToggleButtonRelay lfo1SyncRelay;

    juce::WebComboBoxRelay lfo2ShapeRelay;
    juce::WebSliderRelay lfo2RateRelay;
    juce::WebToggleButtonRelay lfo2SyncRelay;

    juce::WebComboBoxRelay lfo3ShapeRelay;
    juce::WebSliderRelay lfo3RateRelay;
    juce::WebToggleButtonRelay lfo3SyncRelay;

    juce::WebComboBoxRelay lfo4ShapeRelay;
    juce::WebSliderRelay lfo4RateRelay;
    juce::WebToggleButtonRelay lfo4SyncRelay;

    // Macros + Voice (7 parameters)
    juce::WebSliderRelay macro1Relay;
    juce::WebSliderRelay macro2Relay;
    juce::WebSliderRelay macro3Relay;
    juce::WebSliderRelay macro4Relay;
    juce::WebComboBoxRelay unisonVoicesRelay;
    juce::WebSliderRelay unisonDetuneRelay;
    juce::WebSliderRelay glideTimeRelay;

    // Effects (8 parameters)
    juce::WebSliderRelay fxDistortionMixRelay;
    juce::WebSliderRelay fxChorusMixRelay;
    juce::WebSliderRelay fxPhaserMixRelay;
    juce::WebSliderRelay fxFlangerMixRelay;
    juce::WebSliderRelay fxDelayMixRelay;
    juce::WebSliderRelay fxReverbMixRelay;
    juce::WebSliderRelay fxEqMixRelay;
    juce::WebSliderRelay fxCompressorMixRelay;

    // 2️⃣ WEBVIEW SECOND (depends on relays)
    juce::WebBrowserComponent webView;

    // 3️⃣ ATTACHMENTS LAST (depend on relays and webView)
    // Header
    juce::WebSliderParameterAttachment masterVolumeAttachment;

    // OSC A
    juce::WebComboBoxParameterAttachment oscAWavetableAttachment;
    juce::WebSliderParameterAttachment oscAPositionAttachment;
    juce::WebSliderParameterAttachment oscALevelAttachment;
    juce::WebSliderParameterAttachment oscAPanAttachment;
    juce::WebComboBoxParameterAttachment oscAOctaveAttachment;
    juce::WebSliderParameterAttachment oscASemitoneAttachment;
    juce::WebSliderParameterAttachment oscAFineAttachment;
    juce::WebComboBoxParameterAttachment oscAWarpModeAttachment;
    juce::WebSliderParameterAttachment oscAWarpAmountAttachment;

    // OSC B
    juce::WebComboBoxParameterAttachment oscBWavetableAttachment;
    juce::WebSliderParameterAttachment oscBPositionAttachment;
    juce::WebSliderParameterAttachment oscBLevelAttachment;
    juce::WebSliderParameterAttachment oscBPanAttachment;
    juce::WebComboBoxParameterAttachment oscBOctaveAttachment;
    juce::WebSliderParameterAttachment oscBSemitoneAttachment;
    juce::WebSliderParameterAttachment oscBFineAttachment;
    juce::WebComboBoxParameterAttachment oscBWarpModeAttachment;
    juce::WebSliderParameterAttachment oscBWarpAmountAttachment;

    // Sub + Noise
    juce::WebComboBoxParameterAttachment subShapeAttachment;
    juce::WebComboBoxParameterAttachment subOctaveAttachment;
    juce::WebSliderParameterAttachment subLevelAttachment;
    juce::WebComboBoxParameterAttachment noiseTypeAttachment;
    juce::WebSliderParameterAttachment noiseLevelAttachment;

    // Filter
    juce::WebComboBoxParameterAttachment filterTypeAttachment;
    juce::WebSliderParameterAttachment filterCutoffAttachment;
    juce::WebSliderParameterAttachment filterResonanceAttachment;
    juce::WebSliderParameterAttachment filterDriveAttachment;
    juce::WebSliderParameterAttachment filterEnvDepthAttachment;
    juce::WebSliderParameterAttachment filterKeytrackAttachment;

    // Amp Envelope
    juce::WebSliderParameterAttachment ampAttackAttachment;
    juce::WebSliderParameterAttachment ampDecayAttachment;
    juce::WebSliderParameterAttachment ampSustainAttachment;
    juce::WebSliderParameterAttachment ampReleaseAttachment;

    // Filter Envelope
    juce::WebSliderParameterAttachment filtAttackAttachment;
    juce::WebSliderParameterAttachment filtDecayAttachment;
    juce::WebSliderParameterAttachment filtSustainAttachment;
    juce::WebSliderParameterAttachment filtReleaseAttachment;

    // LFOs
    juce::WebComboBoxParameterAttachment lfo1ShapeAttachment;
    juce::WebSliderParameterAttachment lfo1RateAttachment;
    juce::WebToggleButtonParameterAttachment lfo1SyncAttachment;

    juce::WebComboBoxParameterAttachment lfo2ShapeAttachment;
    juce::WebSliderParameterAttachment lfo2RateAttachment;
    juce::WebToggleButtonParameterAttachment lfo2SyncAttachment;

    juce::WebComboBoxParameterAttachment lfo3ShapeAttachment;
    juce::WebSliderParameterAttachment lfo3RateAttachment;
    juce::WebToggleButtonParameterAttachment lfo3SyncAttachment;

    juce::WebComboBoxParameterAttachment lfo4ShapeAttachment;
    juce::WebSliderParameterAttachment lfo4RateAttachment;
    juce::WebToggleButtonParameterAttachment lfo4SyncAttachment;

    // Macros + Voice
    juce::WebSliderParameterAttachment macro1Attachment;
    juce::WebSliderParameterAttachment macro2Attachment;
    juce::WebSliderParameterAttachment macro3Attachment;
    juce::WebSliderParameterAttachment macro4Attachment;
    juce::WebComboBoxParameterAttachment unisonVoicesAttachment;
    juce::WebSliderParameterAttachment unisonDetuneAttachment;
    juce::WebSliderParameterAttachment glideTimeAttachment;

    // Effects
    juce::WebSliderParameterAttachment fxDistortionMixAttachment;
    juce::WebSliderParameterAttachment fxChorusMixAttachment;
    juce::WebSliderParameterAttachment fxPhaserMixAttachment;
    juce::WebSliderParameterAttachment fxFlangerMixAttachment;
    juce::WebSliderParameterAttachment fxDelayMixAttachment;
    juce::WebSliderParameterAttachment fxReverbMixAttachment;
    juce::WebSliderParameterAttachment fxEqMixAttachment;
    juce::WebSliderParameterAttachment fxCompressorMixAttachment;

    // Helper for resource serving
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CodoxAudioProcessorEditor)
};
