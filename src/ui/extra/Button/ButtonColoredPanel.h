#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Types de contenu pour les panels
 */
enum class PanelContentType
{
    Section,
    Modulation
};

/**
 * Panel coloré cliquable basé sur juce::Button
 * Ressemble visuellement à ColoredPanel mais avec onClick natif
 * Peut afficher du texte centré (ex: ID de section)
 */
class ButtonColoredPanel : public juce::Button
{
public:
    ButtonColoredPanel(juce::Colour color);
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void mouseDown(const juce::MouseEvent& e) override;
    
    // Callback pour clic droit (suppression de section)
    std::function<void()> onRightClick;
    
    // Gestion de la sélection
    void setSelected(bool selected);
    bool getSelected() const;
    
    void setColor(juce::Colour color);
    juce::Colour getColor() const;
    
    void setUserData(const juce::var& data);
    juce::var getUserData() const;
    
    // Gestion du type de contenu
    void setContentType(PanelContentType type);
    PanelContentType getContentType() const;
    
    // Gestion du texte affiché (ID de section)
    void setDisplayText(const juce::String& text);
    juce::String getDisplayText() const;
    
    // Contrôle de l'affichage du texte
    void setShowText(bool shouldShowText);
    bool getShowText() const;

private:
    juce::Colour baseColor;
    bool isSelected;
    juce::var userData;
    PanelContentType contentType;
    juce::String displayText;  // Texte à afficher sur le panel
    bool showText;  // Contrôle si le texte doit être affiché

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonColoredPanel)
}; 