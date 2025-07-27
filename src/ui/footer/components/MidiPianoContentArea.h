#pragma once

#include <JuceHeader.h>
#include "ui/extra/Button/StyledButton.h"
#include "ui/extra/Component/TextPill.h"
#include "utils/FontManager.h"

//==============================================================================
class MidiPianoContentArea : public juce::Component
{
public:
    MidiPianoContentArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    std::function<void()> onResizeToggle;
    
private:
    void calculateRowPercentages(const juce::Rectangle<int>& topRow, const juce::Rectangle<int>& bottomRow);
    void setupTextPills();
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Bouton de redimensionnement (masqué mais gardé dans le code)
    StyledButton resizeButton;
    
    // Les 5 zones colorées avec TextPill (gèrent fond et texte)
    TextPill selectedSecPill;
    TextPill pianoModePill;
    TextPill midiInfoPill;
    TextPill moreInfoPill;
    
    // État de visibilité de la zone 5
    bool isZone5Visible;
    
    // Pourcentages pour chaque ligne
    float topLeftPercentage = 50.0f;
    float topRightPercentage = 50.0f;
    float bottomLeftPercentage = 50.0f;
    float bottomRightPercentage = 50.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPianoContentArea)
}; 