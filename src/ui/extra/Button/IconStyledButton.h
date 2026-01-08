// IconStyledButton.h
#pragma once
#include <JuceHeader.h>
#include <memory>

/// Un Button réutilisable avec une icône SVG, style similaire à StyledButton
class IconStyledButton : public juce::Button
{
public:
    /** 
     * Constructeur avec icône SVG et couleurs de fond
     * @param buttonName Nom du bouton (pour l'accessibilité)
     * @param svgData Données binaires du SVG
     * @param svgDataSize Taille des données SVG
     * @param normalColor Couleur de fond normale
     * @param highlightColor Couleur de fond au survol/clic
     * @param iconColor Couleur de l'icône (par défaut blanc)
     */
    IconStyledButton(const juce::String& buttonName,
                     const char* svgData,
                     size_t svgDataSize,
                     juce::Colour normalColor,
                     juce::Colour highlightColor,
                     juce::Colour iconColor = juce::Colours::white);
    
    ~IconStyledButton() override = default;
    
    /** Change la couleur de l'icône */
    void setIconColour(juce::Colour newColour);
    
    /** Change les couleurs de fond */
    void setBackgroundColours(juce::Colour normalColor, juce::Colour highlightColor);

protected:
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    std::unique_ptr<juce::Drawable> loadSvgIcon(const char* data, size_t dataSize);
    void updateIconColours();
    
    std::unique_ptr<juce::Drawable> originalIcon;   // Icône originale (pour créer des copies)
    std::unique_ptr<juce::Drawable> normalIcon;     // Icône état normal
    std::unique_ptr<juce::Drawable> hoverIcon;      // Icône état survol
    std::unique_ptr<juce::Drawable> downIcon;       // Icône état pressé
    
    juce::Colour backgroundNormal;
    juce::Colour backgroundHighlight;
    juce::Colour iconColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IconStyledButton)
};

