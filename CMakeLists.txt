cmake_minimum_required(VERSION 3.22)

include(FetchContent)

# Dépendance JSON 
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Dépendance Melatonin Inspector, pour inspecter l'interface de l'application
FetchContent_Declare(
    melatonin_inspector
    GIT_REPOSITORY https://github.com/sudara/melatonin_inspector.git
    GIT_TAG        origin/main      # ou une release taguée si tu préfères
    SOURCE_DIR     ${CMAKE_CURRENT_BINARY_DIR}/melatonin_inspector
)

# Dépendance Melatonin Blur, pour les effets de flou et shadows
FetchContent_Declare(
    melatonin_blur
    GIT_REPOSITORY https://github.com/sudara/melatonin_blur.git
    GIT_TAG        origin/main
    SOURCE_DIR     ${CMAKE_CURRENT_BINARY_DIR}/melatonin_blur
)

# Cette option désactive la compilation des tests
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

FetchContent_MakeAvailable(melatonin_inspector)
FetchContent_MakeAvailable(melatonin_blur)  

juce_add_plugin(DiatonyDawApplication
    COMPANY_NAME "64492300_CN_UCL"              # Specify the name of the plugin's author
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
        src/controller/core/PluginProcessor.cpp
        src/controller/core/PluginProcessor.h
        
        # App Controller
        src/controller/AppController.cpp
        src/controller/AppController.h
        
        # UI
        src/ui/PluginEditor.cpp
        src/ui/PluginEditor.h
        src/ui/UIStateIdentifiers.h
        src/ui/RootAnimator.h
        src/ui/MainContentComponent.h
        src/ui/extra/ColoredPanel.h
        src/ui/extra/StyledButton.h
        src/ui/extra/OutlineTextButton.h
        src/ui/extra/SimpleToastComponent.h
        src/ui/extra/DiatonyAlertWindow.h
        src/ui/footer/FooterPanel.h
        src/ui/header/HeaderPanel.h
        src/ui/section/SectionPanel.h
        src/ui/section/SectionPanel.cpp
        src/ui/style/DiatonyLookAndFeel.h
        
        # Animation
        src/ui/animation/AnimationManager.cpp
        src/ui/animation/AnimationManager.h
        
        # Fichiers header only (à inclure pour être sûr qu'ils sont pris en compte)
        src/audio/SimpleSound.h
        src/audio/SimpleVoice.h

        src/utils/DiatonyConstants.h
        src/utils/Midiprocessor.h
        src/utils/FileUtils.h
        src/utils/DatabaseManagement.h
        
        # Font Manager
        src/utils/FontManager.cpp
        src/utils/FontManager.h
        
        # Color Generator
        src/utils/ColorGenerator.cpp
        src/utils/ColorGenerator.h
        
        # UI Components - EditZone
        src/ui/section/components/EditZone/ProgressionArea.h
        src/ui/section/components/EditZone/ProgressionArea.cpp
        src/ui/section/components/EditZone/WelcomeView.h
        src/ui/section/components/EditZone/WelcomeView.cpp
        
        # UI Components - OverviewZone/PlayBack
        src/ui/section/components/OverviewZone/PlayBack/PlaybackActionArea.h
        src/ui/section/components/OverviewZone/PlayBack/PlaybackActionArea.cpp
        
        # UI Components - OverviewZone/GenerateAndMore
        src/ui/section/components/OverviewZone/GenerateAndMore/GenerationButtons.h
        src/ui/section/components/OverviewZone/GenerateAndMore/GenerationButtons.cpp
        
        # UI Components - OverviewZone/Overview
        src/ui/section/components/OverviewZone/Overview/ScrollableContentPanel.h
        src/ui/section/components/OverviewZone/Overview/ScrollableContentPanel.cpp
        src/ui/section/components/OverviewZone/Overview/OverviewContentArea.h
        src/ui/section/components/OverviewZone/Overview/OverviewContentArea.cpp
        src/ui/section/components/OverviewZone/Overview/OverviewArea.h
        src/ui/section/components/OverviewZone/Overview/OverviewArea.cpp
        src/ui/section/components/OverviewZone/Overview/OverviewActionArea.h
        src/ui/section/components/OverviewZone/Overview/OverviewActionArea.cpp

        # Model
        src/model/Identifiers.h
        src/model/Progression.h
        src/model/Progression.cpp
        src/model/Chord.h
        src/model/Chord.cpp
        src/model/Modulation.h
        src/model/Modulation.cpp
        src/model/Piece.h
        src/model/Piece.cpp
        src/model/Section.h
        src/model/Section.cpp
        src/model/DiatonyTypes.h

        # Services
        src/services/GenerationService.h
        src/services/GenerationService.cpp
)

# === Configuration des Fonts SF Pro ===
# Vérification de la présence des fonts essentielles
set(FONTS_DIR "${CMAKE_SOURCE_DIR}/src/utils/Fonts")
set(ESSENTIAL_FONTS
    "${FONTS_DIR}/SF-Pro-Display-Regular.otf"
    "${FONTS_DIR}/SF-Pro-Display-Bold.otf"
    "${FONTS_DIR}/SF-Pro-Display-Semibold.otf"
    "${FONTS_DIR}/SF-Pro-Text-Regular.otf"
    "${FONTS_DIR}/SF-Pro-Text-Medium.otf"
)

# Rechercher toutes les fonts SF Pro disponibles
file(GLOB SF_PRO_FONTS "${FONTS_DIR}/SF-Pro-*.otf")

if(SF_PRO_FONTS)
    # Compter le nombre de fonts trouvées
    list(LENGTH SF_PRO_FONTS FONT_COUNT)
    message(STATUS "Found ${FONT_COUNT} SF Pro fonts")
    
    # Afficher la liste des fonts trouvées pour debug
    foreach(FONT ${SF_PRO_FONTS})
        get_filename_component(FONT_NAME ${FONT} NAME)
        message(STATUS "  - ${FONT_NAME}")
    endforeach()
    
    # Créer les ressources binaires pour les fonts
    juce_add_binary_data(FontResources
        SOURCES ${SF_PRO_FONTS}
        HEADER_NAME "BinaryData.h"
        NAMESPACE "BinaryData"
    )
    
    message(STATUS "SF Pro fonts successfully added to binary resources")
else()
    message(WARNING "No SF Pro fonts found in ${FONTS_DIR}")
    message(WARNING "Please place your SF Pro .otf files in src/utils/Fonts/")
endif()

target_include_directories(DiatonyDawApplication PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}  # Pour accéder à BinaryData.h généré
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
        melatonin_inspector
        melatonin_blur
        juce::juce_audio_utils
        juce::juce_animation
        juce::juce_data_structures # Ajouté pour support du ValueTree
        nlohmann_json::nlohmann_json
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_config_flags)

# Link font resources if they were created
if(TARGET FontResources)
    target_link_libraries(DiatonyDawApplication PRIVATE FontResources)
    message(STATUS "FontResources target linked to DiatonyDawApplication")
endif()


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