#pragma once

#include <juce_core/juce_core.h>

#ifndef APPLICATION_SUPPORT_PATH
    #error "APPLICATION_SUPPORT_PATH n'est pas défini"
#endif

namespace FileUtils {
    
    /** @brief Génère un nom de fichier unique avec timestamp (format YYYYMMDD_HHMMSS). */
    inline juce::String generateUniqueFilename(const juce::String& basePath, const juce::String& extension = ".mid") {
        juce::Time now = juce::Time::getCurrentTime();
        juce::String timestamp = now.formatted("%Y%m%d_%H%M%S");
        return basePath + "_" + timestamp + extension;
    }
    
    /** @brief Retourne le dossier des solutions MIDI (le crée si inexistant). */
    inline juce::File getMidiSolutionsFolder() {
        juce::File midiFolder = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
            .getChildFile(APPLICATION_SUPPORT_PATH)
            .getChildFile("DiatonyDawApplication")
            .getChildFile("Solutions")
            .getChildFile("MidiFiles");
        
        if (!midiFolder.exists())
            midiFolder.createDirectory();
        
        return midiFolder;
    }
    
    /** @brief Ouvre le dossier des solutions MIDI dans l'explorateur natif. */
    inline void openMidiSolutionsFolder() {
        getMidiSolutionsFolder().startAsProcess();
    }
    
}
