#pragma once

#include <JuceHeader.h>
#include "components/MidiPianoArea.h"
#include "components/MidiPianoContentArea.h"
#include "components/InteractivePiano/InteractivePiano.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/UIStateIdentifiers.h"
#include "utils/FontManager.h"

//==============================================================================
class FooterPanel : public juce::Component, public juce::ValueTree::Listener
{
public:
    FooterPanel() 
        : midiPianoArea(),
          interactivePiano(juce::Colours::white.withAlpha(0.50f)), // Couleur blanche
          isExpanded(false),
          gridTransitionFraction(0.0f) // Paramètre d'interpolation (0 = compact, 1 = élargi)
    {        
        addAndMakeVisible(midiPianoArea);
        addAndMakeVisible(interactivePiano);
        
        // Configuration du callback pour le toggle du piano interactif
        getMidiPianoContentArea().onResizeToggle = [this]() {
            if (!appState.isValid()) {
                DBG("FooterPanel: ERREUR: ValueTree non initialisé !");
                return;
            }
            
            bool currentState = static_cast<bool>(appState.getProperty(UIStateIdentifiers::interactivePianoVisible, false));
            appState.setProperty(UIStateIdentifiers::interactivePianoVisible, !currentState, nullptr);
            DBG("FooterPanel: Toggle InteractivePiano -> " << (!currentState ? "visible" : "caché"));
        };
    }
    
    ~FooterPanel() override
    {
        if (appState.isValid())
            appState.removeListener(this);
    }

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state)
    {
        if (appState.isValid())
            appState.removeListener(this);
            
        appState = state;
        appState.addListener(this);
        
        // Synchroniser l'état initial
        updateFooterState();
        DBG("FooterPanel: ValueTree connecté et état initial synchronisé");
    }
    
    void paint(juce::Graphics& g) override
    {
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
    
    // === Setters publics pour que l'Animator puisse les appeler ===
    
    /** Met à jour l'état d'expansion */
    void setExpanded(bool expanded)
    {
        isExpanded = expanded;
    }
    
    // === Accesseurs publics pour que l'Animator puisse les lire ===
    
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
    
    /** Accès au MidiPianoContentArea pour configuration des callbacks */
    MidiPianoContentArea& getMidiPianoContentArea()
    {
        return midiPianoArea.getContentArea();
    }

    // === Callbacks pour que l'Animator puisse s'y abonner ===
    
    /** Callback déclenché quand interactivePianoVisible change dans le ValueTree */
    std::function<void(bool)> onInteractivePianoVisibilityChange;

    // =================================================================================
    // ValueTree::Listener interface
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        if (property == UIStateIdentifiers::footerExpanded)
        {
            updateFooterState();
        }
        else if (property == UIStateIdentifiers::interactivePianoVisible)
        {
            updateInteractivePianoState();
        }
    }

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    
private:
    MidiPianoArea midiPianoArea;
    InteractivePiano interactivePiano;
    bool isExpanded; // État du toggle de la grille
    float gridTransitionFraction; // Paramètre d'interpolation (0 = compact, 1 = élargi)
    
    juce::ValueTree appState;      // Référence au ValueTree global

    /** Met à jour l'état d'expansion du footer selon le ValueTree */
    void updateFooterState()
    {
        if (!appState.isValid()) return;
        
        bool expanded = static_cast<bool>(appState.getProperty(UIStateIdentifiers::footerExpanded, false));
        setExpanded(expanded);
        DBG("FooterPanel: État footer mis à jour -> " << (expanded ? "étendu" : "réduit"));
    }

    /** Met à jour l'état du piano interactif et notifie via callback */
    void updateInteractivePianoState()
    {
        if (!appState.isValid()) return;
        
        bool visible = static_cast<bool>(appState.getProperty(UIStateIdentifiers::interactivePianoVisible, false));
        
        // Notifier via callback si l'Animator s'est abonné
        if (onInteractivePianoVisibilityChange)
        {
            onInteractivePianoVisibilityChange(visible);
            DBG("FooterPanel: Notifie Animator via callback -> " << (visible ? "visible" : "caché"));
        }
        else
        {
            // Fallback : mise à jour directe sans animation
            setExpanded(visible);
            DBG("FooterPanel: Mise à jour directe InteractivePiano (pas d'Animator) -> " << (visible ? "visible" : "caché"));
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
}; 