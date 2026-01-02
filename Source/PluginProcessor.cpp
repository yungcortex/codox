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

    return layout;
}

//==============================================================================
CodoxAudioProcessor::CodoxAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))  // Synth: output-only bus
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

CodoxAudioProcessor::~CodoxAudioProcessor()
{
}

//==============================================================================
void CodoxAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    // Prepare all voices
    for (auto& voice : voices)
        voice.prepareToPlay(sampleRate);
}

void CodoxAudioProcessor::releaseResources()
{
    // Reset all voices
    for (auto& voice : voices)
        voice.reset();
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

    // Update all voice envelopes
    for (auto& voice : voices)
        voice.updateEnvelope(attack, decay, sustain, release);

    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int note = message.getNoteNumber();
            float velocity = message.getVelocity() / 127.0f;
            allocateVoice(note, velocity, getSampleRate());
        }
        else if (message.isNoteOff())
        {
            int note = message.getNoteNumber();
            releaseVoice(note);
        }
    }

    // Generate audio from all active voices
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float mixedSample = 0.0f;

        // Sum all active voices
        for (auto& voice : voices)
        {
            if (voice.isPlaying())
                mixedSample += voice.getNextSample();
        }

        // Apply master volume
        auto* masterVolumeParam = parameters.getRawParameterValue("master_volume");
        float masterVolumeDB = masterVolumeParam->load();
        float masterVolumeLinear = juce::Decibels::decibelsToGain(masterVolumeDB);
        mixedSample *= masterVolumeLinear;

        // Write to all output channels (stereo)
        for (int channel = 0; channel < numChannels; ++channel)
            buffer.setSample(channel, sample, mixedSample);
    }
}

// Voice allocation: Round-robin with voice stealing
void CodoxAudioProcessor::allocateVoice(int midiNote, float velocity, double sampleRate)
{
    // First, try to find a free voice
    for (auto& voice : voices)
    {
        if (!voice.isPlaying())
        {
            voice.noteOn(midiNote, velocity, sampleRate);
            return;
        }
    }

    // No free voices - steal oldest voice (round-robin)
    voices[nextVoiceIndex].noteOn(midiNote, velocity, sampleRate);
    nextVoiceIndex = (nextVoiceIndex + 1) % numVoices;
}

// Voice release: Find all voices playing this MIDI note and release them
void CodoxAudioProcessor::releaseVoice(int midiNote)
{
    for (auto& voice : voices)
    {
        if (voice.isPlaying() && voice.getMidiNote() == midiNote)
            voice.noteOff();
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
