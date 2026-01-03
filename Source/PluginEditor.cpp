#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
CodoxAudioProcessorEditor::CodoxAudioProcessorEditor (CodoxAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
    // ========================================================================
    // 1️⃣ RELAY INITIALIZATION (order matches header declaration)
    // ========================================================================
    , masterVolumeRelay("master_volume")
    // OSC A
    , oscAWavetableRelay("osc_a_wavetable")
    , oscAPositionRelay("osc_a_position")
    , oscALevelRelay("osc_a_level")
    , oscAPanRelay("osc_a_pan")
    , oscAOctaveRelay("osc_a_octave")
    , oscASemitoneRelay("osc_a_semitone")
    , oscAFineRelay("osc_a_fine")
    , oscAWarpModeRelay("osc_a_warp_mode")
    , oscAWarpAmountRelay("osc_a_warp_amount")
    // OSC B
    , oscBWavetableRelay("osc_b_wavetable")
    , oscBPositionRelay("osc_b_position")
    , oscBLevelRelay("osc_b_level")
    , oscBPanRelay("osc_b_pan")
    , oscBOctaveRelay("osc_b_octave")
    , oscBSemitoneRelay("osc_b_semitone")
    , oscBFineRelay("osc_b_fine")
    , oscBWarpModeRelay("osc_b_warp_mode")
    , oscBWarpAmountRelay("osc_b_warp_amount")
    // Sub + Noise
    , subShapeRelay("sub_shape")
    , subOctaveRelay("sub_octave")
    , subLevelRelay("sub_level")
    , noiseTypeRelay("noise_type")
    , noiseLevelRelay("noise_level")
    // Filter
    , filterTypeRelay("filter_type")
    , filterCutoffRelay("filter_cutoff")
    , filterResonanceRelay("filter_resonance")
    , filterDriveRelay("filter_drive")
    , filterEnvDepthRelay("filter_env_depth")
    , filterKeytrackRelay("filter_keytrack")
    // Amp Envelope
    , ampAttackRelay("amp_attack")
    , ampDecayRelay("amp_decay")
    , ampSustainRelay("amp_sustain")
    , ampReleaseRelay("amp_release")
    // Filter Envelope
    , filtAttackRelay("filt_attack")
    , filtDecayRelay("filt_decay")
    , filtSustainRelay("filt_sustain")
    , filtReleaseRelay("filt_release")
    // LFOs
    , lfo1ShapeRelay("lfo1_shape")
    , lfo1RateRelay("lfo1_rate")
    , lfo1SyncRelay("lfo1_sync")
    , lfo2ShapeRelay("lfo2_shape")
    , lfo2RateRelay("lfo2_rate")
    , lfo2SyncRelay("lfo2_sync")
    , lfo3ShapeRelay("lfo3_shape")
    , lfo3RateRelay("lfo3_rate")
    , lfo3SyncRelay("lfo3_sync")
    , lfo4ShapeRelay("lfo4_shape")
    , lfo4RateRelay("lfo4_rate")
    , lfo4SyncRelay("lfo4_sync")
    // Macros + Voice
    , macro1Relay("macro1")
    , macro2Relay("macro2")
    , macro3Relay("macro3")
    , macro4Relay("macro4")
    , unisonVoicesRelay("unison_voices")
    , unisonDetuneRelay("unison_detune")
    , glideTimeRelay("glide_time")
    // Effects
    , fxDistortionMixRelay("fx_distortion_mix")
    , fxChorusMixRelay("fx_chorus_mix")
    , fxPhaserMixRelay("fx_phaser_mix")
    , fxFlangerMixRelay("fx_flanger_mix")
    , fxDelayMixRelay("fx_delay_mix")
    , fxDelayTimeRelay("fx_delay_time")
    , fxDelayFeedbackRelay("fx_delay_feedback")
    , fxReverbMixRelay("fx_reverb_mix")
    , fxReverbSizeRelay("fx_reverb_size")
    , fxReverbDecayRelay("fx_reverb_decay")
    , fxEqMixRelay("fx_eq_mix")
    , fxCompressorMixRelay("fx_compressor_mix")
    // ========================================================================
    // 2️⃣ WEBVIEW INITIALIZATION (register all relays via .withOptionsFrom())
    // ========================================================================
    , webView(juce::WebBrowserComponent::Options{}
        .withNativeIntegrationEnabled()
        .withResourceProvider([this](const auto& url) { return getResource(url); })
        // Native preset file handling functions
        .withNativeFunction("savePresetToFile", [this](const juce::Array<juce::var>& args, juce::WebBrowserComponent::NativeFunctionCompletion completion) {
            if (args.size() >= 2) {
                savePresetToFile(args[0].toString(), args[1].toString());
            }
            completion({});
        })
        .withNativeFunction("loadPresetFromFile", [this](const juce::Array<juce::var>& args, juce::WebBrowserComponent::NativeFunctionCompletion completion) {
            (void)args; // unused
            loadPresetFromFile();
            completion({});
        })
        .withOptionsFrom(masterVolumeRelay)
        .withOptionsFrom(oscAWavetableRelay)
        .withOptionsFrom(oscAPositionRelay)
        .withOptionsFrom(oscALevelRelay)
        .withOptionsFrom(oscAPanRelay)
        .withOptionsFrom(oscAOctaveRelay)
        .withOptionsFrom(oscASemitoneRelay)
        .withOptionsFrom(oscAFineRelay)
        .withOptionsFrom(oscAWarpModeRelay)
        .withOptionsFrom(oscAWarpAmountRelay)
        .withOptionsFrom(oscBWavetableRelay)
        .withOptionsFrom(oscBPositionRelay)
        .withOptionsFrom(oscBLevelRelay)
        .withOptionsFrom(oscBPanRelay)
        .withOptionsFrom(oscBOctaveRelay)
        .withOptionsFrom(oscBSemitoneRelay)
        .withOptionsFrom(oscBFineRelay)
        .withOptionsFrom(oscBWarpModeRelay)
        .withOptionsFrom(oscBWarpAmountRelay)
        .withOptionsFrom(subShapeRelay)
        .withOptionsFrom(subOctaveRelay)
        .withOptionsFrom(subLevelRelay)
        .withOptionsFrom(noiseTypeRelay)
        .withOptionsFrom(noiseLevelRelay)
        .withOptionsFrom(filterTypeRelay)
        .withOptionsFrom(filterCutoffRelay)
        .withOptionsFrom(filterResonanceRelay)
        .withOptionsFrom(filterDriveRelay)
        .withOptionsFrom(filterEnvDepthRelay)
        .withOptionsFrom(filterKeytrackRelay)
        .withOptionsFrom(ampAttackRelay)
        .withOptionsFrom(ampDecayRelay)
        .withOptionsFrom(ampSustainRelay)
        .withOptionsFrom(ampReleaseRelay)
        .withOptionsFrom(filtAttackRelay)
        .withOptionsFrom(filtDecayRelay)
        .withOptionsFrom(filtSustainRelay)
        .withOptionsFrom(filtReleaseRelay)
        .withOptionsFrom(lfo1ShapeRelay)
        .withOptionsFrom(lfo1RateRelay)
        .withOptionsFrom(lfo1SyncRelay)
        .withOptionsFrom(lfo2ShapeRelay)
        .withOptionsFrom(lfo2RateRelay)
        .withOptionsFrom(lfo2SyncRelay)
        .withOptionsFrom(lfo3ShapeRelay)
        .withOptionsFrom(lfo3RateRelay)
        .withOptionsFrom(lfo3SyncRelay)
        .withOptionsFrom(lfo4ShapeRelay)
        .withOptionsFrom(lfo4RateRelay)
        .withOptionsFrom(lfo4SyncRelay)
        .withOptionsFrom(macro1Relay)
        .withOptionsFrom(macro2Relay)
        .withOptionsFrom(macro3Relay)
        .withOptionsFrom(macro4Relay)
        .withOptionsFrom(unisonVoicesRelay)
        .withOptionsFrom(unisonDetuneRelay)
        .withOptionsFrom(glideTimeRelay)
        .withOptionsFrom(fxDistortionMixRelay)
        .withOptionsFrom(fxChorusMixRelay)
        .withOptionsFrom(fxPhaserMixRelay)
        .withOptionsFrom(fxFlangerMixRelay)
        .withOptionsFrom(fxDelayMixRelay)
        .withOptionsFrom(fxDelayTimeRelay)
        .withOptionsFrom(fxDelayFeedbackRelay)
        .withOptionsFrom(fxReverbMixRelay)
        .withOptionsFrom(fxReverbSizeRelay)
        .withOptionsFrom(fxReverbDecayRelay)
        .withOptionsFrom(fxEqMixRelay)
        .withOptionsFrom(fxCompressorMixRelay)
    )
    // ========================================================================
    // 3️⃣ ATTACHMENT INITIALIZATION (bind parameters to relays)
    // JUCE 8 CRITICAL: Third parameter (nullptr for undoManager) is REQUIRED
    // ========================================================================
    , masterVolumeAttachment(*audioProcessor.parameters.getParameter("master_volume"), masterVolumeRelay, nullptr)
    // OSC A
    , oscAWavetableAttachment(*audioProcessor.parameters.getParameter("osc_a_wavetable"), oscAWavetableRelay, nullptr)
    , oscAPositionAttachment(*audioProcessor.parameters.getParameter("osc_a_position"), oscAPositionRelay, nullptr)
    , oscALevelAttachment(*audioProcessor.parameters.getParameter("osc_a_level"), oscALevelRelay, nullptr)
    , oscAPanAttachment(*audioProcessor.parameters.getParameter("osc_a_pan"), oscAPanRelay, nullptr)
    , oscAOctaveAttachment(*audioProcessor.parameters.getParameter("osc_a_octave"), oscAOctaveRelay, nullptr)
    , oscASemitoneAttachment(*audioProcessor.parameters.getParameter("osc_a_semitone"), oscASemitoneRelay, nullptr)
    , oscAFineAttachment(*audioProcessor.parameters.getParameter("osc_a_fine"), oscAFineRelay, nullptr)
    , oscAWarpModeAttachment(*audioProcessor.parameters.getParameter("osc_a_warp_mode"), oscAWarpModeRelay, nullptr)
    , oscAWarpAmountAttachment(*audioProcessor.parameters.getParameter("osc_a_warp_amount"), oscAWarpAmountRelay, nullptr)
    // OSC B
    , oscBWavetableAttachment(*audioProcessor.parameters.getParameter("osc_b_wavetable"), oscBWavetableRelay, nullptr)
    , oscBPositionAttachment(*audioProcessor.parameters.getParameter("osc_b_position"), oscBPositionRelay, nullptr)
    , oscBLevelAttachment(*audioProcessor.parameters.getParameter("osc_b_level"), oscBLevelRelay, nullptr)
    , oscBPanAttachment(*audioProcessor.parameters.getParameter("osc_b_pan"), oscBPanRelay, nullptr)
    , oscBOctaveAttachment(*audioProcessor.parameters.getParameter("osc_b_octave"), oscBOctaveRelay, nullptr)
    , oscBSemitoneAttachment(*audioProcessor.parameters.getParameter("osc_b_semitone"), oscBSemitoneRelay, nullptr)
    , oscBFineAttachment(*audioProcessor.parameters.getParameter("osc_b_fine"), oscBFineRelay, nullptr)
    , oscBWarpModeAttachment(*audioProcessor.parameters.getParameter("osc_b_warp_mode"), oscBWarpModeRelay, nullptr)
    , oscBWarpAmountAttachment(*audioProcessor.parameters.getParameter("osc_b_warp_amount"), oscBWarpAmountRelay, nullptr)
    // Sub + Noise
    , subShapeAttachment(*audioProcessor.parameters.getParameter("sub_shape"), subShapeRelay, nullptr)
    , subOctaveAttachment(*audioProcessor.parameters.getParameter("sub_octave"), subOctaveRelay, nullptr)
    , subLevelAttachment(*audioProcessor.parameters.getParameter("sub_level"), subLevelRelay, nullptr)
    , noiseTypeAttachment(*audioProcessor.parameters.getParameter("noise_type"), noiseTypeRelay, nullptr)
    , noiseLevelAttachment(*audioProcessor.parameters.getParameter("noise_level"), noiseLevelRelay, nullptr)
    // Filter
    , filterTypeAttachment(*audioProcessor.parameters.getParameter("filter_type"), filterTypeRelay, nullptr)
    , filterCutoffAttachment(*audioProcessor.parameters.getParameter("filter_cutoff"), filterCutoffRelay, nullptr)
    , filterResonanceAttachment(*audioProcessor.parameters.getParameter("filter_resonance"), filterResonanceRelay, nullptr)
    , filterDriveAttachment(*audioProcessor.parameters.getParameter("filter_drive"), filterDriveRelay, nullptr)
    , filterEnvDepthAttachment(*audioProcessor.parameters.getParameter("filter_env_depth"), filterEnvDepthRelay, nullptr)
    , filterKeytrackAttachment(*audioProcessor.parameters.getParameter("filter_keytrack"), filterKeytrackRelay, nullptr)
    // Amp Envelope
    , ampAttackAttachment(*audioProcessor.parameters.getParameter("amp_attack"), ampAttackRelay, nullptr)
    , ampDecayAttachment(*audioProcessor.parameters.getParameter("amp_decay"), ampDecayRelay, nullptr)
    , ampSustainAttachment(*audioProcessor.parameters.getParameter("amp_sustain"), ampSustainRelay, nullptr)
    , ampReleaseAttachment(*audioProcessor.parameters.getParameter("amp_release"), ampReleaseRelay, nullptr)
    // Filter Envelope
    , filtAttackAttachment(*audioProcessor.parameters.getParameter("filt_attack"), filtAttackRelay, nullptr)
    , filtDecayAttachment(*audioProcessor.parameters.getParameter("filt_decay"), filtDecayRelay, nullptr)
    , filtSustainAttachment(*audioProcessor.parameters.getParameter("filt_sustain"), filtSustainRelay, nullptr)
    , filtReleaseAttachment(*audioProcessor.parameters.getParameter("filt_release"), filtReleaseRelay, nullptr)
    // LFOs
    , lfo1ShapeAttachment(*audioProcessor.parameters.getParameter("lfo1_shape"), lfo1ShapeRelay, nullptr)
    , lfo1RateAttachment(*audioProcessor.parameters.getParameter("lfo1_rate"), lfo1RateRelay, nullptr)
    , lfo1SyncAttachment(*audioProcessor.parameters.getParameter("lfo1_sync"), lfo1SyncRelay, nullptr)
    , lfo2ShapeAttachment(*audioProcessor.parameters.getParameter("lfo2_shape"), lfo2ShapeRelay, nullptr)
    , lfo2RateAttachment(*audioProcessor.parameters.getParameter("lfo2_rate"), lfo2RateRelay, nullptr)
    , lfo2SyncAttachment(*audioProcessor.parameters.getParameter("lfo2_sync"), lfo2SyncRelay, nullptr)
    , lfo3ShapeAttachment(*audioProcessor.parameters.getParameter("lfo3_shape"), lfo3ShapeRelay, nullptr)
    , lfo3RateAttachment(*audioProcessor.parameters.getParameter("lfo3_rate"), lfo3RateRelay, nullptr)
    , lfo3SyncAttachment(*audioProcessor.parameters.getParameter("lfo3_sync"), lfo3SyncRelay, nullptr)
    , lfo4ShapeAttachment(*audioProcessor.parameters.getParameter("lfo4_shape"), lfo4ShapeRelay, nullptr)
    , lfo4RateAttachment(*audioProcessor.parameters.getParameter("lfo4_rate"), lfo4RateRelay, nullptr)
    , lfo4SyncAttachment(*audioProcessor.parameters.getParameter("lfo4_sync"), lfo4SyncRelay, nullptr)
    // Macros + Voice
    , macro1Attachment(*audioProcessor.parameters.getParameter("macro1"), macro1Relay, nullptr)
    , macro2Attachment(*audioProcessor.parameters.getParameter("macro2"), macro2Relay, nullptr)
    , macro3Attachment(*audioProcessor.parameters.getParameter("macro3"), macro3Relay, nullptr)
    , macro4Attachment(*audioProcessor.parameters.getParameter("macro4"), macro4Relay, nullptr)
    , unisonVoicesAttachment(*audioProcessor.parameters.getParameter("unison_voices"), unisonVoicesRelay, nullptr)
    , unisonDetuneAttachment(*audioProcessor.parameters.getParameter("unison_detune"), unisonDetuneRelay, nullptr)
    , glideTimeAttachment(*audioProcessor.parameters.getParameter("glide_time"), glideTimeRelay, nullptr)
    // Effects
    , fxDistortionMixAttachment(*audioProcessor.parameters.getParameter("fx_distortion_mix"), fxDistortionMixRelay, nullptr)
    , fxChorusMixAttachment(*audioProcessor.parameters.getParameter("fx_chorus_mix"), fxChorusMixRelay, nullptr)
    , fxPhaserMixAttachment(*audioProcessor.parameters.getParameter("fx_phaser_mix"), fxPhaserMixRelay, nullptr)
    , fxFlangerMixAttachment(*audioProcessor.parameters.getParameter("fx_flanger_mix"), fxFlangerMixRelay, nullptr)
    , fxDelayMixAttachment(*audioProcessor.parameters.getParameter("fx_delay_mix"), fxDelayMixRelay, nullptr)
    , fxDelayTimeAttachment(*audioProcessor.parameters.getParameter("fx_delay_time"), fxDelayTimeRelay, nullptr)
    , fxDelayFeedbackAttachment(*audioProcessor.parameters.getParameter("fx_delay_feedback"), fxDelayFeedbackRelay, nullptr)
    , fxReverbMixAttachment(*audioProcessor.parameters.getParameter("fx_reverb_mix"), fxReverbMixRelay, nullptr)
    , fxReverbSizeAttachment(*audioProcessor.parameters.getParameter("fx_reverb_size"), fxReverbSizeRelay, nullptr)
    , fxReverbDecayAttachment(*audioProcessor.parameters.getParameter("fx_reverb_decay"), fxReverbDecayRelay, nullptr)
    , fxEqMixAttachment(*audioProcessor.parameters.getParameter("fx_eq_mix"), fxEqMixRelay, nullptr)
    , fxCompressorMixAttachment(*audioProcessor.parameters.getParameter("fx_compressor_mix"), fxCompressorMixRelay, nullptr)
{
    // Set editor size (matches CSS plugin-frame dimensions - Serum-style)
    // CRITICAL: Must match .plugin CSS dimensions exactly or right side gets cut off
    setSize (1100, 720);

    // Make WebView opaque to prevent transparency issues
    webView.setOpaque(true);

    // Add WebView to editor
    addAndMakeVisible(webView);

    // Navigate to local page
    webView.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // Start timer to handle visibility refresh (check every 100ms)
    startTimer(100);
}

CodoxAudioProcessorEditor::~CodoxAudioProcessorEditor()
{
    // Stop timer before destruction
    stopTimer();

    // Attachments destroyed first (reverse order)
    // Then webView destroyed
    // Then relays destroyed last
    // This order prevents crashes during plugin reload
}

//==============================================================================
void CodoxAudioProcessorEditor::visibilityChanged()
{
    if (isVisible())
        needsRefresh = true;
}

void CodoxAudioProcessorEditor::parentHierarchyChanged()
{
    // Called when component is added to parent - trigger refresh
    needsRefresh = true;
}

void CodoxAudioProcessorEditor::timerCallback()
{
    // Decrement cooldown
    if (refreshCooldown > 0)
        refreshCooldown--;

    // Detect visibility transition (was hidden, now visible)
    bool currentlyVisible = isVisible() && isShowing();

    if (!wasVisible && currentlyVisible)
    {
        // Just became visible - need refresh
        needsRefresh = true;
    }
    wasVisible = currentlyVisible;

    // Perform refresh if needed and cooldown expired
    if (needsRefresh && currentlyVisible && refreshCooldown == 0)
    {
        needsRefresh = false;
        refreshCooldown = 5;  // 500ms cooldown (5 x 100ms)

        // AGGRESSIVE FIX: Force reload the entire page
        webView.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
        webView.setBounds(getLocalBounds());
        webView.repaint();
        repaint();
    }
}

//==============================================================================
void CodoxAudioProcessorEditor::paint (juce::Graphics& g)
{
    // WebView fills entire area - no painting needed
    g.fillAll (juce::Colours::black);
}

void CodoxAudioProcessorEditor::resized()
{
    // WebView fills entire editor area
    webView.setBounds(getLocalBounds());
}

//==============================================================================
std::optional<juce::WebBrowserComponent::Resource>
CodoxAudioProcessorEditor::getResource(const juce::String& url)
{
    // Helper lambda to convert raw data to vector<byte>
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Root "/" → index.html
    if (url == "/" || url == "/index.html") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_html, BinaryData::index_htmlSize),
            juce::String("text/html")
        };
    }

    // JUCE frontend library (CRITICAL: Must be "application/javascript")
    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("application/javascript")
        };
    }

    // JUCE native interop check (required by index.js)
    if (url == "/js/juce/check_native_interop.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::check_native_interop_js, BinaryData::check_native_interop_jsSize),
            juce::String("application/javascript")
        };
    }

    // Resource not found
    juce::Logger::writeToLog("Resource not found: " + url);
    return std::nullopt;
}

//==============================================================================
// Preset File Handling
//==============================================================================
void CodoxAudioProcessorEditor::savePresetToFile(const juce::String& jsonData, const juce::String& presetName)
{
    // Get default preset directory (user's Documents/Codox Presets)
    auto presetsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                          .getChildFile("Codox Presets");
    presetsDir.createDirectory();

    // Sanitize filename (remove problematic characters)
    auto safeName = presetName.replaceCharacters("/\\:*?\"<>|", "---------");
    if (safeName.isEmpty())
        safeName = "Untitled";

    // Save directly to preset folder (no dialog - more reliable in DAW plugin context)
    auto file = presetsDir.getChildFile(safeName + ".codox");
    file.replaceWithText(jsonData);

    juce::Logger::writeToLog("Preset saved to: " + file.getFullPathName());
}

void CodoxAudioProcessorEditor::loadPresetFromFile()
{
    // Get default preset directory
    auto presetsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                          .getChildFile("Codox Presets");
    if (!presetsDir.exists())
        presetsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    // Create file chooser for loading
    auto chooser = std::make_shared<juce::FileChooser>(
        "Load Preset",
        presetsDir,
        "*.codox;*.json"
    );

    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{} && file.existsAsFile()) {
                auto jsonData = file.loadFileAsString();
                if (jsonData.isNotEmpty()) {
                    sendPresetToWebView(jsonData);
                }
            }
        });
}

void CodoxAudioProcessorEditor::sendPresetToWebView(const juce::String& jsonData)
{
    // Escape the JSON for JavaScript string
    auto escaped = jsonData.replace("\\", "\\\\")
                           .replace("'", "\\'")
                           .replace("\n", "\\n")
                           .replace("\r", "\\r");

    // Call JavaScript function to apply the preset
    webView.evaluateJavascript("if (typeof applyPresetFromFile === 'function') applyPresetFromFile('" + escaped + "');");
}
