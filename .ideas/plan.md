# Codox - Implementation Plan

**Date:** 2026-01-01
**Complexity Score:** 9.2 (Very High - Complex, phased implementation required)
**Strategy:** Phase-based implementation (mandatory due to extreme complexity)

---

## Complexity Factors

**Calculation breakdown:**

- **Parameters:** 50 parameters (50/5 = 10.0, capped at 2.0) = **2.0**
- **Algorithms:** 15 DSP components = **15.0**
  - Wavetable Oscillator A (custom implementation)
  - Wavetable Oscillator B (custom implementation)
  - Sub Oscillator (juce::dsp::Oscillator)
  - Noise Oscillator (custom implementation)
  - Filter Bank (5 filter types: LP24, LP12, HP24, BP12, Notch)
  - Amp Envelope (juce::ADSR)
  - Filter Envelope (juce::ADSR)
  - LFO 1-4 (juce::dsp::Oscillator × 4)
  - Distortion (custom waveshaping)
  - Chorus (juce::dsp::Chorus)
  - Phaser (juce::dsp::Phaser)
  - Flanger (custom delay-based)
  - Delay (juce::dsp::DelayLine)
  - Reverb (juce::dsp::Reverb)
  - EQ (juce::dsp::IIR::Filter × 3 bands)
  - Compressor (juce::dsp::Compressor)
- **Features:** 5 points
  - Polyphonic synthesis (16 voices) = +1
  - Unison processing (up to 16 virtual voices per voice) = +1
  - MIDI input and routing = +1
  - Custom wavetable engine (frame interpolation, warp modes) = +1
  - Serial effects chain (8 effects) = +1
- **Subtotal:** 2.0 + 15.0 + 5.0 = **22.0**
- **Total (capped at 10.0):** **9.2** (Tier 6: Extreme complexity - synthesizer with extensive modulation)

**Complexity tier:** Tier 6 (Real-time synthesis with extensive modulation and effects)
**Research depth:** DEEP (30+ minutes research executed)

---

## Stages

- Stage 0: Research ✓ (architecture.md created)
- Stage 1: Planning ✓ (plan.md created - THIS FILE)
- Stage 1: Foundation ← Next (CMakeLists.txt, project structure, IS_SYNTH flag)
- Stage 2: Shell (APVTS parameters - 50 parameters)
- Stage 3: DSP - **PHASED IMPLEMENTATION** (6 phases)
- Stage 3: GUI - **PHASED IMPLEMENTATION** (3 phases)
- Stage 3: Validation

---

## Complex Implementation (Score = 9.2)

### Stage 3: DSP Phases

**CRITICAL:** Due to extreme complexity (score 9.2), DSP implementation is broken into 6 phases with explicit test criteria and fallback architectures. Each phase is committed independently with validation before proceeding.

---

#### Phase 3.1: Core Voice Architecture (VALIDATION PHASE)

**Goal:** Establish voice management, MIDI routing, and basic synthesis with simple sine oscillator. VALIDATE that IS_SYNTH flag works, voices allocate correctly, and MIDI triggers sound.

**Components:**
- Voice management (16 polyphonic voices, round-robin allocation)
- MIDI input handling (note-on/note-off, velocity)
- Basic sine oscillator (NOT wavetable yet - simple validation)
- Amp envelope (juce::ADSR per voice)
- Master volume output

**Test Criteria:**
- [ ] Plugin loads in DAW as INSTRUMENT (not effect - IS_SYNTH flag working)
- [ ] MIDI note-on triggers sine wave (basic sound generation)
- [ ] MIDI note-off releases amp envelope (clean release)
- [ ] 16 voices polyphonic (play 16 notes simultaneously without voice stealing)
- [ ] Voice stealing works when >16 notes pressed (oldest voice stolen)
- [ ] Velocity affects amplitude (louder velocity = louder output)
- [ ] No clicks or pops on note-on/note-off (envelope smooth)
- [ ] Master volume parameter works correctly (dB to linear gain)

**Duration estimate:** ~3 hours

**Why start here:** Validates entire architecture BEFORE implementing complex wavetable engine. If voice management broken, fix early before investing in wavetable complexity.

---

#### Phase 3.2: Wavetable Engine (HIGH RISK PHASE)

**Goal:** Replace sine oscillator with full wavetable engine (frame interpolation, position morphing, warp modes). This is the highest-risk component of the entire project.

**Components:**
- Wavetable format (256 frames × 2048 samples per frame)
- 4 built-in wavetables (Basic, Analog, Digital, Vocal) embedded in binary
- Wavetable frame interpolation (linear interpolation between frames)
- Position morphing (0-100% parameter → frame index 0-255)
- Pitch calculation (octave/semitone/fine tuning)
- Warp modes (5 algorithms: Sync, Bend+, FM, AM, PWM)
- Wavetable Oscillator A (full implementation)
- Wavetable Oscillator B (duplicate of Osc A)

**Sub-phase breakdown:**
1. **Sub-phase 3.2a:** Basic wavetable reader (single wavetable, no morphing, no warp)
   - Load wavetable data into memory
   - Phase accumulation and sample reading
   - Test: Wavetable playback sounds correct (no artifacts)
   
2. **Sub-phase 3.2b:** Frame interpolation (position morphing)
   - Linear interpolation between adjacent frames
   - Position parameter (0-100%) controls frame index
   - Test: Smooth morphing (no zipper noise, no pops)
   
3. **Sub-phase 3.2c:** Warp modes (one-by-one)
   - Implement each warp mode independently (Sync, Bend+, FM, AM, PWM)
   - Test each for artifacts and CPU usage
   - Test: Each warp mode sounds correct, no extreme CPU spikes
   
4. **Sub-phase 3.2d:** Second oscillator (Osc B)
   - Duplicate Osc A implementation
   - Independent wavetable selection, position, tuning, warp
   - Test: Both oscillators mix correctly, no phase cancellation

**Test Criteria:**
- [ ] Wavetable playback sounds correct (no clicks, pops, discontinuities)
- [ ] Frame interpolation is smooth (no zipper noise when morphing)
- [ ] Position parameter (0-100%) morphs through wavetable smoothly
- [ ] Octave/semitone/fine tuning works correctly (pitch-perfect)
- [ ] All 5 warp modes sound correct (Sync, Bend+, FM, AM, PWM)
- [ ] Osc A and Osc B mix without artifacts (level and pan work)
- [ ] CPU usage acceptable (<30% single core for 16 voices, no warp)
- [ ] Wavetable switching (4 wavetables) works without glitches

**Duration estimate:** ~8 hours (2hr basic wavetable + 2hr frame interpolation + 3hr warp modes + 1hr Osc B)

**Fallback architecture:**
- If frame interpolation causes artifacts → Use pre-rendered wavetable positions (no interpolation)
- If warp modes too complex → Omit warp modes (basic wavetable only)
- If CPU too high → Reduce wavetable resolution (128 frames instead of 256)
- If wavetable engine fails entirely → Fall back to standard oscillator (sine, saw, square)

**CRITICAL:** This phase represents ~40% of total project risk. Test incrementally, be ready to fall back.

---

#### Phase 3.3: Oscillator Expansion + Filter Bank

**Goal:** Add Sub oscillator, Noise oscillator, and filter bank with multiple filter types. Complete core subtractive synthesis path.

**Components:**
- Sub Oscillator (sine, triangle, square waveforms, -2 to 0 octave offset)
- Noise Oscillator (white, pink, digital noise types)
- Filter Bank (5 filter types: LP 24dB, LP 12dB, HP 24dB, BP 12dB, Notch)
- Filter Envelope (juce::ADSR per voice)
- Filter modulation (envelope depth, keytrack)

**Test Criteria:**
- [ ] Sub oscillator generates correct waveforms (sine, triangle, square)
- [ ] Sub octave offset works correctly (-2, -1, 0 octaves below note)
- [ ] Noise oscillator generates correct noise types (white, pink, digital)
- [ ] All 5 filter types sound correct (LP24, LP12, HP24, BP12, Notch)
- [ ] Filter cutoff sweeps smoothly (20Hz to 20kHz, no clicks)
- [ ] Filter resonance increases Q factor correctly (0-100% → 0.5-10.0)
- [ ] Filter envelope modulates cutoff correctly (attack/decay/sustain/release)
- [ ] Filter keytrack follows MIDI note pitch (0% = fixed, 100% = 1:1)
- [ ] Filter drive applies pre-filter saturation (0-100% → 1.0-10.0x gain)
- [ ] All oscillators (Osc A, Osc B, Sub, Noise) mix correctly
- [ ] No artifacts when switching filter types

**Duration estimate:** ~6 hours (2hr Sub+Noise + 3hr Filter Bank + 1hr Filter Envelope)

**Fallback architecture:**
- If 5 filter types too complex → Reduce to 2 types (LP 24dB, HP 24dB only)
- If filter envelope causes artifacts → Simplify envelope (fixed attack/release)
- If keytrack broken → Omit keytrack (fixed cutoff only)

---

#### Phase 3.4: Unison Processing (HIGH RISK PHASE)

**Goal:** Implement unison voice expansion (up to 16 virtual voices per allocated voice). This is the second-highest risk component due to extreme CPU scaling.

**Components:**
- Unison voice expansion (1, 2, 4, 8, 16 virtual voices per note)
- Detuning calculation (symmetric spread around base pitch)
- Stereo spread (pan virtual voices across stereo field)
- Voice summation (mix all virtual voices)

**Sub-phase breakdown:**
1. **Sub-phase 3.4a:** 2 unison voices (test basic detuning and stereo spread)
2. **Sub-phase 3.4b:** 4 unison voices (test CPU scaling)
3. **Sub-phase 3.4c:** 8 unison voices (test CPU at medium unison count)
4. **Sub-phase 3.4d:** 16 unison voices (test worst-case CPU: 16 voices × 16 unison = 256 oscillators)

**Test Criteria:**
- [ ] Unison creates lush chorusing effect (not just louder)
- [ ] Detuning spreads voices symmetrically (±detune around base pitch)
- [ ] Stereo spread creates wide image (not mono sum)
- [ ] Unison count (1, 2, 4, 8, 16) scales correctly
- [ ] Detune parameter (0-100%) controls detuning amount
- [ ] CPU usage scales linearly with unison count (2× unison = 2× CPU)
- [ ] No phase cancellation (virtual voices don't cancel each other)
- [ ] Unison works with all oscillators (Osc A, Osc B, Sub, Noise)
- [ ] **CRITICAL:** 16 voices × 16 unison = CPU usage acceptable (<150% single core)

**Duration estimate:** ~5 hours (1hr per sub-phase + 1hr optimization)

**Fallback architecture:**
- If 16 unison too CPU-intensive → Reduce max unison to 8 voices
- If still too CPU-intensive → Reduce max unison to 4 voices
- If unison causes artifacts → Omit unison feature (1 voice per note only)

**CRITICAL:** This phase represents ~40% of total project risk. Benchmark CPU at each sub-phase. Be ready to reduce max unison count.

---

#### Phase 3.5: Effects Chain

**Goal:** Implement 8 effects in serial chain with fixed internal parameters (only mix exposed).

**Components:**
- Distortion (tanh waveshaping, drive = 5.0 fixed)
- Chorus (juce::dsp::Chorus, rate = 0.5Hz, depth = 0.3, delay = 7ms)
- Phaser (juce::dsp::Phaser, rate = 0.3Hz, depth = 0.5, feedback = 0.4)
- Flanger (custom delay-based, rate = 0.2Hz, delay = 1-10ms, feedback = 0.6)
- Delay (juce::dsp::DelayLine, time = 500ms, feedback = 0.4)
- Reverb (juce::dsp::Reverb, size = 0.7, damping = 0.5)
- EQ (3-band: Low +3dB@100Hz, Mid 0dB@1kHz, High +2dB@8kHz)
- Compressor (juce::dsp::Compressor, threshold = -20dB, ratio = 4:1, attack = 10ms, release = 100ms)

**Sub-phase breakdown (add effects one-by-one):**
1. Distortion
2. Chorus
3. Phaser
4. Flanger
5. Delay
6. Reverb
7. EQ
8. Compressor

**Test Criteria:**
- [ ] Each effect sounds correct when mix = 100% (wet only)
- [ ] Each effect mix parameter works (0% = dry, 100% = wet, 50% = blend)
- [ ] Effects chain order is correct (Distortion → ... → Compressor)
- [ ] No artifacts when switching effects on/off (smooth transitions)
- [ ] Latency reported correctly (getLatencySamples includes all effect latency)
- [ ] CPU usage acceptable with all effects active (<25% additional CPU)
- [ ] No runaway feedback in Delay or Reverb (feedback = 0.4 stable)

**Duration estimate:** ~6 hours (8 effects × 40min each + 1hr latency testing)

**Fallback architecture:**
- If 8 effects too complex → Reduce to 4 core effects (Distortion, Delay, Reverb, Compressor)
- If CPU too high → Reduce to 2 effects (Delay, Reverb only)
- If latency issues → Remove Delay and Reverb (reduce to 6 effects)

---

#### Phase 3.6: LFOs + Glide + Polish

**Goal:** Implement LFO generation (NO routing in v1.0), glide/portamento, and final DSP polish.

**Components:**
- LFO 1-4 (generate waveforms: Sine, Triangle, Saw, Square, S&H)
- LFO parameters (shape, rate, sync - sync inactive in v1.0)
- **NOTE:** LFOs generate signals but NO routing (no mod matrix in parameter spec)
- Glide/Portamento (exponential pitch smoothing, 0-10s)
- CPU optimization (profile hot paths, SIMD if needed)
- Denormal protection (ScopedNoDenormals, epsilon in feedback paths)
- Parameter smoothing (prevent zipper noise on cutoff/resonance changes)

**Test Criteria:**
- [ ] LFO 1-4 generate correct waveforms (sine, triangle, saw, square, S&H)
- [ ] LFO rate parameter works (0.01Hz to 100Hz)
- [ ] **LFOs do NOT modulate anything** (no routing in v1.0 - expected behavior)
- [ ] Glide smoothly transitions between notes (exponential curve)
- [ ] Glide time parameter works (0s = instant, 10s = slow slide)
- [ ] CPU usage optimized (profile shows no unexpected hot spots)
- [ ] Denormal protection active (no CPU spikes from denormal processing)
- [ ] Parameter smoothing prevents zipper noise (cutoff/resonance sweeps smooth)
- [ ] All parameters respond correctly to automation
- [ ] No memory leaks (Valgrind or Instruments profiling)

**Duration estimate:** ~4 hours (2hr LFOs + 1hr Glide + 1hr Polish/Optimization)

---

### Stage 3: GUI Phases

**CRITICAL:** GUI implementation is broken into 3 phases to manage complexity (50 parameters, 9 visualization canvases, 4 LFO previews).

---

#### Phase 4.1: Layout and Basic Controls

**Goal:** Integrate finalized UI mockup HTML and establish WebView window with correct layout.

**Components:**
- Copy v6-ui.html (finalized mockup) to Source/ui/public/index.html
- Update PluginEditor.h/cpp with WebView setup
- Configure CMakeLists.txt for WebView resources (juce_add_binary_data)
- Add resource provider (getResource() method with explicit URL mapping)
- WebView window sizing (1200×800 as per mockup)
- Basic HTML/CSS rendering (no parameter bindings yet)

**Test Criteria:**
- [ ] WebView window opens with correct size (1200×800)
- [ ] All UI sections visible (Header, Oscillators, Filter, Envelopes, LFOs, Macros, Voice, Effects)
- [ ] Layout matches mockup design (spacing, alignment, colors)
- [ ] Background and styling render properly (CSS loaded correctly)
- [ ] No console errors (check browser dev tools via JUCE WebView inspector)
- [ ] HTML renders completely (no missing images or broken resources)

**Duration estimate:** ~2 hours

---

#### Phase 4.2: Parameter Binding and Interaction

**Goal:** Establish two-way parameter communication (UI ↔ DSP) for all 50 parameters.

**Components:**
- WebSliderRelay creation (38 parameters: all float/int sliders)
- WebComboBoxRelay creation (8 parameters: all dropdowns)
- WebToggleButtonRelay creation (4 parameters: all LFO sync toggles)
- WebSliderParameterAttachment (38 attachments with nullptr for undoManager - JUCE 8 requirement)
- WebComboBoxParameterAttachment (8 attachments)
- WebToggleButtonParameterAttachment (4 attachments)
- JavaScript relay integration (import { getSliderState, getToggleState } from './js/juce/index.js')
- Knob interaction (relative drag pattern - NOT absolute positioning)
- Slider interaction (vertical drag for levels, horizontal for pan)
- Dropdown interaction (click to open, select option)
- Toggle interaction (click to flip boolean)

**Test Criteria:**
- [ ] All 50 parameters bound correctly (38 sliders + 8 dropdowns + 4 toggles)
- [ ] Knobs rotate when dragged (relative drag, not absolute jump)
- [ ] Sliders move when dragged (vertical or horizontal depending on control)
- [ ] Dropdowns show options when clicked (wavetable, filter type, warp mode, etc.)
- [ ] Toggles flip state when clicked (LFO sync on/off)
- [ ] Parameter changes update DSP (knob drag → audio changes)
- [ ] Host automation updates UI (DAW automation → knobs rotate)
- [ ] Preset changes update all UI elements (load preset → all controls reflect values)
- [ ] Parameter values display correctly (numeric readouts match actual values)
- [ ] No lag or visual glitches (smooth parameter updates)
- [ ] ES6 module imports work (type="module" on script tags)
- [ ] valueChangedEvent listeners call getNormalisedValue() INSIDE callback (no parameters passed)

**Duration estimate:** ~6 hours (50 parameters × ~7min each + 1hr testing)

**CRITICAL:** Check juce8-critical-patterns.md for:
- Pattern 12: WebSliderParameterAttachment requires 3 parameters (add nullptr for undoManager)
- Pattern 13: check_native_interop.js required in binary resources
- Pattern 15: valueChangedEvent callback receives NO parameters (call getNormalisedValue() inside)
- Pattern 21: Use type="module" for ES6 imports

---

#### Phase 4.3: Visualizations and Advanced UI

**Goal:** Implement real-time visualizations (waveform displays, ADSR curves, LFO previews, filter curve).

**Components:**
- Wavetable waveform displays (Osc A, Osc B) - animated based on position parameter
- Filter curve display (frequency response visualization)
- Amp ADSR curve display (envelope shape based on A/D/S/R parameters)
- Filter ADSR curve display (envelope shape based on A/D/S/R parameters)
- LFO waveform previews (LFO1-4 shape visualization)
- Canvas rendering (HTML5 Canvas API)
- Real-time updates (requestAnimationFrame loop for smooth animation)

**Test Criteria:**
- [ ] Wavetable displays show correct waveform (update when position changes)
- [ ] Wavetable displays animate smoothly (position morphing visible)
- [ ] Filter curve shows correct frequency response (LP/HP/BP/Notch curves)
- [ ] Filter curve updates when cutoff/resonance change (real-time curve morphing)
- [ ] Amp ADSR curve shows correct envelope shape (A/D/S/R segments visible)
- [ ] Filter ADSR curve shows correct envelope shape (independent from Amp ADSR)
- [ ] LFO previews show correct waveform (sine, triangle, saw, square, S&H)
- [ ] LFO previews update when shape parameter changes
- [ ] All visualizations run at 60fps (no jitter or stuttering)
- [ ] Canvas rendering doesn't spike CPU (requestAnimationFrame efficient)
- [ ] Visualizations match parameter values (curve updates sync with parameter changes)

**Duration estimate:** ~4 hours (2hr wavetable displays + 1hr ADSR curves + 1hr LFO previews)

**CRITICAL:** Check juce8-critical-patterns.md for:
- Pattern 20: VU meters need requestAnimationFrame loop (separate current/target with interpolation)
- Apply same pattern to all animated visualizations (smooth motion, not instant jumps)

---

### Implementation Flow

- Stage 0: Research ✓ (architecture.md created with DEEP research)
- Stage 1: Planning ✓ (plan.md created - THIS FILE)
- Stage 1: Foundation (next: CMakeLists.txt, project structure, IS_SYNTH flag)
- Stage 2: Shell (APVTS parameters - 50 parameters)
- Stage 3: DSP - 6 phases
  - Phase 3.1: Core Voice Architecture (~3hr)
  - Phase 3.2: Wavetable Engine (~8hr) **HIGH RISK**
  - Phase 3.3: Oscillator Expansion + Filter Bank (~6hr)
  - Phase 3.4: Unison Processing (~5hr) **HIGH RISK**
  - Phase 3.5: Effects Chain (~6hr)
  - Phase 3.6: LFOs + Glide + Polish (~4hr)
  - **Total DSP:** ~32 hours
- Stage 3: GUI - 3 phases
  - Phase 4.1: Layout and Basic Controls (~2hr)
  - Phase 4.2: Parameter Binding and Interaction (~6hr)
  - Phase 4.3: Visualizations and Advanced UI (~4hr)
  - **Total GUI:** ~12 hours
- Stage 3: Validation (presets, pluginval, changelog)
  - **Total Validation:** ~2 hours

**Grand Total Estimate:** ~46 hours (excluding Foundation/Shell/Validation)

---

## Implementation Notes

### Thread Safety

- All parameter reads use atomic `getRawParameterValue()->load()` (APVTS atomic access)
- Voice state is per-voice (no shared state between voices)
  - Each voice has own: oscillator phases, envelopes, filter state
- LFO phase state is global but read-only from audio thread (no race conditions)
- Wavetable data is read-only after prepareToPlay (no thread conflicts)
- No locks in audio thread (all operations are lock-free)

**Example:**
```cpp
// Audio thread parameter access (atomic read)
float cutoff = *apvts.getRawParameterValue("filter_cutoff");
float resonance = *apvts.getRawParameterValue("filter_resonance");

// Per-voice state (no shared memory)
for (int v = 0; v < 16; ++v) {
    voices[v].oscA_phase += voices[v].oscA_freq / sampleRate; // Independent per voice
}
```

---

### Performance

**Estimated CPU usage breakdown:**
- **Worst case (16 voices × 16 unison × all oscillators + effects):** ~800% CPU (8 cores) - UNREALISTIC
- **Realistic usage (4 voices × 4 unison + effects):** ~120% CPU (1.2 cores) - TYPICAL
- **Conservative target:** 80-150% single core for typical patches

**Hot paths to profile:**
1. Wavetable frame interpolation (inner loop, per-sample) - Consider SIMD optimization
2. Filter processing (per-voice, per-sample) - Pre-calculate coefficients
3. Unison detuning (multiplicative CPU scaling) - Add quality settings if needed
4. Voice deactivation (skip silent voices) - Check amp envelope state

**Buffer size sensitivity:**
- Target: Stable at 256 samples (typical DAW buffer size)
- Smaller buffers (64 samples) = higher CPU overhead (more frequent processBlock calls)
- Larger buffers (512 samples) = lower CPU but higher latency

---

### Latency

**Processing latency sources:**
- Wavetable oscillators: ~1 sample (phase interpolation)
- Filters: <1 sample (IIR group delay)
- Effects:
  - Reverb: ~40ms (algorithmic latency)
  - Chorus/Phaser/Flanger: ~5-10ms (delay line latency)
  - Delay: 500ms (user-intended, not plugin-induced)
- **Total plugin latency:** ~50-60ms (report via getLatencySamples)

**Host compensation:**
- Calculate latency: Sum of Reverb + Chorus + Phaser + Flanger latencies
- Exclude Delay effect (user-intended latency, not for compensation)
- Report via `getLatencySamples()` for DAW compensation

**Example:**
```cpp
int getLatencySamples() const override
{
    return reverb.getLatencySamples() + chorus.getLatencySamples() + 
           phaser.getLatencySamples() + flanger.getLatencySamples();
    // ~2700 samples at 48kHz = ~56ms
}
```

---

### Denormal Protection

- Use `juce::ScopedNoDenormals` in processBlock() (flush-to-zero mode)
- All JUCE DSP components handle denormals internally (juce::dsp::Filter, juce::dsp::Reverb)
- Custom oscillators use phase wrapping (phase always in range 0 to 2π)
- Filter feedback paths include epsilon to prevent denormal oscillation

**Example:**
```cpp
void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
{
    juce::ScopedNoDenormals noDenormals; // Flush-to-zero mode
    
    // ... (DSP processing)
}
```

---

### Known Challenges

**Challenge 1: Wavetable frame interpolation artifacts**
- **Issue:** Linear interpolation can cause zipper noise if position parameter changes rapidly
- **Solution:** Parameter smoothing on position parameter (exponential smoothing with time constant)
- **Reference:** Surge Synthesizer uses smoothed position parameter

**Challenge 2: Unison CPU scaling**
- **Issue:** 16 voices × 16 unison = 256 oscillators (extreme CPU usage)
- **Solution:** Benchmark at each unison count (2, 4, 8, 16), fall back if CPU too high
- **Mitigation:** Add CPU usage warning in UI when unison > 8

**Challenge 3: Warp mode CPU cost**
- **Issue:** FM and AM warp modes require cross-oscillator communication (Osc A reads Osc B output)
- **Solution:** Cache Osc B output in voice state, read during Osc A processing (no additional calculation)
- **Reference:** Vital uses same approach for cross-modulation

**Challenge 4: Effects latency accumulation**
- **Issue:** Serial effects chain accumulates latency (Reverb + Chorus + Phaser + Flanger)
- **Solution:** Report total latency via getLatencySamples(), DAW compensates automatically
- **Test:** Verify latency compensation in Logic Pro and Ableton (mix aligned with other tracks)

**Challenge 5: LFOs with no routing in v1.0**
- **Issue:** Users may expect LFO modulation (creative brief mentions mod matrix)
- **Solution:** Document in changelog that LFOs are v1.0 feature reserved for v1.1 mod matrix
- **Mitigation:** LFO parameters functional (shape, rate), just no routing yet

**Challenge 6: 50 parameters in UI**
- **Issue:** Large parameter count = complex WebView binding (50 relay objects, 50 attachments)
- **Solution:** Follow systematic approach (Phase 4.2 binds all parameters methodically)
- **Reference:** Use existing plugins (TapeAge has 8 params, scale pattern to 50)

---

## References

**Contract files:**
- Creative brief: `plugins/Codox/.ideas/creative-brief.md`
- Parameter spec: `plugins/Codox/.ideas/parameter-spec.md`
- DSP architecture: `plugins/Codox/.ideas/architecture.md` (THIS FILE)
- UI mockup: `plugins/Codox/.ideas/mockups/v6-ui.yaml`

**Similar plugins for reference:**
- **Sleeper (existing plugin):** Synthesizer with oscillators, filter, envelopes (voice management pattern)
  - Reference: Voice allocation, MIDI routing, IS_SYNTH flag setup
- **LushPad (if exists):** Synthesizer with polyphony and envelopes (ADSR pattern)
  - Reference: Per-voice envelope instances, envelope triggering
- **TapeAge (existing plugin):** 8 parameters with WebView UI (parameter binding pattern)
  - Reference: Relay creation, attachment setup, ES6 module imports
- **FlutterVerb (existing plugin):** Effects plugin with reverb, chorus, phaser (effects chain pattern)
  - Reference: juce::dsp::Reverb usage, serial effects routing

**Open-source references:**
- **Surge Synthesizer:** Wavetable implementation, frame interpolation, warp modes
- **Vital Synthesizer:** SIMD-optimized wavetable code, mod matrix architecture
- **JUCE AudioPluginHost:** Example of IS_SYNTH flag usage, synthesizer template

**Technical resources:**
- JUCE 8 Critical Patterns: `troubleshooting/patterns/juce8-critical-patterns.md` (READ BEFORE EACH PHASE)
  - Pattern 12: WebSliderParameterAttachment (3 parameters in JUCE 8)
  - Pattern 13: check_native_interop.js required
  - Pattern 15: valueChangedEvent callback (no parameters passed)
  - Pattern 20: requestAnimationFrame for smooth animations
  - Pattern 21: ES6 module loading (type="module")
  - Pattern 22: IS_SYNTH flag for instruments
- DAFX (Digital Audio Effects): Synthesis chapter (wavetable theory)
- Music DSP Archive: Pink noise filter, constant power panning

---

## Risk Management

### Highest Risk Components (in order)

1. **Wavetable Engine (40% of total risk)**
   - Custom implementation (no JUCE class)
   - Frame interpolation artifacts potential
   - Warp modes complexity (5 algorithms)
   - **Mitigation:** Incremental implementation (basic wavetable → frame interpolation → warp modes)
   - **Fallback:** Omit warp modes if too complex, reduce frame count if CPU too high

2. **Unison Processing (40% of total risk)**
   - Extreme CPU scaling (16 voices × 16 unison = 256 oscillators)
   - Detuning and stereo spread calculations
   - **Mitigation:** Benchmark at each unison count (2, 4, 8, 16), fall back if needed
   - **Fallback:** Reduce max unison to 8 or 4 voices

3. **Effects Chain (15% of total risk)**
   - 8 effects in series (latency accumulation)
   - Fixed parameters must sound good across use cases
   - **Mitigation:** Add effects one-by-one, test each independently
   - **Fallback:** Reduce to 4 core effects if 8 too complex

4. **GUI Complexity (5% of total risk)**
   - 50 parameters to bind (large relay/attachment count)
   - 9 visualization canvases (wavetable displays, ADSR curves, LFO previews)
   - **Mitigation:** Systematic binding (Phase 4.2), requestAnimationFrame for visualizations
   - **Fallback:** Omit visualizations if too complex (static UI only)

### Success Criteria

**Project succeeds if:**
1. Core voice architecture works (MIDI → voices → sound output)
2. Wavetable engine produces clean, artifact-free output
3. Unison creates lush chorusing effect (even if max count reduced to 8)
4. Filter bank and envelopes work correctly (standard subtractive synthesis)
5. At least 4 core effects functional (Distortion, Delay, Reverb, Compressor)
6. All 50 parameters bound to UI (knobs/sliders/dropdowns responsive)
7. CPU usage acceptable (<150% single core for realistic patches)
8. No crashes, glitches, or audio artifacts during normal use

**Project can fall back if:**
- Warp modes omitted (basic wavetable morphing only)
- Max unison reduced to 8 or 4 voices (still lush, lower CPU)
- Effects count reduced to 4 (core effects covered)
- Visualizations omitted (static UI, no animated displays)

**Project fails if:**
- MIDI routing doesn't work (no sound on note-on)
- Wavetable engine has severe artifacts (clicks, pops, discontinuities)
- Voice management crashes or steals voices incorrectly
- CPU usage unacceptable (>200% single core for simple patches)
- UI doesn't bind to parameters (knobs frozen or broken)

---

## Next Steps

1. **Stage 1: Foundation** - Run `/implement Codox` to create CMakeLists.txt and project structure
   - CRITICAL: Ensure IS_SYNTH TRUE flag set (synthesizer, not effect)
   - CRITICAL: Ensure NEEDS_WEB_BROWSER TRUE flag set (WebView UI)
   - CRITICAL: Ensure NEEDS_MIDI_INPUT TRUE flag set (MIDI routing)
   - Verify BusesProperties: Output-only bus (no input bus for synth)

2. **Stage 2: Shell** - Create all 50 APVTS parameters from parameter-spec.md
   - 38 WebSliderRelay parameters (float/int sliders)
   - 8 WebComboBoxRelay parameters (dropdowns)
   - 4 WebToggleButtonRelay parameters (LFO sync toggles)

3. **Stage 3: DSP Phase 3.1** - Core Voice Architecture (validation phase)
   - Implement basic sine oscillator + amp envelope
   - Test MIDI routing and voice allocation
   - Verify IS_SYNTH flag working (plugin appears as instrument in DAW)

4. **Stage 3: DSP Phase 3.2** - Wavetable Engine (highest risk)
   - Sub-phase 3.2a: Basic wavetable reader
   - Sub-phase 3.2b: Frame interpolation
   - Sub-phase 3.2c: Warp modes (one-by-one)
   - Sub-phase 3.2d: Second oscillator (Osc B)
   - **CRITICAL:** Be ready to fall back if artifacts or CPU issues

5. **Continue through phases 3.3-3.6, 4.1-4.3, validation**

**Estimated total time:** ~50 hours (including Foundation, Shell, Validation)
**Recommended approach:** Work in focused 3-4 hour sessions, commit each phase independently
