# Codox - Creative Brief

## Overview

**Type:** Synthesizer
**Core Concept:** Advanced wavetable synthesizer with Serum 2-level feature parity — same functionality, unique visual identity
**Status:** Ideated
**Created:** 2026-01-01

## Vision

Codox is a professional-grade wavetable synthesizer designed as a versatile workhorse for all genres. It delivers the full power of modern wavetable synthesis: deep modulation, extensive effects, and comprehensive wavetable manipulation tools. The goal is Serum-level functionality with a distinct visual aesthetic. A randomize button enables rapid sound exploration and happy accidents.

## Architecture

### Oscillators
- **2 Wavetable Oscillators** — Full wavetable playback with position control, morphing, and all warp modes
- **1 Sub Oscillator** — Sine, triangle, square options for low-end foundation
- **1 Noise Oscillator** — Multiple noise types (white, pink, digital, etc.)

### Warp Modes (10+)
- Sync, Bend+, Bend-, Bend +/-, FM (from B), FM (from Sub), AM (from B), RM (from B), Formant, Quantize, PWM, Asym+, Asym-, Flip, Mirror

### Filters (20+ types)
- **Low Pass:** 12dB, 24dB, LP Ladder, LP German, LP Vintage
- **High Pass:** 12dB, 24dB, HP Ladder
- **Band Pass:** BP 12, BP 24
- **Notch:** Notch 12, Notch 24
- **Comb:** Comb+, Comb-
- **Formant:** Formant I, Formant II
- **Special:** Phaser, Flanger, Ring Mod, Sample & Hold, French LP, Reverb Filter

### Modulation
- **4 LFOs** — Multiple shapes (sine, triangle, saw, square, S&H, custom), sync options, retrigger modes
- **3 Envelopes** — ADSR with curve control, looping options
- **Mod Matrix** — Any source to any destination, with depth and curve controls
- **Macros** — 4 macro knobs for performance control

### Effects Rack (10+ effects)
1. Distortion (multiple algorithms: soft clip, hard clip, tube, tape, foldback, etc.)
2. Chorus
3. Phaser
4. Flanger
5. Delay (stereo, ping-pong, tempo-sync)
6. Reverb (algorithmic, multiple room types)
7. EQ (parametric 3-band)
8. Compressor
9. Filter (independent from main filter)
10. Hyper/Dimension (stereo width/unison effect)

### Voices
- **Polyphony:** Up to 16 voices
- **Unison:** Up to 16 voices per note with detune, blend, and stereo spread
- **Portamento/Glide:** Multiple modes (always, legato, off)

### Wavetable Engine
- **Factory Library:** 200+ wavetables across categories (basic, digital, analog, vocal, evolving, etc.)
- **Audio Import:** Drag audio files to generate wavetables
- **Wavetable Editor:** Draw, generate, and manipulate waveforms directly
- **Frame Morphing:** Smooth interpolation between wavetable frames

## Parameters (Core)

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Osc1 Wavetable | 0-N | 0 | Wavetable selection for oscillator 1 |
| Osc1 Position | 0-100% | 0% | Wavetable frame position |
| Osc1 Warp | Mode select | Sync | Warp mode for oscillator 1 |
| Osc1 Warp Amount | 0-100% | 0% | Warp intensity |
| Osc1 Level | 0-100% | 100% | Oscillator 1 volume |
| Osc1 Pan | -100 to +100 | 0 | Stereo position |
| Osc1 Octave | -4 to +4 | 0 | Octave offset |
| Osc1 Semi | -12 to +12 | 0 | Semitone offset |
| Osc1 Fine | -100 to +100 cents | 0 | Fine tuning |
| Osc2 Wavetable | 0-N | 0 | Wavetable selection for oscillator 2 |
| Osc2 Position | 0-100% | 0% | Wavetable frame position |
| Osc2 Warp | Mode select | Sync | Warp mode for oscillator 2 |
| Osc2 Warp Amount | 0-100% | 0% | Warp intensity |
| Osc2 Level | 0-100% | 100% | Oscillator 2 volume |
| Osc2 Pan | -100 to +100 | 0 | Stereo position |
| Osc2 Octave | -4 to +4 | 0 | Octave offset |
| Osc2 Semi | -12 to +12 | 0 | Semitone offset |
| Osc2 Fine | -100 to +100 cents | 0 | Fine tuning |
| Sub Level | 0-100% | 0% | Sub oscillator volume |
| Sub Shape | Sine/Tri/Square | Sine | Sub oscillator waveform |
| Sub Octave | -2 to 0 | -1 | Sub octave offset |
| Noise Level | 0-100% | 0% | Noise oscillator volume |
| Noise Type | Select | White | Noise type selection |
| Filter Type | Select | LP 24 | Filter algorithm |
| Filter Cutoff | 20Hz-20kHz | 20kHz | Filter cutoff frequency |
| Filter Resonance | 0-100% | 0% | Filter resonance |
| Filter Drive | 0-100% | 0% | Filter saturation |
| Filter Env Depth | -100 to +100% | 0% | Envelope modulation depth |
| Filter Keytrack | 0-100% | 0% | Keyboard tracking amount |
| Amp Attack | 0-10s | 10ms | Amplitude envelope attack |
| Amp Decay | 0-10s | 100ms | Amplitude envelope decay |
| Amp Sustain | 0-100% | 100% | Amplitude envelope sustain |
| Amp Release | 0-10s | 200ms | Amplitude envelope release |
| Filter Attack | 0-10s | 10ms | Filter envelope attack |
| Filter Decay | 0-10s | 100ms | Filter envelope decay |
| Filter Sustain | 0-100% | 50% | Filter envelope sustain |
| Filter Release | 0-10s | 200ms | Filter envelope release |
| LFO1 Rate | 0.01Hz-100Hz | 1Hz | LFO 1 speed |
| LFO1 Shape | Select | Sine | LFO 1 waveform |
| LFO1 Depth | 0-100% | 50% | LFO 1 amount |
| Master Volume | -inf to +6dB | 0dB | Master output level |
| Unison Voices | 1-16 | 1 | Unison voice count |
| Unison Detune | 0-100% | 25% | Unison detuning amount |
| Unison Blend | 0-100% | 50% | Unison mix balance |
| Glide Time | 0-10s | 0ms | Portamento time |
| Glide Mode | Off/Always/Legato | Off | Portamento mode |
| Randomize | Trigger | — | Randomize all parameters |

## UI Concept

**Layout:** Different visual identity from Serum — unique aesthetic while maintaining familiar workflow
**Visual Style:** To be determined in mockup phase — distinct from Serum's dark blue
**Key Elements:**
- Wavetable 3D visualizer per oscillator
- Draggable modulation routing (visual connections)
- Collapsible effects rack
- Large wavetable position displays
- Randomize button (prominent placement)
- Macro knobs with visual feedback

## Use Cases

- EDM bass design (growls, reeses, sub basses)
- Lead synthesis (screaming leads, plucks, stabs)
- Pad creation (evolving textures, lush atmospheres)
- Sound design (experimental textures, cinematic elements)
- Pop/electronic production (modern synth sounds)

## Inspirations

- Serum 2 (primary reference — feature parity goal)
- Vital (open-source wavetable synth)
- Phase Plant (modular approach)
- Pigments (wavetable + granular)

## Technical Notes

- **DSP Complexity:** Very High — wavetable interpolation, 20+ filter algorithms, 10+ warp modes, extensive modulation routing
- **CPU Consideration:** 16-voice unison with full effects will be CPU-intensive; consider quality settings
- **Wavetable Format:** Standard 2048-sample frames, variable frame count
- **Implementation Phases Recommended:**
  1. Core oscillators + basic filter + amp envelope
  2. Full filter set + mod matrix
  3. Effects rack
  4. Wavetable import/editor
  5. Full UI polish

## Next Steps

- [ ] Create UI mockup (`/dream Codox` -> option for mockup)
- [ ] Start implementation (`/implement Codox`)
