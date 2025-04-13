// Created by Cédric Niyikiza
//

#pragma once

#include <juce_core/juce_core.h>

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
    
} 