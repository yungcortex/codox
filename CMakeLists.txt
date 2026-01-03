cmake_minimum_required(VERSION 3.15)
project(Codox VERSION 2.1.0)

# JUCE is already added by root CMakeLists.txt - do not add again

# Plugin formats: VST3, AU, Standalone
juce_add_plugin(Codox
    COMPANY_NAME "Cody Littlefield"
    PLUGIN_MANUFACTURER_CODE Cdlt
    PLUGIN_CODE Codx
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Codox"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_WEB_BROWSER TRUE
)

# Source files
target_sources(Codox
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Voice.h
)

# Include paths
target_include_directories(Codox
    PRIVATE
        Source
)

# Required JUCE modules
target_link_libraries(Codox
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Generate JuceHeader.h (JUCE 8 requirement - MUST come after target_link_libraries)
juce_generate_juce_header(Codox)

# WebView UI Resources
juce_add_binary_data(Codox_UIResources
    SOURCES
        Source/ui/public/index.html
        Source/ui/public/js/juce/index.js
        Source/ui/public/js/juce/check_native_interop.js
)

target_link_libraries(Codox
    PRIVATE
        Codox_UIResources
)

# Compile definitions
target_compile_definitions(Codox
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
)
