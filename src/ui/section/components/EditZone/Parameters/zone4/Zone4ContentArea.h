#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Component/Panel/InfoColoredPanel.h"
#include "Zone4ScrollablePanel.h"
#include "utils/FontManager.h"
#include "model/DiatonyTypes.h"
#include "model/ModelIdentifiers.h"
#include "ui/DiatonyText.h"

/** @brief Zone de contenu pour Zone4 avec viewport scrollable et gestion d'état vide/plein. */
class Zone4ContentArea : public ColoredPanel
{
public:
    Zone4ContentArea();
    ~Zone4ContentArea() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void addRectangle();
    void addRectangle(juce::ValueTree chordState);
    void clearAllRectangles();
    bool hasContent() const;
    
    /** @brief Callback appelé quand un accord doit être supprimé (index de l'accord). */
    std::function<void(int)> onChordRemoved;
    
    juce::Rectangle<int> getPreferredSize() const;
    
private:
    juce::Viewport viewport;
    std::unique_ptr<Zone4ScrollablePanel> scrollableContent;
    juce::Label emptyLabel;
    
    static constexpr int PREFERRED_WIDTH = 300;
    static constexpr int PREFERRED_HEIGHT = 45;
    static constexpr int HORIZONTAL_PADDING = 8;
    static constexpr int TOP_PADDING = 6;
    static constexpr int BOTTOM_PADDING = 0;
    
    // Compteur pour les rectangles
    int nextRectangleId = 1;
    
    // Les 16 degrés d'accords disponibles
    static constexpr std::array<Diatony::ChordDegree, 16> chordDegrees = {
        Diatony::ChordDegree::First, Diatony::ChordDegree::Second,
        Diatony::ChordDegree::Third, Diatony::ChordDegree::Fourth,
        Diatony::ChordDegree::Fifth, Diatony::ChordDegree::Sixth,
        Diatony::ChordDegree::Seventh, Diatony::ChordDegree::FifthAppogiatura,
        Diatony::ChordDegree::FiveOfTwo, Diatony::ChordDegree::FiveOfThree,
        Diatony::ChordDegree::FiveOfFour, Diatony::ChordDegree::FiveOfFive,
        Diatony::ChordDegree::FiveOfSix, Diatony::ChordDegree::FiveOfSeven,
        Diatony::ChordDegree::FlatTwo, Diatony::ChordDegree::AugmentedSixth
    };
    
    // Les 5 états d'inversion disponibles
    static constexpr std::array<Diatony::ChordState, 5> chordStates = {
        Diatony::ChordState::Fundamental, Diatony::ChordState::FirstInversion,
        Diatony::ChordState::SecondInversion, Diatony::ChordState::ThirdInversion,
        Diatony::ChordState::FourthInversion
    };
    
    // Les 14 qualités d'accords disponibles (incluant Auto)
    static constexpr std::array<Diatony::ChordQuality, 14> chordQualities = {
        Diatony::ChordQuality::Auto, Diatony::ChordQuality::Major,
        Diatony::ChordQuality::Minor, Diatony::ChordQuality::Diminished,
        Diatony::ChordQuality::Augmented, Diatony::ChordQuality::AugmentedSixth,
        Diatony::ChordQuality::DominantSeventh, Diatony::ChordQuality::MajorSeventh,
        Diatony::ChordQuality::MinorSeventh, Diatony::ChordQuality::DiminishedSeventh,
        Diatony::ChordQuality::HalfDiminished, Diatony::ChordQuality::MinorMajorSeventh,
        Diatony::ChordQuality::MajorNinthDominant, Diatony::ChordQuality::MinorNinthDominant
    };
    
    void setupViewport();
    void setupEmptyLabel();
    void updateVisibility();
    juce::Colour getNextColour();
    void populateInfoColoredPanel(InfoColoredPanel* panel);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4ContentArea)
};
