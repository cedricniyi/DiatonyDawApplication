#include "FooterPanel.h"
#include "components/MidiPianoArea.h"
#include "components/MidiPianoContentArea.h"
#include "components/InteractivePiano/InteractivePiano.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/UIStateIdentifiers.h"
#include "utils/FontManager.h"

//==============================================================================
FooterPanel::FooterPanel() 
    : midiPianoArea(),
      interactivePiano(juce::Colours::white.withAlpha(0.50f)),
      isExpanded(false),
      gridTransitionFraction(0.0f) // Facteur d'interpolation pour l'animation du layout (0.0 = compact, 1.0 = étendu)
{        
    addAndMakeVisible(midiPianoArea);
    addAndMakeVisible(interactivePiano);
    
    // Le clic sur le bouton de redimensionnement bascule la visibilité du piano interactif
    getMidiPianoContentArea().onResizeToggle = [this]() {
        if (!appState.isValid()) {
            return;
        }
        
        bool currentState = static_cast<bool>(appState.getProperty(UIStateIdentifiers::interactivePianoVisible, false));
        appState.setProperty(UIStateIdentifiers::interactivePianoVisible, !currentState, nullptr);
    };
}

FooterPanel::~FooterPanel()
{
    if (appState.isValid())
        appState.removeListener(this);
}

void FooterPanel::setAppState(juce::ValueTree& state)
{
    if (appState.isValid())
        appState.removeListener(this);
        
    appState = state;
    appState.addListener(this);
    
    // Synchronise l'état du footer avec les données du ValueTree
    updateFooterState();
}

void FooterPanel::paint(juce::Graphics& g)
{
}

void FooterPanel::resized()
{
    auto area = getLocalBounds();

    // Layout basé sur une grille qui s'adapte selon gridTransitionFraction
    juce::Grid grid;
    
    // Le layout est sur une seule ligne
    grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
    
    // Choix du layout en fonction de l'état d'expansion
    if (gridTransitionFraction <= 0.001f)
    {
        // État compact : le piano MIDI est centré dans une grille à 3 colonnes
        grid.templateColumns = { 
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),      // Colonne gauche flexible
            juce::Grid::TrackInfo(juce::Grid::Px(400)),    // Colonne centrale pour MidiPianoArea
            juce::Grid::TrackInfo(juce::Grid::Fr(1))       // Colonne droite flexible
        };
        
        // Positionne le piano MIDI dans la colonne centrale
        grid.items.add(juce::GridItem(midiPianoArea)
                      .withArea(1, 2));
    }
    else
    {
        // État étendu : le piano interactif apparaît progressivement à droite
        float interactivePianoWidth = 400.0f * gridTransitionFraction;
        
        grid.templateColumns = { 
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),                               // Colonne gauche flexible
            juce::Grid::TrackInfo(juce::Grid::Px(400)),                             // Colonne pour MidiPianoArea
            juce::Grid::TrackInfo(juce::Grid::Px(static_cast<int>(interactivePianoWidth))), // Colonne pour le piano interactif, largeur animée
            juce::Grid::TrackInfo(juce::Grid::Fr(1))                                // Colonne droite flexible
        };
        
        // Positionne les deux pianos
        grid.items.add(juce::GridItem(midiPianoArea)
                      .withArea(1, 2)
                      .withMargin(juce::GridItem::Margin(0, 4, 0, 0))); // Marge à droite
        
        grid.items.add(juce::GridItem(interactivePiano)
                      .withArea(1, 3)
                      .withMargin(juce::GridItem::Margin(0, 0, 0, 4))); // Marge à gauche
    }
    
    // Configuration de l'alignement de la grille
    grid.justifyContent = juce::Grid::JustifyContent::center;
    grid.alignContent = juce::Grid::AlignContent::center;
    grid.alignItems = juce::Grid::AlignItems::stretch;
    
    // Applique la configuration de la grille
    grid.performLayout(area);
}

void FooterPanel::setExpanded(bool expanded)
{
    isExpanded = expanded;
}

bool FooterPanel::getExpanded() const
{
    return isExpanded;
}

juce::Component& FooterPanel::getFadingComponent()
{
    return interactivePiano;
}

float& FooterPanel::getGridFractionRef()
{
    return gridTransitionFraction;
}

MidiPianoContentArea& FooterPanel::getMidiPianoContentArea()
{
    return midiPianoArea.getContentArea();
}

void FooterPanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                          const juce::Identifier& property)
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

void FooterPanel::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void FooterPanel::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void FooterPanel::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void FooterPanel::valueTreeParentChanged(juce::ValueTree&) {}

void FooterPanel::updateFooterState()
{
    if (!appState.isValid()) return;
    
    bool expanded = static_cast<bool>(appState.getProperty(UIStateIdentifiers::footerExpanded, false));
    setExpanded(expanded);
}

void FooterPanel::updateInteractivePianoState()
{
    if (!appState.isValid()) return;
    
    bool visible = static_cast<bool>(appState.getProperty(UIStateIdentifiers::interactivePianoVisible, false));
    
    // Notifie l'Animator pour déclencher l'animation de visibilité
    if (onInteractivePianoVisibilityChange)
    {
        onInteractivePianoVisibilityChange(visible);
    }
    else
    {
        // Fallback : mise à jour directe de l'état si aucun animateur n'est attaché
        setExpanded(visible);
    }
} 