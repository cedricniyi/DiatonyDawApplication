#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"
#include "zone4/Zone4ContentArea.h"
#include "model/DiatonyTypes.h"

/**
 * Zone 4 - Quatrième paramètre/contrôle de section
 * Seconde ligne, toute la largeur
 * Contient maintenant une zone scrollable pour ajouter des rectangles
 * Hérite de ColoredPanel pour avoir un fond coloré automatique
 */
class Zone4 : public ColoredPanel
{
public:
    Zone4();
    ~Zone4() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Binding depuis SectionEditor (même pattern que Zone1/2/3)
    std::function<void(Diatony::ChordDegree, Diatony::ChordQuality, Diatony::ChordState)> onChordAdded; // UI -> Modèle
    
    // Synchronisation Modèle -> UI (afficher les accords existants avec leurs valeurs)
    void syncWithProgression(const std::vector<juce::ValueTree>& chords);

private:
    // Couleur de fond pour cette zone
    juce::Colour titleBackgroundColour = juce::Colours::lightsteelblue.withAlpha(0.3f);
    juce::Colour contentBackgroundColour = juce::Colours::lightsteelblue.withAlpha(0.5f); // Plus foncé
    juce::Colour borderColour = juce::Colours::black;
    
    // Layout des zones
    juce::Rectangle<int> titleArea;
    juce::Rectangle<int> contentArea;
    
    // Composants UI
    juce::Label titleLabel;
    StyledButton addButton;
    Zone4ContentArea contentAreaComponent;
    
    // FontManager pour le titre
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Grid pour le layout
    juce::Grid mainGrid;
    
    // Configuration
    static constexpr int INTERNAL_PADDING = 12;
    static constexpr float TITLE_HEIGHT_RATIO = 0.3f;
    
    // Méthodes privées
    void setupGrid();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4)
};