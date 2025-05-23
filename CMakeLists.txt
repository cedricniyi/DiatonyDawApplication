cmake_minimum_required(VERSION 3.22)

include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Cette option désactive la compilation des tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

# Rend la bibliothèque disponible
FetchContent_MakeAvailable(json)

project(DiatonyDawApplication VERSION 0.0.1)

add_compile_definitions(APPLICATION_SUPPORT_PATH="Library/Application Support")

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address")
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/modules/JUCE" JUCE)

juce_add_plugin(DiatonyDawApplication
    COMPANY_NAME "64492300_CN_UCL"                # Specify the name of the plugin's author
    NEEDS_MIDI_INPUT TRUE                       # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT TRUE                      # Does the plugin need midi output?
    PLUGIN_MANUFACTURER_CODE Juce               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Dda1                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "DiatonyDawApplication")       # The name of the final executable, which can differ from the target name

get_target_property(TARGET_TYPE DiatonyDawApplication TYPE)
message(STATUS "DiatonyDawApplication type: ${TARGET_TYPE}")

juce_generate_juce_header(DiatonyDawApplication)

# Ajout de tous les fichiers sources du projet
target_sources(DiatonyDawApplication
    PRIVATE
        # Core
        src/core/PluginProcessor.cpp
        
        # UI
        src/ui/PluginEditor.cpp
        
        # Fichiers header only (à inclure pour être sûr qu'ils sont pris en compte)
        src/model/Progression.h
        src/model/ChordSequence.h
        src/audio/SimpleSound.h
        src/audio/SimpleVoice.h
        src/ui/components/TonalityPanel.h
        src/ui/components/ProgressionPanel.h
        src/ui/components/GenerationPanel.h
        src/ui/LookAndFeel/DiatonyLookAndFeel.h
        src/utils/DiatonyConstants.h
        src/utils/Midiprocessor.h
        src/utils/FileUtils.h
        src/utils/DatabaseManagement.h
)

target_include_directories(DiatonyDawApplication PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(DiatonyDawApplication
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_STRINGS_ARE_UNICODE=1
        JUCE_USE_UTF8_FILESYSTEM=1
        JucePlugin_Name="DiatonyDawApplication")

target_link_libraries(DiatonyDawApplication
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
        nlohmann_json::nlohmann_json
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

# Déclare une commande personnalisée pour générer diatony.dylib.
# OUTPUT : indique le chemin du fichier généré.
# COMMAND : exécute le makefile externe pour compiler la bibliothèque dynamique.
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/Diatony/out/diatony.dylib
    COMMAND make -C ${CMAKE_SOURCE_DIR}/Diatony/c++/ compile dylib
    COMMENT "Compile Diatony Solver via Makefile"
)

# Crée une cible custom 'build_external_make' qui dépend du fichier diatony.dylib.
# Cela permet à CMake de connaître le fichier externe généré.
add_custom_target(build_external_make ALL
    DEPENDS ${CMAKE_SOURCE_DIR}/Diatony/out/diatony.dylib
)

# Assure que la cible principale du plug-in dépend de la construction de la bibliothèque externe.
# Le plug-in ne sera compilé qu'après la génération de diatony.dylib.
add_dependencies(DiatonyDawApplication build_external_make)

# Lie la bibliothèque dynamique générée au plug-in pour résoudre les symboles manquants.
target_link_libraries(DiatonyDawApplication
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Diatony/out/diatony.dylib
)

# Définit le rpath du binaire final.
# Cela indique au chargeur dynamique de rechercher les bibliothèques dans le dossier '../Frameworks'
# relatif au chemin de l'exécutable lors de l'exécution.
# Votre configuration RPATH existante
set_target_properties(DiatonyDawApplication_Standalone PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@executable_path/../Frameworks"
)

# Copie la bibliothèque dynamique diatony.dylib dans le bundle de l'application.
# Cette étape est nécessaire pour que l'application puisse trouver et charger la bibliothèque à l'exécution.
# La bibliothèque est copiée dans le dossier Frameworks du bundle.
add_custom_command(TARGET DiatonyDawApplication_Standalone POST_BUILD
    # Debug: Afficher le chemin de l'exécutable
    COMMAND ${CMAKE_COMMAND} -E echo "TARGET_FILE_DIR: $<TARGET_FILE_DIR:DiatonyDawApplication_Standalone>"
    
    # Créer le dossier Frameworks dans le bundle
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:DiatonyDawApplication_Standalone>/../Frameworks"
    
    # # Debug: Vérifier si le dossier a été créé
    # COMMAND ${CMAKE_COMMAND} -E echo "Checking Frameworks directory..."
    # COMMAND ls -la "$<TARGET_FILE_DIR:DiatonyDawApplication_Standalone>/.."
    
    # Copier la dylib
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/Diatony/out/diatony.dylib"
        "$<TARGET_FILE_DIR:DiatonyDawApplication_Standalone>/../Frameworks/diatony.dylib"
        
    # # Debug: Vérifier la copie
    # COMMAND ${CMAKE_COMMAND} -E echo "Checking dylib..."
    # COMMAND ls -la "$<TARGET_FILE_DIR:DiatonyDawApplication_Standalone>/../Frameworks"
)

# # Vérifier que le dossier source existe
# add_custom_command(TARGET DiatonyDawApplication PRE_BUILD
#     COMMAND ${CMAKE_COMMAND} -E echo "Checking source dylib..."
#     COMMAND ls -la "${CMAKE_SOURCE_DIR}/Diatony/out/"
# )

# Message supplémentaire pour debug
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

# Ajout du nouveau bloc ici
# Création d'un lien symbolique entre le dossier Solutions de l'application 
# (dans ~/Library/Application Support) et un dossier Solutions à la racine du projet
# pour faciliter l'accès aux solutions générées par le solveur
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SOLUTIONS_DIR "$ENV{HOME}/Library/Application Support/DiatonyDawApplication/Solutions")
    set(SOLUTIONS_LINK "${CMAKE_SOURCE_DIR}/Solutions")
    
    # Création des dossiers et du lien symbolique
    add_custom_command(TARGET DiatonyDawApplication POST_BUILD
        # Créer le dossier dans Library si nécessaire
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SOLUTIONS_DIR}"
        
        # Supprimer le lien symbolique s'il existe (pour éviter les erreurs)
        COMMAND ${CMAKE_COMMAND} -E remove -f "${SOLUTIONS_LINK}"
        
        # Créer le nouveau lien symbolique
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${SOLUTIONS_DIR}"
            "${SOLUTIONS_LINK}"
            
        COMMENT "Setting up Solutions directory symlink for development"
    )
    
    # Message de confirmation
    message(STATUS "Development setup: Solutions directory symlink will be created at ${SOLUTIONS_LINK}")
endif()