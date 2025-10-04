#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Button/CircularButton.h"
#include "model/DiatonyTypes.h"
#include "ui/DiatonyText.h"

/**
 * Zone 1 - Premier paramètre/contrôle de section
 * Première ligne, première colonne
 */
class Zone1 : public juce::Component
{
public:
    Zone1();
    ~Zone1() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Binding depuis SectionEditor
    std::function<void(Diatony::BaseNote)> onBaseNoteChanged; // UI -> Modèle
    void setSelectedBaseNote(Diatony::BaseNote note);         // Modèle -> UI

private:
    // Couleur de fond pour cette zone
    juce::Colour titleBackgroundColour = juce::Colours::lightcoral.withAlpha(0.3f);
    juce::Colour contentBackgroundColour = juce::Colours::lightcoral.withAlpha(0.5f); // Plus foncé
    juce::Colour borderColour = juce::Colours::black;
    
    // Layout des zones
    juce::Rectangle<int> titleArea;
    juce::Rectangle<int> contentArea;
    
    // FontManager pour le titre
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Les 7 boutons circulaires correspondant aux 7 notes de base
    std::array<std::unique_ptr<CircularButton>, 7> circularButtons;
    
    // Les 7 notes de base dans l'ordre
    static constexpr std::array<Diatony::BaseNote, 7> baseNotes = {
        Diatony::BaseNote::C, Diatony::BaseNote::D, Diatony::BaseNote::E, 
        Diatony::BaseNote::F, Diatony::BaseNote::G, Diatony::BaseNote::A, Diatony::BaseNote::B
    };
    
    // Note de base actuellement sélectionnée
    Diatony::BaseNote selectedBaseNote = Diatony::BaseNote::C;
    
    void setupCircularButtons();
    void layoutCircularButtons();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone1)
};