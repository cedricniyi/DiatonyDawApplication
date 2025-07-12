#include "FontManager.h"
#include "DiatonyConstants.h"
#include "BinaryData.h"
#include <algorithm>

void FontManager::initializeFonts() {
    // Pré-charger les fonts les plus utilisées pour améliorer les performances
    getSFProDisplay(16.0f, FontWeight::Regular);
    getSFProDisplay(16.0f, FontWeight::Bold);
    getSFProDisplay(16.0f, FontWeight::Semibold);
    getSFProText(14.0f, FontWeight::Regular);
    getSFProText(14.0f, FontWeight::Medium);
}

juce::FontOptions FontManager::getSFProDisplay(float size, FontWeight weight, FontStyle style) {
    return getFont("SF-Pro-Display", size, weight, style);
}

juce::FontOptions FontManager::getSFProText(float size, FontWeight weight, FontStyle style) {
    return getFont("SF-Pro-Text", size, weight, style);
}

juce::FontOptions FontManager::getSFProRounded(float size, FontWeight weight, FontStyle style) {
    return getFont("SF-Pro-Rounded", size, weight, style);
}

juce::FontOptions FontManager::getFont(const std::string& family, float size, FontWeight weight, FontStyle style) {
    std::string fontKey = getFontFileName(family, weight, style);
    
    // Vérifier si la font est déjà en cache
    if (typefaceCache.find(fontKey) == typefaceCache.end()) {
        // Charger la font depuis BinaryData
        juce::Typeface::Ptr typeface = nullptr;
        
        DBG("Trying to load font: " + fontKey);
        
        // Créer le nom de ressource pour BinaryData (remplacer les tirets par des underscores)
        std::string resourceName = fontKey;
        std::replace(resourceName.begin(), resourceName.end(), '-', '_');
        resourceName += "_otf";
        
        // Essayer de charger depuis BinaryData avec l'API JUCE standard
        int resourceSize = 0;
        const char* resourceData = nullptr;
        
        // JUCE génère des constantes pour chaque ressource. Nous devons les mapper manuellement
        // ou utiliser une approche de fallback pour l'instant
        
        // Mapping des fonts les plus courantes
        if (fontKey == "SF-Pro-Display-Regular") {
            resourceData = BinaryData::SFProDisplayRegular_otf;
            resourceSize = BinaryData::SFProDisplayRegular_otfSize;
        }
        else if (fontKey == "SF-Pro-Display-Bold") {
            resourceData = BinaryData::SFProDisplayBold_otf;
            resourceSize = BinaryData::SFProDisplayBold_otfSize;
        }
        else if (fontKey == "SF-Pro-Display-Semibold") {
            resourceData = BinaryData::SFProDisplaySemibold_otf;
            resourceSize = BinaryData::SFProDisplaySemibold_otfSize;
        }
        else if (fontKey == "SF-Pro-Text-Regular") {
            resourceData = BinaryData::SFProTextRegular_otf;
            resourceSize = BinaryData::SFProTextRegular_otfSize;
        }
        else if (fontKey == "SF-Pro-Text-Medium") {
            resourceData = BinaryData::SFProTextMedium_otf;
            resourceSize = BinaryData::SFProTextMedium_otfSize;
        }
        else if (fontKey == "SF-Pro-Rounded-Bold") {
            resourceData = BinaryData::SFProRoundedBold_otf;
            resourceSize = BinaryData::SFProRoundedBold_otfSize;
        }
        else if (fontKey == "SF-Pro-Rounded-Regular") {
            resourceData = BinaryData::SFProRoundedRegular_otf;
            resourceSize = BinaryData::SFProRoundedRegular_otfSize;
        }
        // Ajouter d'autres mappings au besoin...
        
        if (resourceData != nullptr && resourceSize > 0) {
            typeface = createTypefaceFromBinaryData(resourceData, resourceSize);
            if (typeface != nullptr) {
                DBG("Successfully loaded SF Pro font: " + fontKey);
            }
        }
        
        // Fallback vers la font par défaut si la ressource n'est pas trouvée ou si le chargement a échoué
        if (typeface == nullptr) {
            DBG("Font resource not found or loading failed, using default font for: " + fontKey);
            
            // Utiliser une font par défaut simple
            juce::Font defaultFont("Arial", size, juce::Font::plain);
            typeface = defaultFont.getTypefacePtr();
            
            // Si même Arial échoue, utiliser la font par défaut de JUCE
            if (typeface == nullptr) {
                DBG("Arial fallback failed, using JUCE default font for: " + fontKey);
                juce::Font juceDefaultFont;
                typeface = juceDefaultFont.getTypefacePtr();
            }
        }
        
        typefaceCache[fontKey] = typeface;
    }
    
    // Vérifier que la typeface mise en cache est valide
    auto cachedTypeface = typefaceCache[fontKey];
    if (cachedTypeface == nullptr) {
        DBG("Cached typeface is null for: " + fontKey + ", creating emergency fallback");
        
        // Créer une font d'urgence simple
        juce::Font emergencyFont("Arial", size, juce::Font::plain);
        auto emergencyTypeface = emergencyFont.getTypefacePtr();
        
        // Vérification finale : si même la font d'urgence échoue, retourner une FontOptions basique
        if (emergencyTypeface == nullptr) {
            DBG("Emergency font failed, returning basic FontOptions for: " + fontKey);
            return juce::FontOptions().withHeight(size);
        }
        
        // Utiliser le constructeur direct au lieu de withTypeface() pour éviter l'assertion JUCE 8
        return juce::FontOptions(emergencyTypeface).withHeight(size);
    }
    
    // Utiliser le constructeur direct au lieu de withTypeface() pour éviter l'assertion JUCE 8
    return juce::FontOptions(cachedTypeface).withHeight(size);
}

juce::Typeface::Ptr FontManager::createTypefaceFromBinaryData(const char* data, size_t dataSize) {
    return juce::Typeface::createSystemTypefaceFor(data, dataSize);
}

std::string FontManager::getFontFileName(const std::string& family, FontWeight weight, FontStyle style) {
    std::string result = family;
    
    // Ajouter le poids
    switch (weight) {
        case FontWeight::Thin:
            result += "-Thin";
            break;
        case FontWeight::Ultralight:
            result += "-Ultralight";
            break;
        case FontWeight::Light:
            result += "-Light";
            break;
        case FontWeight::Regular:
            result += "-Regular";
            break;
        case FontWeight::Medium:
            result += "-Medium";
            break;
        case FontWeight::Semibold:
            result += "-Semibold";
            break;
        case FontWeight::Bold:
            result += "-Bold";
            break;
        case FontWeight::Heavy:
            result += "-Heavy";
            break;
        case FontWeight::Black:
            result += "-Black";
            break;
    }
    
    // Ajouter le style
    if (style == FontStyle::Italic) {
        result += "Italic";
    }
    
    return result;
} 