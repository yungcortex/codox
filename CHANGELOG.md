# Codox Changelog

All notable changes to Codox wavetable synthesizer are documented here.

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
