#pragma once

#include <JuceHeader.h>
#include "components/MidiPianoArea.h"
#include "components/InteractivePiano.h"
#include "../extra/ColoredPanel.h"
#include "../../utils/FontManager.h"

//==============================================================================
class FooterPanel : public juce::Component
{
public:
    FooterPanel() 
        : midiPianoArea(),
          interactivePiano(juce::Colours::white), // Couleur blanche
          isExpanded(false),
          gridTransitionFraction(0.0f) // Paramètre d'interpolation (0 = compact, 1 = élargi)
    {
        // Configurer le callback pour le bouton de redimensionnement
        midiPianoArea.onResizeToggle = [this] { 
            DBG("FooterPanel: Callback onResizeToggle reçu !");
            if (onToggleRequest) {
                DBG("FooterPanel: Callback onToggleRequest trouvé, appel en cours...");
                onToggleRequest(); // Déclenche la demande d'animation
            } else {
                DBG("FooterPanel: ERREUR: Callback onToggleRequest est null !");
            }
        };
        
        // Configurer le composant qui va fade in/out
        interactivePiano.setAlpha(1.0f);
        interactivePiano.setVisible(true);
        
        addAndMakeVisible(midiPianoArea);
        addAndMakeVisible(interactivePiano);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Pas de peinture spéciale nécessaire
    }
    
    void resized() override
    {
        auto area = getLocalBounds();

        // Utiliser Grid avec interpolation progressive selon gridTransitionFraction
        juce::Grid grid;
        
        // Définir une seule ligne
        grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        
        // Interpolation smooth entre les deux configurations
        if (gridTransitionFraction <= 0.001f)
        {
            // État compact : 3 colonnes [Fr(1) | 400px | Fr(1)]
            grid.templateColumns = { 
                juce::Grid::TrackInfo(juce::Grid::Fr(1)),      // Colonne gauche (flexible)
                juce::Grid::TrackInfo(juce::Grid::Px(400)),    // Colonne centre MidiPianoArea
                juce::Grid::TrackInfo(juce::Grid::Fr(1))       // Colonne droite (flexible)
            };
            
            // Ajouter seulement le MidiPianoArea
            grid.items.add(juce::GridItem(midiPianoArea)
                          .withArea(1, 2));       // Ligne 1, Colonne 2 (centre)
        }
        else
        {
            // État progressif : 4 colonnes avec interpolation
            // La largeur de la colonne "mystère" grandit progressivement
            float mysteryColumnWidth = 400.0f * gridTransitionFraction;
            
            grid.templateColumns = { 
                juce::Grid::TrackInfo(juce::Grid::Fr(1)),                           // Colonne gauche (flexible)
                juce::Grid::TrackInfo(juce::Grid::Px(400)),                        // Colonne MidiPianoArea (fixe)
                juce::Grid::TrackInfo(juce::Grid::Px(static_cast<int>(mysteryColumnWidth))), // Colonne FadingDemoPanel (progressive)
                juce::Grid::TrackInfo(juce::Grid::Fr(1))                           // Colonne droite (flexible)
            };
            
            // Ajouter les deux composants
            grid.items.add(juce::GridItem(midiPianoArea)
                          .withArea(1, 2)         // Ligne 1, Colonne 2
                          .withMargin(juce::GridItem::Margin(0, 4, 0, 0))); // Marge droite
            
            grid.items.add(juce::GridItem(interactivePiano)
                          .withArea(1, 3)         // Ligne 1, Colonne 3
                          .withMargin(juce::GridItem::Margin(0, 0, 0, 4))); // Marge gauche
        }
        
        // Centrer le contenu
        grid.justifyContent = juce::Grid::JustifyContent::center;
        grid.alignContent = juce::Grid::AlignContent::center;
        grid.alignItems = juce::Grid::AlignItems::stretch;
        
        // Appliquer le layout
        grid.performLayout(area);
    }
    
    // === Interface publique pour contrôler l'état ===
    
    /** Met à jour la fraction d'interpolation de la grille (0 = compact, 1 = élargi) */
    void setGridFraction(float fraction)
    {
        gridTransitionFraction = juce::jlimit(0.0f, 1.0f, fraction);
        resized(); // Redessiner le layout
    }
    
    /** Met à jour l'état d'expansion */
    void setExpanded(bool expanded)
    {
        isExpanded = expanded;
    }
    
    /** Retourne l'état d'expansion actuel */
    bool getExpanded() const
    {
        return isExpanded;
    }
    
    /** Retourne une référence au composant qui fade in/out */
    juce::Component& getFadingComponent()
    {
        return interactivePiano;
    }
    
    /** Retourne une référence à la fraction de grille pour l'animation */
    float& getGridFractionRef()
    {
        return gridTransitionFraction;
    }
    
    /** Callback déclenché quand une animation de toggle est demandée */
    std::function<void()> onToggleRequest;
    
private:
    MidiPianoArea midiPianoArea;
    InteractivePiano interactivePiano;
    bool isExpanded; // État du toggle de la grille
    float gridTransitionFraction; // Paramètre d'interpolation (0 = compact, 1 = élargi)
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
}; 