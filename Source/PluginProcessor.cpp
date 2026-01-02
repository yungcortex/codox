#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Parameter Layout Creation (BEFORE constructor)
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout CodoxAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // ==================== HEADER CONTROLS ====================

    // master_volume - Float (-60 to 6 dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "master_volume", 1 },
        "Master Volume",
        juce::NormalisableRange<float>(-60.0f, 6.0f, 0.1f),
        0.0f,
        "dB"
    ));

    // ==================== OSCILLATOR A ====================

    // osc_a_wavetable - Choice (0-3: Basic, Analog, Digital, Vocal)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "osc_a_wavetable", 1 },
        "Osc A Wavetable",
        juce::StringArray { "Basic", "Analog", "Digital", "Vocal" },
        0
    ));

    // osc_a_position - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "osc_a_position", 1 },
        "Osc A Position",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // osc_a_level - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "osc_a_level", 1 },
        "Osc A Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"
    ));

    // osc_a_pan - Float (-100 to +100)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "osc_a_pan", 1 },
        "Osc A Pan",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f
    ));

    // osc_a_octave - Choice (-4 to +4)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "osc_a_octave", 1 },
        "Osc A Octave",
        juce::StringArray { "-4", "-3", "-2", "-1", "0", "+1", "+2", "+3", "+4" },
        4  // Default: 0 octave (index 4)
    ));

    // osc_a_semitone - Int (-12 to +12)
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "osc_a_semitone", 1 },
        "Osc A Semitone",
        -12,
        12,
        0
    ));

    // osc_a_fine - Int (-100 to +100 cents)
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "osc_a_fine", 1 },
        "Osc A Fine",
        -100,
        100,
        0
    ));

    // osc_a_warp_mode - Choice (0-4: Sync, Bend+, FM, AM, PWM)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "osc_a_warp_mode", 1 },
        "Osc A Warp Mode",
        juce::StringArray { "Sync", "Bend+", "FM", "AM", "PWM" },
        0
    ));

    // osc_a_warp_amount - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "osc_a_warp_amount", 1 },
        "Osc A Warp Amount",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // ==================== OSCILLATOR B ====================

    // osc_b_wavetable - Choice (0-3: Basic, Analog, Digital, Vocal)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "osc_b_wavetable", 1 },
        "Osc B Wavetable",
        juce::StringArray { "Basic", "Analog", "Digital", "Vocal" },
        0
    ));

    // osc_b_position - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "osc_b_position", 1 },
        "Osc B Position",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // osc_b_level - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "osc_b_level", 1 },
        "Osc B Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"
    ));

    // osc_b_pan - Float (-100 to +100)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "osc_b_pan", 1 },
        "Osc B Pan",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f
    ));

    // osc_b_octave - Choice (-4 to +4)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "osc_b_octave", 1 },
        "Osc B Octave",
        juce::StringArray { "-4", "-3", "-2", "-1", "0", "+1", "+2", "+3", "+4" },
        4  // Default: 0 octave (index 4)
    ));

    // osc_b_semitone - Int (-12 to +12)
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "osc_b_semitone", 1 },
        "Osc B Semitone",
        -12,
        12,
        0
    ));

    // osc_b_fine - Int (-100 to +100 cents)
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "osc_b_fine", 1 },
        "Osc B Fine",
        -100,
        100,
        0
    ));

    // osc_b_warp_mode - Choice (0-4: Sync, Bend+, FM, AM, PWM)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "osc_b_warp_mode", 1 },
        "Osc B Warp Mode",
        juce::StringArray { "Sync", "Bend+", "FM", "AM", "PWM" },
        0
    ));

    // osc_b_warp_amount - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "osc_b_warp_amount", 1 },
        "Osc B Warp Amount",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // ==================== SUB + NOISE ====================

    // sub_shape - Choice (0-2: Sine, Triangle, Square)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "sub_shape", 1 },
        "Sub Shape",
        juce::StringArray { "Sine", "Triangle", "Square" },
        0
    ));

    // sub_octave - Choice (-2 to 0)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "sub_octave", 1 },
        "Sub Octave",
        juce::StringArray { "-2", "-1", "0" },
        1  // Default: -1 (index 1)
    ));

    // sub_level - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sub_level", 1 },
        "Sub Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // noise_type - Choice (0-2: White, Pink, Digital)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "noise_type", 1 },
        "Noise Type",
        juce::StringArray { "White", "Pink", "Digital" },
        0
    ));

    // noise_level - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "noise_level", 1 },
        "Noise Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // ==================== FILTER ====================

    // filter_type - Choice (0-4: LP 24dB, LP 12dB, HP 24dB, BP 12dB, Notch)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "filter_type", 1 },
        "Filter Type",
        juce::StringArray { "LP 24dB", "LP 12dB", "HP 24dB", "BP 12dB", "Notch" },
        0
    ));

    // filter_cutoff - Float (20-20000 Hz, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filter_cutoff", 1 },
        "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f),  // Skew 0.3 = logarithmic
        20000.0f,
        "Hz"
    ));

    // filter_resonance - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filter_resonance", 1 },
        "Filter Resonance",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // filter_drive - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filter_drive", 1 },
        "Filter Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // filter_env_depth - Float (-100 to +100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filter_env_depth", 1 },
        "Filter Env Depth",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // filter_keytrack - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filter_keytrack", 1 },
        "Filter Keytrack",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // ==================== AMP ENVELOPE ====================

    // amp_attack - Float (0-10s, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "amp_attack", 1 },
        "Amp Attack",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),  // Skew 0.3 = logarithmic
        0.01f,
        "s"
    ));

    // amp_decay - Float (0-10s, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "amp_decay", 1 },
        "Amp Decay",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.1f,
        "s"
    ));

    // amp_sustain - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "amp_sustain", 1 },
        "Amp Sustain",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"
    ));

    // amp_release - Float (0-10s, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "amp_release", 1 },
        "Amp Release",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.2f,
        "s"
    ));

    // ==================== FILTER ENVELOPE ====================

    // filt_attack - Float (0-10s, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filt_attack", 1 },
        "Filt Attack",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.01f,
        "s"
    ));

    // filt_decay - Float (0-10s, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filt_decay", 1 },
        "Filt Decay",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.1f,
        "s"
    ));

    // filt_sustain - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filt_sustain", 1 },
        "Filt Sustain",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    // filt_release - Float (0-10s, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filt_release", 1 },
        "Filt Release",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.2f,
        "s"
    ));

    // ==================== LFO 1 ====================

    // lfo1_shape - Choice (0-4: Sine, Triangle, Saw, Square, S&H)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "lfo1_shape", 1 },
        "LFO 1 Shape",
        juce::StringArray { "Sine", "Triangle", "Saw", "Square", "S&H" },
        0
    ));

    // lfo1_rate - Float (0.01-100 Hz, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lfo1_rate", 1 },
        "LFO 1 Rate",
        juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.3f),
        1.0f,
        "Hz"
    ));

    // lfo1_sync - Bool
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "lfo1_sync", 1 },
        "LFO 1 Sync",
        false
    ));

    // ==================== LFO 2 ====================

    // lfo2_shape - Choice (0-4: Sine, Triangle, Saw, Square, S&H)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "lfo2_shape", 1 },
        "LFO 2 Shape",
        juce::StringArray { "Sine", "Triangle", "Saw", "Square", "S&H" },
        0
    ));

    // lfo2_rate - Float (0.01-100 Hz, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lfo2_rate", 1 },
        "LFO 2 Rate",
        juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.3f),
        1.0f,
        "Hz"
    ));

    // lfo2_sync - Bool
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "lfo2_sync", 1 },
        "LFO 2 Sync",
        false
    ));

    // ==================== LFO 3 ====================

    // lfo3_shape - Choice (0-4: Sine, Triangle, Saw, Square, S&H)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "lfo3_shape", 1 },
        "LFO 3 Shape",
        juce::StringArray { "Sine", "Triangle", "Saw", "Square", "S&H" },
        0
    ));

    // lfo3_rate - Float (0.01-100 Hz, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lfo3_rate", 1 },
        "LFO 3 Rate",
        juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.3f),
        1.0f,
        "Hz"
    ));

    // lfo3_sync - Bool
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "lfo3_sync", 1 },
        "LFO 3 Sync",
        false
    ));

    // ==================== LFO 4 ====================

    // lfo4_shape - Choice (0-4: Sine, Triangle, Saw, Square, S&H)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "lfo4_shape", 1 },
        "LFO 4 Shape",
        juce::StringArray { "Sine", "Triangle", "Saw", "Square", "S&H" },
        0
    ));

    // lfo4_rate - Float (0.01-100 Hz, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lfo4_rate", 1 },
        "LFO 4 Rate",
        juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.3f),
        1.0f,
        "Hz"
    ));

    // lfo4_sync - Bool
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "lfo4_sync", 1 },
        "LFO 4 Sync",
        false
    ));

    // ==================== MACROS ====================

    // macro1 - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "macro1", 1 },
        "Macro 1",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    // macro2 - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "macro2", 1 },
        "Macro 2",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    // macro3 - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "macro3", 1 },
        "Macro 3",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    // macro4 - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "macro4", 1 },
        "Macro 4",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    // ==================== VOICE ====================

    // unison_voices - Choice (0-4: 1, 2, 4, 8, 16)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "unison_voices", 1 },
        "Unison Voices",
        juce::StringArray { "1", "2", "4", "8", "16" },
        0  // Default: 1 voice
    ));

    // unison_detune - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "unison_detune", 1 },
        "Unison Detune",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        25.0f,
        "%"
    ));

    // glide_time - Float (0-10s, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "glide_time", 1 },
        "Glide Time",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f),
        0.0f,
        "s"
    ));

    // ==================== EFFECTS ====================

    // fx_distortion_mix - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_distortion_mix", 1 },
        "Distortion Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // fx_chorus_mix - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_chorus_mix", 1 },
        "Chorus Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // fx_phaser_mix - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_phaser_mix", 1 },
        "Phaser Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // fx_flanger_mix - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_flanger_mix", 1 },
        "Flanger Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // fx_delay_mix - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_delay_mix", 1 },
        "Delay Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // fx_reverb_mix - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_reverb_mix", 1 },
        "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // fx_eq_mix - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_eq_mix", 1 },
        "EQ Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // fx_compressor_mix - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_compressor_mix", 1 },
        "Compressor Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        "%"
    ));

    // fx_delay_time - Float (10-2000ms)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_delay_time", 1 },
        "Delay Time",
        juce::NormalisableRange<float>(10.0f, 2000.0f, 1.0f, 0.5f),
        500.0f,
        "ms"
    ));

    // fx_delay_feedback - Float (0-95%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_delay_feedback", 1 },
        "Delay Feedback",
        juce::NormalisableRange<float>(0.0f, 95.0f, 0.1f),
        40.0f,
        "%"
    ));

    // ==================== TEMPO SYNC PARAMETERS ====================

    // fx_delay_sync - Bool (sync to tempo)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "fx_delay_sync", 1 },
        "Delay Sync",
        false
    ));

    // fx_delay_sync_rate - Choice (1/16, 1/8, 1/4, 1/2, 1, 2 bars)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "fx_delay_sync_rate", 1 },
        "Delay Sync Rate",
        juce::StringArray { "1/16", "1/8", "1/4", "1/2", "1", "2" },
        2  // Default: 1/4 note
    ));

    // fx_chorus_sync - Bool
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "fx_chorus_sync", 1 },
        "Chorus Sync",
        false
    ));

    // fx_chorus_sync_rate - Choice
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "fx_chorus_sync_rate", 1 },
        "Chorus Sync Rate",
        juce::StringArray { "1/16", "1/8", "1/4", "1/2", "1", "2", "4" },
        4  // Default: 1 bar
    ));

    // fx_phaser_sync - Bool
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "fx_phaser_sync", 1 },
        "Phaser Sync",
        false
    ));

    // fx_phaser_sync_rate - Choice
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "fx_phaser_sync_rate", 1 },
        "Phaser Sync Rate",
        juce::StringArray { "1/16", "1/8", "1/4", "1/2", "1", "2", "4" },
        4  // Default: 1 bar
    ));

    // fx_flanger_sync - Bool
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "fx_flanger_sync", 1 },
        "Flanger Sync",
        false
    ));

    // fx_flanger_sync_rate - Choice
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "fx_flanger_sync_rate", 1 },
        "Flanger Sync Rate",
        juce::StringArray { "1/16", "1/8", "1/4", "1/2", "1", "2", "4" },
        3  // Default: 1/2 bar
    ));

    // fx_reverb_size - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_reverb_size", 1 },
        "Reverb Size",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        70.0f,
        "%"
    ));

    // fx_reverb_decay - Float (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "fx_reverb_decay", 1 },
        "Reverb Decay",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    return layout;
}

//==============================================================================
CodoxAudioProcessor::CodoxAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))  // Synth: output-only bus
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
    // Initialize voice pool with unique_ptr
    voices.reserve(numVoices);
    for (int i = 0; i < numVoices; ++i)
    {
        voices.push_back(std::make_unique<Voice>());
    }
}

CodoxAudioProcessor::~CodoxAudioProcessor()
{
}

//==============================================================================
void CodoxAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare all voices
    for (auto& voice : voices)
        voice->prepareToPlay(sampleRate);

    // Phase 3.5: Prepare effects chain
    effectsChain.prepareToPlay(sampleRate, samplesPerBlock);

    // Phase 3.6: Prepare LFOs
    lfo1.prepareToPlay(sampleRate);
    lfo2.prepareToPlay(sampleRate);
    lfo3.prepareToPlay(sampleRate);
    lfo4.prepareToPlay(sampleRate);
}

void CodoxAudioProcessor::releaseResources()
{
    // Reset all voices
    for (auto& voice : voices)
        voice->reset();

    // Phase 3.5: Reset effects chain
    effectsChain.reset();

    // Phase 3.6: Reset LFOs
    lfo1.reset();
    lfo2.reset();
    lfo3.reset();
    lfo4.reset();
}

void CodoxAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output buffer
    buffer.clear();

    // Read envelope parameters (atomic, real-time safe)
    auto* ampAttackParam = parameters.getRawParameterValue("amp_attack");
    auto* ampDecayParam = parameters.getRawParameterValue("amp_decay");
    auto* ampSustainParam = parameters.getRawParameterValue("amp_sustain");
    auto* ampReleaseParam = parameters.getRawParameterValue("amp_release");

    float attack = ampAttackParam->load();
    float decay = ampDecayParam->load();
    float sustain = ampSustainParam->load() / 100.0f; // Convert 0-100% to 0.0-1.0
    float release = ampReleaseParam->load();

    // Read oscillator A parameters
    auto* oscA_wavetable = parameters.getRawParameterValue("osc_a_wavetable");
    auto* oscA_position = parameters.getRawParameterValue("osc_a_position");
    auto* oscA_level = parameters.getRawParameterValue("osc_a_level");
    auto* oscA_pan = parameters.getRawParameterValue("osc_a_pan");
    auto* oscA_octave = parameters.getRawParameterValue("osc_a_octave");
    auto* oscA_semitone = parameters.getRawParameterValue("osc_a_semitone");
    auto* oscA_fine = parameters.getRawParameterValue("osc_a_fine");
    auto* oscA_warpMode = parameters.getRawParameterValue("osc_a_warp_mode");
    auto* oscA_warpAmount = parameters.getRawParameterValue("osc_a_warp_amount");

    // Read oscillator B parameters
    auto* oscB_wavetable = parameters.getRawParameterValue("osc_b_wavetable");
    auto* oscB_position = parameters.getRawParameterValue("osc_b_position");
    auto* oscB_level = parameters.getRawParameterValue("osc_b_level");
    auto* oscB_pan = parameters.getRawParameterValue("osc_b_pan");
    auto* oscB_octave = parameters.getRawParameterValue("osc_b_octave");
    auto* oscB_semitone = parameters.getRawParameterValue("osc_b_semitone");
    auto* oscB_fine = parameters.getRawParameterValue("osc_b_fine");
    auto* oscB_warpMode = parameters.getRawParameterValue("osc_b_warp_mode");
    auto* oscB_warpAmount = parameters.getRawParameterValue("osc_b_warp_amount");

    // Read sub oscillator parameters (Phase 3.3)
    auto* sub_shape = parameters.getRawParameterValue("sub_shape");
    auto* sub_octave = parameters.getRawParameterValue("sub_octave");
    auto* sub_level = parameters.getRawParameterValue("sub_level");

    // Read noise oscillator parameters (Phase 3.3)
    auto* noise_type = parameters.getRawParameterValue("noise_type");
    auto* noise_level = parameters.getRawParameterValue("noise_level");

    // Read filter parameters (Phase 3.3)
    auto* filter_type = parameters.getRawParameterValue("filter_type");
    auto* filter_cutoff = parameters.getRawParameterValue("filter_cutoff");
    auto* filter_resonance = parameters.getRawParameterValue("filter_resonance");
    auto* filter_drive = parameters.getRawParameterValue("filter_drive");
    auto* filter_env_depth = parameters.getRawParameterValue("filter_env_depth");
    auto* filter_keytrack = parameters.getRawParameterValue("filter_keytrack");

    // Read filter envelope parameters (Phase 3.3)
    auto* filt_attack = parameters.getRawParameterValue("filt_attack");
    auto* filt_decay = parameters.getRawParameterValue("filt_decay");
    auto* filt_sustain = parameters.getRawParameterValue("filt_sustain");
    auto* filt_release = parameters.getRawParameterValue("filt_release");

    // Read unison parameters (Phase 3.4)
    auto* unison_voices = parameters.getRawParameterValue("unison_voices");
    auto* unison_detune = parameters.getRawParameterValue("unison_detune");

    // Phase 3.6: Read LFO parameters
    auto* lfo1_shape = parameters.getRawParameterValue("lfo1_shape");
    auto* lfo1_rate = parameters.getRawParameterValue("lfo1_rate");
    auto* lfo1_sync = parameters.getRawParameterValue("lfo1_sync");

    auto* lfo2_shape = parameters.getRawParameterValue("lfo2_shape");
    auto* lfo2_rate = parameters.getRawParameterValue("lfo2_rate");
    auto* lfo2_sync = parameters.getRawParameterValue("lfo2_sync");

    auto* lfo3_shape = parameters.getRawParameterValue("lfo3_shape");
    auto* lfo3_rate = parameters.getRawParameterValue("lfo3_rate");
    auto* lfo3_sync = parameters.getRawParameterValue("lfo3_sync");

    auto* lfo4_shape = parameters.getRawParameterValue("lfo4_shape");
    auto* lfo4_rate = parameters.getRawParameterValue("lfo4_rate");
    auto* lfo4_sync = parameters.getRawParameterValue("lfo4_sync");

    // Phase 3.6: Update LFOs (NOTE: v1.0 has NO routing - LFOs generate but don't modulate)
    lfo1.setShape(static_cast<int>(lfo1_shape->load()));
    lfo1.setRate(lfo1_rate->load());
    lfo1.setTempoSync(lfo1_sync->load() > 0.5f);

    lfo2.setShape(static_cast<int>(lfo2_shape->load()));
    lfo2.setRate(lfo2_rate->load());
    lfo2.setTempoSync(lfo2_sync->load() > 0.5f);

    lfo3.setShape(static_cast<int>(lfo3_shape->load()));
    lfo3.setRate(lfo3_rate->load());
    lfo3.setTempoSync(lfo3_sync->load() > 0.5f);

    lfo4.setShape(static_cast<int>(lfo4_shape->load()));
    lfo4.setRate(lfo4_rate->load());
    lfo4.setTempoSync(lfo4_sync->load() > 0.5f);

    // Phase 3.5: Read effects mix parameters
    auto* fx_distortion_mix = parameters.getRawParameterValue("fx_distortion_mix");
    auto* fx_chorus_mix = parameters.getRawParameterValue("fx_chorus_mix");
    auto* fx_phaser_mix = parameters.getRawParameterValue("fx_phaser_mix");
    auto* fx_flanger_mix = parameters.getRawParameterValue("fx_flanger_mix");
    auto* fx_delay_mix = parameters.getRawParameterValue("fx_delay_mix");
    auto* fx_reverb_mix = parameters.getRawParameterValue("fx_reverb_mix");
    auto* fx_eq_mix = parameters.getRawParameterValue("fx_eq_mix");
    auto* fx_compressor_mix = parameters.getRawParameterValue("fx_compressor_mix");

    // Phase 3.5: Read effect timing parameters
    auto* fx_delay_time = parameters.getRawParameterValue("fx_delay_time");
    auto* fx_delay_feedback = parameters.getRawParameterValue("fx_delay_feedback");
    auto* fx_reverb_size = parameters.getRawParameterValue("fx_reverb_size");
    auto* fx_reverb_decay = parameters.getRawParameterValue("fx_reverb_decay");

    // Read tempo sync parameters
    auto* fx_delay_sync = parameters.getRawParameterValue("fx_delay_sync");
    auto* fx_delay_sync_rate = parameters.getRawParameterValue("fx_delay_sync_rate");
    auto* fx_chorus_sync = parameters.getRawParameterValue("fx_chorus_sync");
    auto* fx_chorus_sync_rate = parameters.getRawParameterValue("fx_chorus_sync_rate");
    auto* fx_phaser_sync = parameters.getRawParameterValue("fx_phaser_sync");
    auto* fx_phaser_sync_rate = parameters.getRawParameterValue("fx_phaser_sync_rate");
    auto* fx_flanger_sync = parameters.getRawParameterValue("fx_flanger_sync");
    auto* fx_flanger_sync_rate = parameters.getRawParameterValue("fx_flanger_sync_rate");

    // Get BPM from host
    double bpm = 120.0; // Default fallback
    if (auto* playHead = getPlayHead())
    {
        if (auto posInfo = playHead->getPosition())
        {
            if (posInfo->getBpm().hasValue())
                bpm = *posInfo->getBpm();
        }
    }

    // Helper lambda to convert sync rate index to note multiplier
    // Index: 0=1/16, 1=1/8, 2=1/4, 3=1/2, 4=1, 5=2, 6=4
    auto getSyncMultiplier = [](int rateIndex) -> float {
        const float multipliers[] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f };
        return (rateIndex >= 0 && rateIndex < 7) ? multipliers[rateIndex] : 1.0f;
    };

    // Calculate synced time in ms: (60000 / bpm) * multiplier
    auto calcSyncedTimeMs = [&](int rateIndex) -> float {
        float quarterNoteMs = 60000.0f / static_cast<float>(bpm);
        return quarterNoteMs * getSyncMultiplier(rateIndex);
    };

    // Calculate synced rate in Hz: bpm / (60 * multiplier)
    auto calcSyncedRateHz = [&](int rateIndex) -> float {
        float multiplier = getSyncMultiplier(rateIndex);
        return static_cast<float>(bpm) / (60.0f * multiplier);
    };

    // Phase 3.5: Update effects chain mix parameters
    effectsChain.setDistortionMix(fx_distortion_mix->load() / 100.0f); // Convert 0-100% to 0.0-1.0
    effectsChain.setChorusMix(fx_chorus_mix->load() / 100.0f);
    effectsChain.setPhaserMix(fx_phaser_mix->load() / 100.0f);
    effectsChain.setFlangerMix(fx_flanger_mix->load() / 100.0f);
    effectsChain.setDelayMix(fx_delay_mix->load() / 100.0f);
    effectsChain.setReverbMix(fx_reverb_mix->load() / 100.0f);
    effectsChain.setEQMix(fx_eq_mix->load() / 100.0f);
    effectsChain.setCompressorMix(fx_compressor_mix->load() / 100.0f);

    // Update delay time (with optional tempo sync)
    if (fx_delay_sync->load() > 0.5f)
    {
        float syncedTimeMs = calcSyncedTimeMs(static_cast<int>(fx_delay_sync_rate->load()));
        effectsChain.setDelayTime(juce::jlimit(10.0f, 2000.0f, syncedTimeMs));
    }
    else
    {
        effectsChain.setDelayTime(fx_delay_time->load());
    }
    effectsChain.setDelayFeedback(fx_delay_feedback->load() / 100.0f);

    // Update chorus rate (with optional tempo sync)
    if (fx_chorus_sync->load() > 0.5f)
    {
        float syncedRateHz = calcSyncedRateHz(static_cast<int>(fx_chorus_sync_rate->load()));
        effectsChain.setChorusRate(juce::jlimit(0.01f, 20.0f, syncedRateHz));
    }

    // Update phaser rate (with optional tempo sync)
    if (fx_phaser_sync->load() > 0.5f)
    {
        float syncedRateHz = calcSyncedRateHz(static_cast<int>(fx_phaser_sync_rate->load()));
        effectsChain.setPhaserRate(juce::jlimit(0.01f, 20.0f, syncedRateHz));
    }

    // Update flanger rate (with optional tempo sync)
    if (fx_flanger_sync->load() > 0.5f)
    {
        float syncedRateHz = calcSyncedRateHz(static_cast<int>(fx_flanger_sync_rate->load()));
        effectsChain.setFlangerRate(juce::jlimit(0.01f, 20.0f, syncedRateHz));
    }

    // Update reverb parameters
    effectsChain.setReverbSize(fx_reverb_size->load() / 100.0f);
    effectsChain.setReverbDamping(1.0f - (fx_reverb_decay->load() / 100.0f)); // Invert: higher decay = less damping

    // Update all voices with current parameters
    for (auto& voice : voices)
    {
        // Update envelope
        voice->updateEnvelope(attack, decay, sustain, release);

        // Update oscillator A
        voice->updateOscillatorA(
            static_cast<int>(oscA_wavetable->load()),
            oscA_position->load(),
            oscA_level->load(),
            oscA_pan->load(),
            static_cast<int>(oscA_octave->load()),
            static_cast<int>(oscA_semitone->load()),
            static_cast<int>(oscA_fine->load()),
            static_cast<int>(oscA_warpMode->load()),
            oscA_warpAmount->load()
        );

        // Update oscillator B
        voice->updateOscillatorB(
            static_cast<int>(oscB_wavetable->load()),
            oscB_position->load(),
            oscB_level->load(),
            oscB_pan->load(),
            static_cast<int>(oscB_octave->load()),
            static_cast<int>(oscB_semitone->load()),
            static_cast<int>(oscB_fine->load()),
            static_cast<int>(oscB_warpMode->load()),
            oscB_warpAmount->load()
        );

        // Update sub oscillator (Phase 3.3)
        voice->updateSubOscillator(
            static_cast<int>(sub_shape->load()),
            static_cast<int>(sub_octave->load()),
            sub_level->load()
        );

        // Update noise oscillator (Phase 3.3)
        voice->updateNoiseOscillator(
            static_cast<int>(noise_type->load()),
            noise_level->load()
        );

        // Update filter (Phase 3.3)
        voice->updateFilter(
            static_cast<int>(filter_type->load()),
            filter_cutoff->load(),
            filter_resonance->load(),
            filter_drive->load(),
            filter_env_depth->load(),
            filter_keytrack->load()
        );

        // Update filter envelope (Phase 3.3)
        voice->updateFilterEnvelope(
            filt_attack->load(),
            filt_decay->load(),
            filt_sustain->load() / 100.0f, // Convert 0-100% to 0.0-1.0
            filt_release->load()
        );

        // Update unison parameters (Phase 3.4)
        voice->updateUnisonParameters(
            static_cast<int>(unison_voices->load()),  // 0-4 index → 1,2,4,8,16 voices
            unison_detune->load(),                    // 0-100%
            50.0f                                     // Fixed stereo spread 50% (not in parameter spec)
        );
    }

    // Phase 3.6: Read glide time parameter
    auto* glide_time = parameters.getRawParameterValue("glide_time");
    float glideTimeSeconds = glide_time->load(); // 0-10 seconds

    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int note = message.getNoteNumber();
            float velocity = message.getVelocity() / 127.0f;
            allocateVoice(note, velocity, getSampleRate(), glideTimeSeconds);
        }
        else if (message.isNoteOff())
        {
            int note = message.getNoteNumber();
            releaseVoice(note);
        }
    }

    // Generate audio from all active voices (Phase 3.2d - stereo with constant-power panning)
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Read master volume parameter
    auto* masterVolumeParam = parameters.getRawParameterValue("master_volume");
    float masterVolumeDB = masterVolumeParam->load();
    float masterVolumeLinear = juce::Decibels::decibelsToGain(masterVolumeDB);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float leftMix = 0.0f;
        float rightMix = 0.0f;

        // Sum all active voices (stereo)
        for (auto& voice : voices)
        {
            if (voice->isPlaying())
            {
                float voiceLeft, voiceRight;
                voice->getNextSampleStereo(voiceLeft, voiceRight);
                leftMix += voiceLeft;
                rightMix += voiceRight;
            }
        }

        // Phase 3.5: Process through effects chain (AFTER voice summation, BEFORE master volume)
        effectsChain.processStereo(leftMix, rightMix);

        // Apply master volume
        leftMix *= masterVolumeLinear;
        rightMix *= masterVolumeLinear;

        // Write to output channels
        if (numChannels >= 1)
            buffer.setSample(0, sample, leftMix);
        if (numChannels >= 2)
            buffer.setSample(1, sample, rightMix);

        // If more than 2 channels, copy left to extra channels
        for (int channel = 2; channel < numChannels; ++channel)
            buffer.setSample(channel, sample, leftMix);

        // Phase 3.6: Generate LFO samples (NOTE: v1.0 has NO routing - LFOs run but don't modulate)
        // This advances LFO phase for each sample, keeping LFOs in sync with audio rate
        lfo1.getNextSample();
        lfo2.getNextSample();
        lfo3.getNextSample();
        lfo4.getNextSample();
    }
}

// Voice allocation: Round-robin with voice stealing (Phase 3.6: Added glide support)
void CodoxAudioProcessor::allocateVoice(int midiNote, float velocity, double sr, float glideTimeParam)
{
    // First, try to find a free voice
    for (auto& voice : voices)
    {
        if (!voice->isPlaying())
        {
            voice->noteOn(midiNote, velocity, sr, glideTimeParam);
            return;
        }
    }

    // No free voices - steal oldest voice (round-robin)
    voices[static_cast<size_t>(nextVoiceIndex)]->noteOn(midiNote, velocity, sr, glideTimeParam);
    nextVoiceIndex = (nextVoiceIndex + 1) % numVoices;
}

// Voice release: Find all voices playing this MIDI note and release them
void CodoxAudioProcessor::releaseVoice(int midiNote)
{
    for (auto& voice : voices)
    {
        if (voice->isPlaying() && voice->getMidiNote() == midiNote)
            voice->noteOff();
    }
}

juce::AudioProcessorEditor* CodoxAudioProcessor::createEditor()
{
    return new CodoxAudioProcessorEditor(*this);
}

void CodoxAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CodoxAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CodoxAudioProcessor();
}
