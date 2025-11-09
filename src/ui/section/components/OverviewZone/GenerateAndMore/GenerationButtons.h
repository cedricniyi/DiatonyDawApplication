#pragma once

#include <JuceHeader.h>
#include "ui/extra/Button/StyledButton.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"

//==============================================================================
class GenerationButtons : public ColoredPanel
{
public:
    GenerationButtons();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Méthodes pour accéder aux boutons si nécessaire
    StyledButton& getGenerateButton();
    StyledButton& getMoreButton();
    
    // Méthode pour calculer la taille préférée du composant
    juce::Rectangle<int> getPreferredSize() const;
    
    // Callback pour le bouton Generate
    std::function<void()> onGenerateClicked;
    
private:
    StyledButton generateButton;
    StyledButton moreButton;
    juce::FlexBox flexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationButtons)
}; 