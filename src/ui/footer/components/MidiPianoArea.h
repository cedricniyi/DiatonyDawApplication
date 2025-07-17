#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"
#include "../../extra/StyledButton.h"
#include "../../../utils/FontManager.h"
#include "MidiPianoContentArea.h"

//==============================================================================
class MidiPianoArea : public ColoredPanel
{
public:
    MidiPianoArea() 
        : ColoredPanel(juce::Colours::white),
          contentArea(),
          resizeButton(juce::String::fromUTF8("📏 Resize Footer"), 
                      juce::Colour(0xff4a90e2),  // Couleur normale (bleu)
                      juce::Colour(0xff357abd),  // Couleur highlight (bleu plus foncé)
                      14.0f,                     // Taille de police
                      FontManager::FontWeight::Medium)
    {
        // Label "Midi Piano"
        headerLabel.setText(juce::String::fromUTF8("Midi Piano"), juce::dontSendNotification);
        headerLabel.setJustificationType (juce::Justification::centredLeft);
        headerLabel.setColour (juce::Label::textColourId, juce::Colours::black);
        
        // Correction : Convertir FontOptions en Font
        auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
        headerLabel.setFont(juce::Font(fontOptions));
        
        // Configuration du bouton de redimensionnement
        resizeButton.onClick = [this] { 
            DBG("Bouton de redimensionnement cliqué !"); // Debug
            if (onResizeToggle) {
                DBG("Callback onResizeToggle trouvé, appel en cours...");
                onResizeToggle(); // Déclenche l'animation de redimensionnement
            } else {
                DBG("ERREUR: Callback onResizeToggle est null !");
            }
        };
        
        addAndMakeVisible (headerLabel);
        addAndMakeVisible (contentArea);
        addAndMakeVisible (resizeButton);
    }
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        // Appliquer le padding à toute la zone
        auto area = getLocalBounds().reduced(20, 10);

        // Calculer la hauteur nécessaire pour le header
        auto headerHeight = juce::jmax(30, static_cast<int>(headerLabel.getFont().getHeight() + 10)); // hauteur du texte + marge
        auto headerArea = area.removeFromTop(headerHeight);
        
        // Positionner le label "Midi Piano" et le bouton dans le header
        auto labelArea = headerArea;
        auto buttonWidth = 120;
        auto buttonArea = labelArea.removeFromRight(buttonWidth);
        
        headerLabel.setBounds(labelArea);
        resizeButton.setBounds(buttonArea);

        int headerBottomMargin = 8;
        area.removeFromTop (headerBottomMargin);
        
        // Le reste de l'espace va au contentArea
        contentArea.setBounds(area);
    }
    
    /** Callback déclenché quand le bouton de redimensionnement est cliqué */
    std::function<void()> onResizeToggle;
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Label headerLabel;
    MidiPianoContentArea contentArea;
    StyledButton resizeButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPianoArea)
}; 