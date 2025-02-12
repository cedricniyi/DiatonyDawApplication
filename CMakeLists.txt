cmake_minimum_required(VERSION 3.22)

project(DiatonyDawApplication VERSION 0.0.1)

add_subdirectory("${CMAKE_SOURCE_DIR}/modules/JUCE" JUCE)

juce_add_plugin(DiatonyDawApplication
    COMPANY_NAME "cedricniyikza"                # Specify the name of the plugin's author
    NEEDS_MIDI_INPUT TRUE                       # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT TRUE                      # Does the plugin need midi output?
    PLUGIN_MANUFACTURER_CODE Juce               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Dda1                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "DiatonyDawApplication")       # The name of the final executable, which can differ from the target name

juce_generate_juce_header(DiatonyDawApplication)

target_sources(DiatonyDawApplication
    PRIVATE
        src/PluginEditor.cpp
        src/PluginProcessor.cpp
)

target_link_directories(DiatonyDawApplication PRIVATE "${CMAKE_SOURCE_DIR}/src")

target_compile_definitions(DiatonyDawApplication
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(DiatonyDawApplication
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_config_flags)


# --- Set the Gecode paths ---
set(GECODE_ROOT "/opt/homebrew/opt/gecode")
set(GECODE_INCLUDE_DIR "${GECODE_ROOT}/include")
set(GECODE_LIBRARY_DIR "${GECODE_ROOT}/lib")

# --- Tell CMake where to find Gecode headers ---
include_directories(${GECODE_INCLUDE_DIR})

find_library(GECODE_KERNEL_LIB      NAMES gecodekernel      PATHS ${GECODE_LIBRARY_DIR} REQUIRED)
find_library(GECODE_DRIVER_LIB      NAMES gecodedriver      PATHS ${GECODE_LIBRARY_DIR} REQUIRED)
find_library(GECODE_FLATZINC_LIB    NAMES gecodeflatzinc    PATHS ${GECODE_LIBRARY_DIR} REQUIRED)
find_library(GECODE_FLOAT_LIB       NAMES gecodefloat       PATHS ${GECODE_LIBRARY_DIR} REQUIRED)
find_library(GECODE_INT_LIB         NAMES gecodeint         PATHS ${GECODE_LIBRARY_DIR} REQUIRED)
find_library(GECODE_MINIMODEL_LIB   NAMES gecodeminimodel   PATHS ${GECODE_LIBRARY_DIR} REQUIRED)
find_library(GECODE_SEARCH_LIB      NAMES gecodesearch      PATHS ${GECODE_LIBRARY_DIR} REQUIRED)
find_library(GECODE_SET_LIB         NAMES gecodeset         PATHS ${GECODE_LIBRARY_DIR} REQUIRED)
find_library(GECODE_SUPPORT_LIB     NAMES gecodesupport     PATHS ${GECODE_LIBRARY_DIR} REQUIRED)

target_link_libraries(DiatonyDawApplication
        PRIVATE
        ${GECODE_KERNEL_LIB}
        ${GECODE_DRIVER_LIB}
        ${GECODE_FLATZINC_LIB}
        ${GECODE_FLOAT_LIB}
        ${GECODE_INT_LIB}
        ${GECODE_MINIMODEL_LIB}
        ${GECODE_SEARCH_LIB}
        ${GECODE_SET_LIB}
        ${GECODE_SUPPORT_LIB}
)
