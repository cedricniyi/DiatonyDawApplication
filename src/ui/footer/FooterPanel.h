#pragma once

#include <JuceHeader.h>
#include "components/MidiPianoArea.h"

//==============================================================================
class FooterPanel : public juce::Component
{
public:
    FooterPanel() 
        : midiPianoArea()
    {
        // Configurer le callback pour le bouton de redimensionnement
        midiPianoArea.onResizeToggle = [this] { 
            DBG("FooterPanel: Callback onResizeToggle reçu !"); // Debug
            if (onRequestResize) {
                DBG("FooterPanel: Callback onRequestResize trouvé, appel en cours...");
                onRequestResize(); 
            } else {
                DBG("FooterPanel: ERREUR: Callback onRequestResize est null !");
            }
        };
        
        addAndMakeVisible (midiPianoArea);    
    }
    
    void paint(juce::Graphics& g) override
    {

    }
    
    void resized() override
    {
        auto area = getLocalBounds();

        // Utiliser Grid pour centrer le MidiPianoArea
        juce::Grid grid;
        
        // Définir une seule ligne et trois colonnes
        grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        grid.templateColumns = { 
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),      // Colonne gauche (flexible)
            juce::Grid::TrackInfo(juce::Grid::Px(400)),    // Colonne centre (largeur fixe 400px)
            juce::Grid::TrackInfo(juce::Grid::Fr(1))       // Colonne droite (flexible)
        };
        
        // Centrer le contenu
        grid.justifyContent = juce::Grid::JustifyContent::center;
        grid.alignContent = juce::Grid::AlignContent::center;
        grid.alignItems = juce::Grid::AlignItems::stretch;
        
        // Ajouter le MidiPianoArea dans la colonne du centre
        grid.items.add(juce::GridItem(midiPianoArea)
                      .withArea(1, 2));  // Ligne 1, Colonne 2 (centre)
        
        // Appliquer le layout
        grid.performLayout(area);
    }
    
    /** Callback déclenché quand une animation de redimensionnement est demandée */
    std::function<void()> onRequestResize;
    
private:
    MidiPianoArea       midiPianoArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
}; 