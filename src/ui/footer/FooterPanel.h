#pragma once

#include <JuceHeader.h>
#include "components/ActionsArea.h"
#include "components/MidiPianoArea.h"

//==============================================================================
class FooterPanel : public juce::Component
{
public:
    FooterPanel() 
        : midiPianoArea(),
          actionsArea()
    {
        // Configurer le callback pour le bouton de redimensionnement
        midiPianoArea.onResizeToggle = [this] { if (onRequestResize) onRequestResize(); };
        
        addAndMakeVisible (midiPianoArea);    
        addAndMakeVisible (actionsArea);
    }
    
    void paint(juce::Graphics& g) override
    {

    }
    
    void resized() override
    {
        // Appliquer le padding à toute la zone
        auto area = getLocalBounds();

        // Diviser la zone en deux avec FlexBox
        juce::FlexBox fb;
        fb.flexDirection = juce::FlexBox::Direction::row;
        fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        fb.alignItems = juce::FlexBox::AlignItems::stretch;

        // Midi piano à gauche (25% de l'espace)
        fb.items.add(juce::FlexItem(midiPianoArea).withFlex(0.25f).withMargin({0, 4, 0, 0}));
        
        // Actions à droite (75% de l'espace restant)
        fb.items.add(juce::FlexItem(actionsArea).withFlex(0.75f).withMargin({0, 0, 0, 4}));

        fb.performLayout(area);
    }
    
    /** Callback déclenché quand une animation de redimensionnement est demandée */
    std::function<void()> onRequestResize;
    
private:
    MidiPianoArea       midiPianoArea;
    ActionsArea         actionsArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
}; 