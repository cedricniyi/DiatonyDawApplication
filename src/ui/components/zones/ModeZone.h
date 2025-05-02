#pragma once

#include <JuceHeader.h>
#include "BaseZone.h"

//==============================================================================
/**
 * Zone pour la sélection du mode
 */
class ModeZone : public BaseZone
{
public:
    ModeZone()
    : BaseZone(juce::String::fromUTF8("Mode"))
    {
        // Initialiser les composants ici si nécessaire
    }
    
    ~ModeZone() override = default;

protected:
    void paintContent(juce::Graphics& g, const juce::Rectangle<int>& contentBounds) override
    {
        // Texte d'espace réservé
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.setFont(14.0f);
        g.drawText(juce::String::fromUTF8("Composants de mode à venir..."), 
                  contentBounds, juce::Justification::centred, true);
    }
    
    void resizeContent(const juce::Rectangle<int>& contentBounds) override
    {
        // Placer les composants de l'interface utilisateur ici
    }

private:
    // Composants d'interface utilisateur à ajouter ici
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeZone)
}; 