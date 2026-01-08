#pragma once

#include <JuceHeader.h>
#include "DiatonyComboBoxLookAndFeel.h"

//==============================================================================
// ComboBox personnalisé avec design harmonieux avec InfoColoredPanel
// Supporte l'affichage de texte court une fois sélectionné
class DiatonyComboBox : public juce::ComboBox
{
public:
    DiatonyComboBox();
    ~DiatonyComboBox() override;
    
    // Configuration des couleurs pour s'harmoniser avec le contexte
    void adaptToBackgroundColour(juce::Colour backgroundColour);
    
    // Cacher/afficher la flèche du dropdown
    void setArrowVisible(bool visible) { diatonyLookAndFeel.setArrowVisible(visible); repaint(); }
    bool isArrowVisible() const { return diatonyLookAndFeel.isArrowVisible(); }
    
    // Mode texte court : affiche une version abrégée une fois sélectionné
    // Les textes courts sont gérés par le LookAndFeel pour éviter les problèmes avec setText()
    void setShortTextForItem(int itemId, const juce::String& shortText);
    void enableShortDisplayMode(bool enabled);
    bool isShortDisplayModeEnabled() const { return diatonyLookAndFeel.isArrowVisible(); }
    
    // Force la mise à jour visuelle (repositionne le label avec le texte court)
    void refreshDisplayedText();
    
    // Accès au LookAndFeel pour personnalisation avancée
    DiatonyComboBoxLookAndFeel& getDiatonyLookAndFeel() { return diatonyLookAndFeel; }
    
private:
    DiatonyComboBoxLookAndFeel diatonyLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyComboBox)
};