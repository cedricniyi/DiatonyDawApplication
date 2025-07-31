#pragma once

#include <JuceHeader.h>
#include "DiatonyComboBoxLookAndFeel.h"

//==============================================================================
// ComboBox personnalisé avec design harmonieux avec InfoColoredPanel
class DiatonyComboBox : public juce::ComboBox
{
public:
    DiatonyComboBox();
    ~DiatonyComboBox() override;
    
    // Configuration des couleurs pour s'harmoniser avec le contexte
    void adaptToBackgroundColour(juce::Colour backgroundColour);
    
    // Accès au LookAndFeel pour personnalisation avancée
    DiatonyComboBoxLookAndFeel& getDiatonyLookAndFeel() { return diatonyLookAndFeel; }
    
private:
    DiatonyComboBoxLookAndFeel diatonyLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyComboBox)
};