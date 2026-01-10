#include "FontManager.h"
#include "BinaryData.h"
#include <algorithm>

FontManager::FontManager() {
    initializeFonts();
}

void FontManager::initializeFonts() {
    // Pré-charger les fonts les plus utilisées
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
    
    if (typefaceCache.find(fontKey) == typefaceCache.end()) {
        juce::Typeface::Ptr typeface = nullptr;
        
        std::string resourceName = fontKey;
        std::replace(resourceName.begin(), resourceName.end(), '-', '_');
        resourceName += "_otf";
        
        int resourceSize = 0;
        const char* resourceData = nullptr;
        
        // Mapping des fonts SF Pro vers BinaryData
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
        else if (fontKey == "SF-Pro-Display-Medium") {
            resourceData = BinaryData::SFProDisplayMedium_otf;
            resourceSize = BinaryData::SFProDisplayMedium_otfSize;
        }
        else if (fontKey == "SF-Pro-Display-Light") {
            resourceData = BinaryData::SFProDisplayLight_otf;
            resourceSize = BinaryData::SFProDisplayLight_otfSize;
        }
        else if (fontKey == "SF-Pro-Display-Heavy") {
            resourceData = BinaryData::SFProDisplayHeavy_otf;
            resourceSize = BinaryData::SFProDisplayHeavy_otfSize;
        }
        else if (fontKey == "SF-Pro-Display-Black") {
            resourceData = BinaryData::SFProDisplayBlack_otf;
            resourceSize = BinaryData::SFProDisplayBlack_otfSize;
        }
        else if (fontKey == "SF-Pro-Display-Thin") {
            resourceData = BinaryData::SFProDisplayThin_otf;
            resourceSize = BinaryData::SFProDisplayThin_otfSize;
        }
        else if (fontKey == "SF-Pro-Display-Ultralight") {
            resourceData = BinaryData::SFProDisplayUltralight_otf;
            resourceSize = BinaryData::SFProDisplayUltralight_otfSize;
        }
        else if (fontKey == "SF-Pro-Text-Regular") {
            resourceData = BinaryData::SFProTextRegular_otf;
            resourceSize = BinaryData::SFProTextRegular_otfSize;
        }
        else if (fontKey == "SF-Pro-Text-Medium") {
            resourceData = BinaryData::SFProTextMedium_otf;
            resourceSize = BinaryData::SFProTextMedium_otfSize;
        }
        else if (fontKey == "SF-Pro-Text-Bold") {
            resourceData = BinaryData::SFProTextBold_otf;
            resourceSize = BinaryData::SFProTextBold_otfSize;
        }
        else if (fontKey == "SF-Pro-Text-Semibold") {
            resourceData = BinaryData::SFProTextSemibold_otf;
            resourceSize = BinaryData::SFProTextSemibold_otfSize;
        }
        else if (fontKey == "SF-Pro-Rounded-Bold") {
            resourceData = BinaryData::SFProRoundedBold_otf;
            resourceSize = BinaryData::SFProRoundedBold_otfSize;
        }
        else if (fontKey == "SF-Pro-Rounded-Regular") {
            resourceData = BinaryData::SFProRoundedRegular_otf;
            resourceSize = BinaryData::SFProRoundedRegular_otfSize;
        }
        else if (fontKey == "SF-Pro-Rounded-Medium") {
            resourceData = BinaryData::SFProRoundedMedium_otf;
            resourceSize = BinaryData::SFProRoundedMedium_otfSize;
        }
        
        if (resourceData != nullptr && resourceSize > 0)
            typeface = createTypefaceFromBinaryData(resourceData, resourceSize);
        
        // Fallback vers Arial puis font par défaut JUCE
        if (typeface == nullptr) {
            juce::Font defaultFont("Arial", size, juce::Font::plain);
            typeface = defaultFont.getTypefacePtr();
            
            if (typeface == nullptr) {
                juce::Font juceDefaultFont;
                typeface = juceDefaultFont.getTypefacePtr();
            }
        }
        
        typefaceCache[fontKey] = typeface;
    }
    
    auto cachedTypeface = typefaceCache[fontKey];
    if (cachedTypeface == nullptr) {
        juce::Font emergencyFont("Arial", size, juce::Font::plain);
        auto emergencyTypeface = emergencyFont.getTypefacePtr();
        
        if (emergencyTypeface == nullptr)
            return juce::FontOptions().withHeight(size);
        
        return juce::FontOptions(emergencyTypeface).withHeight(size);
    }
    
    return juce::FontOptions(cachedTypeface).withHeight(size);
}

juce::Typeface::Ptr FontManager::createTypefaceFromBinaryData(const char* data, size_t dataSize) {
    return juce::Typeface::createSystemTypefaceFor(data, dataSize);
}

std::string FontManager::getFontFileName(const std::string& family, FontWeight weight, FontStyle style) {
    std::string result = family;
    
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
    
    if (style == FontStyle::Italic)
        result += "Italic";
    
    return result;
}
