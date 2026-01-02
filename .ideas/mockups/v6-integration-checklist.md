# Stage 3 (GUI) Integration Checklist - Codox v6

**Plugin:** Codox
**Mockup Version:** v6 (finalized)
**Generated:** 2026-01-01
**Total Parameters:** 50 (47 audio + 3 UI-only)
**Window Dimensions:** 1200x800 pixels

## Prerequisites

Before starting integration, verify:

- [ ] Stage 1 (Shell) complete - All 50 parameters in APVTS
- [ ] Stage 2 (DSP) complete - Audio processing working
- [ ] v6-ui.yaml has `finalized: true` marker
- [ ] parameter-spec.md exists and matches mockup parameters

## 1. Copy UI Files

**Create directory structure:**
```bash
mkdir -p plugins/Codox/Source/ui/public/js/juce
```

**Copy files:**
- [ ] Copy `v6-ui.html` to `Source/ui/public/index.html`
- [ ] Copy JUCE frontend library to `Source/ui/public/js/juce/index.js`
  - Source: `.claude/skills/ui-mockup/assets/juce-frontend/index.js`
- [ ] Verify no additional CSS, images, or fonts required (mockup uses inline styles)

**Verification:**
```bash
# Check files exist
ls -la plugins/Codox/Source/ui/public/
ls -la plugins/Codox/Source/ui/public/js/juce/
```

## 2. Update PluginEditor.h

**Replace file content:**
- [ ] Open `plugins/Codox/Source/PluginEditor.h`
- [ ] Replace entire file with content from `v6-PluginEditor.h`
- [ ] Update class name from template: `CodoxAudioProcessorEditor` (should already be correct)
- [ ] Verify include: `#include "PluginProcessor.h"` matches your project

**Verify member order (CRITICAL):**
```cpp
private:
    // 1️⃣ RELAYS FIRST (50 relay declarations)
    std::unique_ptr<juce::WebSliderRelay> masterVolumeRelay;
    // ... (all 50 relays)

    // 2️⃣ WEBVIEW SECOND
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // 3️⃣ ATTACHMENTS LAST (50 attachment declarations)
    std::unique_ptr<juce::WebSliderParameterAttachment> masterVolumeAttachment;
    // ... (all 50 attachments)
```

**Why order matters:** Attachments call `evaluateJavascript()` during destruction, so they MUST be destroyed BEFORE webView. C++ destroys members in reverse order of declaration.

## 3. Update PluginEditor.cpp

**Replace file content:**
- [ ] Open `plugins/Codox/Source/PluginEditor.cpp`
- [ ] Replace entire file with content from `v6-PluginEditor.cpp`
- [ ] Update class name: `CodoxAudioProcessorEditor` (should already be correct)
- [ ] Verify constructor initializer list order matches header declaration order

**Verify initialization sections:**
```cpp
CodoxAudioProcessorEditor::CodoxAudioProcessorEditor (CodoxAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
    // 1️⃣ Relay initialization (50 relays)
    , masterVolumeRelay(std::make_unique<juce::WebSliderRelay>("master_volume"))
    // ... (all 50 relays)

    // 2️⃣ WebView initialization (with .withOptionsFrom() for each relay)
    , webView(std::make_unique<juce::WebBrowserComponent>(...))

    // 3️⃣ Attachment initialization (50 attachments)
    , masterVolumeAttachment(std::make_unique<juce::WebSliderParameterAttachment>(...))
    // ... (all 50 attachments)
{
    setSize(1200, 800);  // From v6-ui.yaml
    addAndMakeVisible(*webView);
    webView->goToURL("https://juce.backend/index.html");
}
```

## 4. Update CMakeLists.txt

**Append WebView configuration:**
- [ ] Open `plugins/Codox/CMakeLists.txt`
- [ ] Scroll to end of file (after existing target_link_libraries)
- [ ] Append content from `v6-CMakeLists-SNIPPET.txt`

**Verify additions:**
```cmake
# Should see these additions:
juce_add_binary_data(${PRODUCT_NAME}_UIResources
    SOURCES
        Source/ui/public/index.html
        Source/ui/public/js/juce/index.js
)

target_link_libraries(${PRODUCT_NAME}
    PRIVATE
        ${PRODUCT_NAME}_UIResources
        juce::juce_gui_extra  # NEW: WebView module
)

target_compile_definitions(${PRODUCT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=1  # NEW: Enable WebView
        JUCE_USE_CURL=0
)
```

**Common CMake mistakes:**
- ❌ Forgetting `juce::juce_gui_extra` module → WebBrowserComponent not found
- ❌ Missing `JUCE_WEB_BROWSER=1` → WebView features disabled
- ❌ Wrong MIME type in resource provider → JavaScript won't execute

## 5. Build and Test (Debug)

**Build debug configuration:**
```bash
cd /Users/yungcortex/plugin-freedom-system
./scripts/build-and-install.sh Codox --debug
```

**Verification checklist:**
- [ ] Build succeeds without errors
- [ ] Build succeeds without warnings (check for member order warnings)
- [ ] Standalone loads without crashes
- [ ] WebView displays UI (not blank screen)
- [ ] Right-click → Inspect works (WebView developer tools)
- [ ] Console shows: "Codox UI initialized - 50 parameters ready for binding"
- [ ] Console shows: `window.__JUCE__` object exists
- [ ] Console shows no JavaScript errors

**Common debug build issues:**
- Blank WebView → Check resource provider paths
- JavaScript errors → Check MIME type for index.js ("application/javascript")
- Missing parameters → Verify all 50 parameters in APVTS

## 6. Build and Test (Release)

**Build release configuration:**
```bash
./scripts/build-and-install.sh Codox --release
```

**Verification checklist:**
- [ ] Release build succeeds
- [ ] No crashes on plugin load (test in DAW)
- [ ] No crashes on plugin reload (test 10 times - reload plugin in DAW repeatedly)
- [ ] UI displays correctly in release mode
- [ ] All 50 parameters visible and responsive

**Common release build issues:**
- Crash on reload → Member order violation (attachments before webView in header)
- UI slower than debug → Expected (no developer tools overhead)

## 7. Test Parameter Binding

**For each parameter category, verify bidirectional sync:**

### Master (1 parameter)
- [ ] master_volume: UI slider ↔ APVTS ↔ automation

### OSC A (9 parameters)
- [ ] osc_a_wavetable: Dropdown ↔ APVTS
- [ ] osc_a_position: Slider ↔ APVTS
- [ ] osc_a_level: Slider ↔ APVTS
- [ ] osc_a_pan: Slider ↔ APVTS
- [ ] osc_a_octave: Dropdown ↔ APVTS
- [ ] osc_a_semitone: Slider ↔ APVTS
- [ ] osc_a_fine: Slider ↔ APVTS
- [ ] osc_a_warp_mode: Dropdown ↔ APVTS
- [ ] osc_a_warp_amount: Slider ↔ APVTS

### OSC B (9 parameters)
- [ ] osc_b_wavetable through osc_b_warp_amount (same as OSC A)

### Sub + Noise (5 parameters)
- [ ] sub_shape: Dropdown ↔ APVTS
- [ ] sub_octave: Dropdown ↔ APVTS
- [ ] sub_level: Slider ↔ APVTS
- [ ] noise_type: Dropdown ↔ APVTS
- [ ] noise_level: Slider ↔ APVTS

### Filter (6 parameters)
- [ ] filter_type: Dropdown ↔ APVTS
- [ ] filter_cutoff: Slider ↔ APVTS
- [ ] filter_resonance: Slider ↔ APVTS
- [ ] filter_drive: Slider ↔ APVTS
- [ ] filter_env_depth: Slider ↔ APVTS
- [ ] filter_keytrack: Slider ↔ APVTS

### Amp Envelope (4 parameters)
- [ ] amp_attack, amp_decay, amp_sustain, amp_release: All sliders ↔ APVTS

### Filter Envelope (4 parameters)
- [ ] filt_attack, filt_decay, filt_sustain, filt_release: All sliders ↔ APVTS

### LFOs (12 parameters - 4 LFOs x 3 params)
- [ ] lfo1_shape: Dropdown ↔ APVTS
- [ ] lfo1_rate: Slider ↔ APVTS
- [ ] lfo1_sync: Toggle ↔ APVTS
- [ ] lfo2_shape, lfo2_rate, lfo2_sync (repeat for LFO 2)
- [ ] lfo3_shape, lfo3_rate, lfo3_sync (repeat for LFO 3)
- [ ] lfo4_shape, lfo4_rate, lfo4_sync (repeat for LFO 4)

### Macros + Voice (7 parameters)
- [ ] macro1, macro2, macro3, macro4: All sliders ↔ APVTS
- [ ] unison_voices: Dropdown ↔ APVTS
- [ ] unison_detune: Slider ↔ APVTS
- [ ] glide_time: Slider ↔ APVTS

### Effects (8 parameters)
- [ ] fx_distortion_mix through fx_compressor_mix: All sliders ↔ APVTS

**Testing methodology:**
1. Move UI control → Verify parameter changes in DAW
2. Automate parameter in DAW → Verify UI control moves
3. Save preset → Reload preset → Verify UI updates
4. Save project → Close DAW → Reopen → Verify values persisted

## 8. Test Canvas Visualizations

**Verify real-time canvas updates:**
- [ ] osc_a_waveform_display: Changes when osc_a_wavetable or osc_a_position change
- [ ] osc_b_waveform_display: Changes when osc_b_wavetable or osc_b_position change
- [ ] filter_curve_display: Changes when filter_cutoff, filter_resonance, or filter_type change
- [ ] amp_adsr_display: Updates when amp_attack, amp_decay, amp_sustain, or amp_release change
- [ ] filt_adsr_display: Updates when filt_attack, filt_decay, filt_sustain, or filt_release change
- [ ] lfo1_waveform_preview: Changes when lfo1_shape changes
- [ ] lfo2_waveform_preview: Changes when lfo2_shape changes
- [ ] lfo3_waveform_preview: Changes when lfo3_shape changes
- [ ] lfo4_waveform_preview: Changes when lfo4_shape changes

**Canvas animations:**
- [ ] Wavetable displays animate smoothly
- [ ] ADSR curves redraw on parameter change
- [ ] LFO previews update on shape selection

## 9. WebView-Specific Validation

**CSS constraints:**
- [ ] No viewport units used (`100vh`, `100vw`, `100dvh`, `100svh`)
- [ ] Root elements use `height: 100%` instead
- [ ] `user-select: none` present (native application feel)
- [ ] No text cursor visible when hovering over controls

**Resource provider:**
- [ ] All files served with correct MIME types
  - `index.html` → `text/html`
  - `index.js` → `application/javascript` (NOT `text/javascript`)
- [ ] No 404 errors in console
- [ ] No CORS errors

**Context menu:**
- [ ] Right-click disabled (except on developer tools in debug)
- [ ] Native context menu does not appear on controls

## 10. Performance Testing

**CPU usage:**
- [ ] Idle plugin: < 1% CPU
- [ ] Playing notes: Reasonable CPU (depends on DSP complexity)
- [ ] UI updates: No dropped frames when moving sliders
- [ ] Canvas animations: Smooth 60fps

**Memory usage:**
- [ ] Memory stable after 5 minutes of use
- [ ] No memory leaks (check Activity Monitor / Task Manager)
- [ ] Multiple instances don't cause excessive memory growth

## 11. Cross-Platform Testing (if applicable)

**macOS:**
- [ ] WKWebView loads correctly
- [ ] All 50 parameters functional
- [ ] No crashes on plugin reload

**Windows:**
- [ ] WebView2 loads correctly (requires Edge WebView2 Runtime)
- [ ] All 50 parameters functional
- [ ] No crashes on plugin reload

**Linux:**
- [ ] WebKitGTK loads correctly (requires libwebkit2gtk-4.0-dev)
- [ ] All 50 parameters functional
- [ ] No crashes on plugin reload

## 12. DAW-Specific Testing

Test in at least 2 DAWs:

**Logic Pro:**
- [ ] Plugin loads as AU/VST3
- [ ] All parameters automatable
- [ ] Preset recall works
- [ ] Project save/load works

**Ableton Live:**
- [ ] Plugin loads as AU/VST3
- [ ] All parameters automatable
- [ ] Preset recall works
- [ ] Project save/load works

**Reaper:**
- [ ] Plugin loads as VST3
- [ ] All parameters automatable
- [ ] Preset recall works
- [ ] Project save/load works

## 13. Validation Gate (Automatic)

**After completing all steps, validation-agent will run:**
- [ ] Compile-time validation (build succeeds)
- [ ] Runtime validation (pluginval tests)
- [ ] Parameter validation (all 50 parameters exposed)
- [ ] UI validation (WebView loads, no blank screen)

**Expected validation result:** ✅ All tests pass

## Common Issues and Solutions

### Issue: Blank WebView
**Symptoms:** WebView loads but shows nothing
**Cause:** Resource provider not serving files correctly
**Solution:**
1. Verify binary data symbols exist (check build output)
2. Check MIME types in resource provider
3. Verify HTML file path in `goToURL()` call

### Issue: JavaScript errors
**Symptoms:** Console shows "Failed to load module"
**Cause:** Wrong MIME type for index.js
**Solution:** Change to `"application/javascript"` (NOT `"text/javascript"`)

### Issue: Crash on plugin reload (release only)
**Symptoms:** Debug works fine, release crashes when reloading plugin in DAW
**Cause:** Member order violation (attachments before webView)
**Solution:** Verify header file member order: relays → webView → attachments

### Issue: Parameters don't sync
**Symptoms:** Moving UI control doesn't change DAW parameter
**Cause:** Parameter ID mismatch between UI and APVTS
**Solution:** Verify parameter IDs match exactly (case-sensitive)

### Issue: Viewport units cause blank screen
**Symptoms:** WebView blank on first load, appears after resize
**Cause:** Using `100vh` or `100vw` (viewport units)
**Solution:** Replace with `height: 100%` on html/body elements

## Success Criteria

Stage 3 integration is complete when:

✅ All 50 parameters sync bidirectionally (UI ↔ APVTS ↔ DAW)
✅ All 9 canvas visualizations update in real-time
✅ Debug and release builds succeed without warnings
✅ No crashes after 10 plugin reloads in DAW
✅ Automation works for all parameters
✅ Preset recall updates UI correctly
✅ WebView displays UI (not blank)
✅ No JavaScript errors in console
✅ validation-agent tests pass (automatic)

## Next Steps After Integration

Once Stage 3 completes and validation passes:

1. **Install to system:** `/install-plugin Codox`
2. **Test in real project:** Create music with the plugin
3. **Iterate if needed:** `/improve Codox` for bug fixes or features
4. **Package for distribution:** `/package Codox` (creates installer)

## Parameter Reference

**Total:** 50 parameters across 9 categories

| Category | Count | Types |
|----------|-------|-------|
| Header | 1 | Slider (1) |
| OSC A | 9 | Slider (6), Combo (3) |
| OSC B | 9 | Slider (6), Combo (3) |
| Sub + Noise | 5 | Slider (2), Combo (3) |
| Filter | 6 | Slider (5), Combo (1) |
| Amp Envelope | 4 | Slider (4) |
| Filter Envelope | 4 | Slider (4) |
| LFOs | 12 | Slider (4), Combo (4), Toggle (4) |
| Macros + Voice | 7 | Slider (5), Combo (1) |
| Effects | 8 | Slider (8) |
| **TOTAL** | **50** | **Slider (38), Combo (8), Toggle (4)** |

## File Manifest

Files generated by ui-finalization-agent:

1. `parameter-spec.md` - Complete parameter specification (50 params)
2. `v6-ui.html` - Production HTML (1200x800px, JUCE bindings ready)
3. `v6-PluginEditor.h` - C++ header template (50 relays + 50 attachments)
4. `v6-PluginEditor.cpp` - C++ implementation template (initialization code)
5. `v6-CMakeLists-SNIPPET.txt` - WebView CMake configuration
6. `v6-integration-checklist.md` - This file (integration guide)

All files created: 2026-01-01
