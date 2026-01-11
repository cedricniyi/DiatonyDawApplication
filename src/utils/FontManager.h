#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <map>
#include <string>

/** @brief Gestionnaire de polices SF Pro avec cache des typefaces. */
class FontManager {
public:
    enum class FontWeight { Thin, Ultralight, Light, Regular, Medium, Semibold, Bold, Heavy, Black };
    enum class FontStyle { Regular, Italic };
    
    FontManager();
    ~FontManager() = default;
    
    juce::FontOptions getSFProDisplay(float size, FontWeight weight = FontWeight::Regular, FontStyle style = FontStyle::Regular);
    juce::FontOptions getSFProText(float size, FontWeight weight = FontWeight::Regular, FontStyle style = FontStyle::Regular);
    juce::FontOptions getSFProRounded(float size, FontWeight weight = FontWeight::Regular, FontStyle style = FontStyle::Regular);
    
    juce::FontOptions getDisplayFont(float size) { return getSFProDisplay(size, FontWeight::Regular); }
    juce::FontOptions getTextFont(float size) { return getSFProText(size, FontWeight::Regular); }
    juce::FontOptions getBoldFont(float size) { return getSFProDisplay(size, FontWeight::Bold); }
    juce::FontOptions getSemiboldFont(float size) { return getSFProDisplay(size, FontWeight::Semibold); }
    
private:
    juce::Typeface::Ptr createTypefaceFromBinaryData(const char* data, size_t dataSize);
    std::string getFontFileName(const std::string& family, FontWeight weight, FontStyle style);
    juce::FontOptions getFont(const std::string& family, float size, FontWeight weight, FontStyle style);
    void initializeFonts();
    
    std::map<std::string, juce::Typeface::Ptr> typefaceCache;
};
