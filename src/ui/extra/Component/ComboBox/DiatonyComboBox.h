#pragma once

#include <JuceHeader.h>
#include "DiatonyComboBoxLookAndFeel.h"

/** @brief ComboBox personnalisé avec design harmonieux pour InfoColoredPanel. */
class DiatonyComboBox : public juce::ComboBox
{
public:
    DiatonyComboBox();
    ~DiatonyComboBox() override;
    
    /** @brief Adapte les couleurs selon le fond. */
    void adaptToBackgroundColour(juce::Colour backgroundColour);
    
    void setArrowVisible(bool visible) { diatonyLookAndFeel.setArrowVisible(visible); repaint(); }
    bool isArrowVisible() const { return diatonyLookAndFeel.isArrowVisible(); }
    
    /** @brief Définit un texte court pour un item (affiché une fois sélectionné). */
    void setShortTextForItem(int itemId, const juce::String& shortText);
    void enableShortDisplayMode(bool enabled);
    bool isShortDisplayModeEnabled() const { return diatonyLookAndFeel.isArrowVisible(); }
    
    void refreshDisplayedText();
    
    DiatonyComboBoxLookAndFeel& getDiatonyLookAndFeel() { return diatonyLookAndFeel; }
    
private:
    DiatonyComboBoxLookAndFeel diatonyLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyComboBox)
};
