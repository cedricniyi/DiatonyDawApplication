// Created by Cédric Niyikiza
//

#pragma once

#include <juce_core/juce_core.h>

#ifndef APPLICATION_SUPPORT_PATH
    #error "APPLICATION_SUPPORT_PATH n'est pas défini"
#endif

namespace FileUtils {
    
    /**
     * Génère un nom de fichier unique basé sur un timestamp (date et heure actuelles).
     * @param basePath Chemin de base du fichier sans extension
     * @param extension Extension du fichier (avec le point, ex: ".mid")
     * @return Chemin complet avec timestamp (format YYYYMMDD_HHMMSS)
     */
    inline juce::String generateUniqueFilename(const juce::String& basePath, const juce::String& extension = ".mid") {
        juce::Time now = juce::Time::getCurrentTime();
        juce::String timestamp = now.formatted("%Y%m%d_%H%M%S");
        return basePath + "_" + timestamp + extension;
    }
    
    /**
     * Obtient le dossier contenant les fichiers MIDI des solutions générées.
     * Crée le dossier s'il n'existe pas.
     * @return Le dossier des solutions MIDI
     */
    inline juce::File getMidiSolutionsFolder() {
        juce::File midiFolder = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
            .getChildFile(APPLICATION_SUPPORT_PATH)
            .getChildFile("DiatonyDawApplication")
            .getChildFile("Solutions")
            .getChildFile("MidiFiles");
        
        // Créer le dossier s'il n'existe pas
        if (!midiFolder.exists())
            midiFolder.createDirectory();
        
        return midiFolder;
    }
    
    /**
     * Ouvre le dossier des solutions MIDI dans l'explorateur de fichiers natif.
     * Sur macOS : Finder, Windows : Explorer, Linux : gestionnaire de fichiers par défaut.
     */
    inline void openMidiSolutionsFolder() {
        juce::File midiFolder = getMidiSolutionsFolder();
        
        // startAsProcess() ouvre directement le contenu du dossier
        // contrairement à revealToUser() qui ouvre le parent et sélectionne le dossier
        midiFolder.startAsProcess();
    }
    
} 