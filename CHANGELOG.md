# Codox Changelog

All notable changes to Codox wavetable synthesizer are documented here.

## [2.0.0] - 2026-01-02

### Breaking Changes

- **Complete UI redesign** - Serum-style single-page layout replaces tabbed interface
- **New parameters added** - Preset compatibility with v1.x may be limited

### Added

- **Serum-style single-page layout**
  - All controls visible on one screen (no tabs)
  - Sub + Noise oscillators now on main page with Osc A/B
  - Compact envelope displays
  - FX rack on right side with full parameter access

- **8 Macros** (up from 4)
  - macro5, macro6, macro7, macro8 added to C++ backend
  - All 8 visible in bottom row

- **Full FX parameter controls** (not just Mix)
  - Distortion: Drive control
  - Chorus: Rate, Depth
  - Phaser: Rate, Depth
  - Flanger: Rate, Feedback
  - Delay: Time, Feedback (existing) - now more prominent
  - Reverb: Size, Decay (existing) - now more prominent
  - EQ: Low, Mid, High band controls
  - Compressor: Threshold, Ratio, Attack, Release

- **Drag-and-drop FX chain reordering**
  - Drag any FX unit to reorder the processing chain
  - Visual feedback during drag

- **Custom save modal**
  - Replaced browser prompt() with styled modal
  - Preset file download (.codox format)

- **External preset loading**
  - LOAD button opens file picker
  - Supports .json and .codox files

### Fixed

- **Save button not working** - Custom modal replaces blocked browser prompt()
- **Page switching during FX adjustment** - Event propagation properly stopped
- **Sub oscillator hidden** - Now visible on main page

### Changed

- UI width increased to 1000px for more space
- Envelopes more compact (35px display height)
- LFOs in horizontal row (4 columns)
- Color scheme updated with better contrast

## [1.0.4] - 2026-01-02

### Added

- **Working preset system** - Presets now actually load and save
  - 10 factory presets: Init, Serum Lead, Fat Bass, Ethereal Pad, Pluck, Growl Bass, Supersaw, Bell, Wobble, Ambient
  - Each preset has distinct sound characteristics
  - User presets saved to localStorage
  - Save button prompts for name and stores current state
  - Factory presets protected from overwrite
  - Randomize button creates musically useful random patches

### Fixed

- **Presets not loading** - loadPreset() now applies parameter values via JUCE slider states
- **Save button not working** - savePreset() now captures all knob values and stores to localStorage
- **Randomize not affecting sound** - Now uses applyState() to set JUCE parameters

## [1.0.3] - 2026-01-02

### Fixed

- **FX knobs not connected to JUCE** - Added C++ relays and attachments for Delay Time, Delay Feedback, Reverb Size, and Reverb Decay parameters
  - Added 4 new WebSliderRelay declarations in PluginEditor.h
  - Added 4 new WebSliderParameterAttachment declarations
  - Added relay initialization in constructor
  - Added withOptionsFrom() registration for WebView
  - Added attachment bindings to APVTS parameters

## [1.0.2] - 2026-01-02

### Added

- **Delay parameters** - Added Time and Feedback knobs to Delay effect (UI only)
- **Reverb parameters** - Added Size and Decay knobs to Reverb effect (UI only)

### Fixed

- **FX toggle buttons switching tabs** - Added `e.stopPropagation()` to prevent clicks from bubbling up to tab handlers
- **FX toggle buttons not controlling effects** - Toggle now sets mix to 0 (off) or restores previous value (on)
- **FX toggle state desync** - Toggle state now syncs when mix knob is changed directly
- **Sync buttons event bubbling** - Added `e.stopPropagation()` to LFO sync buttons

## [1.0.1] - 2026-01-02

### Fixed

- **Critical: WebView knobs not affecting audio** - Fixed bidirectional parameter binding
  - Root cause: JavaScript `initKnobs()` only updated local hidden inputs, never sent values to JUCE
  - Fix: Store JUCE slider states globally in `sliderStates` object, call `state.setNormalisedValue()` during drag
  - Added: ComboBox bindings now properly send changes to JUCE via `state.setChoiceIndex()`

- **Parameter ID mismatches between HTML and C++**
  - Fixed: `osc_a_warp` -> `osc_a_warp_amount`
  - Fixed: `osc_b_warp` -> `osc_b_warp_amount`
  - Fixed: `filter_env` -> `filter_env_depth`
  - Updated: Hidden inputs to match correct parameter IDs

### Technical Details

- All 58+ knobs now properly bound bidirectionally (UI <-> JUCE APVTS)
- Added console logging for debugging parameter binding
- No DSP changes - audio processing code was correct, only UI binding was broken

## [1.0.0] - 2026-01-01

### Added

- Initial release of Codox wavetable synthesizer
- Dual oscillators (A/B) with 4 wavetables each (Basic, Analog, Digital, Vocal)
- Per-oscillator controls: Position, Level, Pan, Octave, Semitone, Fine, Warp Mode, Warp Amount
- Sub oscillator (Sine/Triangle/Square) with octave selection
- Noise oscillator (White/Pink/Digital)
- Multimode filter (LP24, LP12, HP24, BP12, Notch) with cutoff, resonance, drive
- Filter envelope with keytracking
- Amp ADSR envelope
- Filter ADSR envelope
- 4 LFOs with multiple shapes (Sine, Tri, Saw, Square, S&H) and tempo sync
- 8-voice polyphony with voice stealing
- Unison (1-16 voices) with detune control
- Glide/portamento
- 8 effects: Distortion, Chorus, Phaser, Flanger, Delay, Reverb, EQ, Compressor
- Delay and modulation tempo sync
- 4 assignable macro knobs
- WebView UI with Serum-style design
- 3D wavetable visualization
- Real-time filter curve, envelope, and LFO displays
