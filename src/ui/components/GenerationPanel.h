#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class GenerationPanel : public juce::Component {
public:
    GenerationPanel() {
        // Initialisation des composants
        generateButton = std::make_unique<juce::TextButton>();
        playButton = std::make_unique<juce::TextButton>();
        
        // Configuration du bouton générer
        addAndMakeVisible(*generateButton);
        generateButton->setButtonText(juce::String::fromUTF8("Générer"));
        generateButton->setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
        generateButton->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        generateButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        
        generateButton->onClick = [this]() {
            if (onGenerateClicked) {
                onGenerateClicked();
            }
        };

        // Configuration du bouton lecture
        addAndMakeVisible(*playButton);
        playButton->setButtonText(juce::String::fromUTF8("Écouter"));
        playButton->setEnabled(false);
        playButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        playButton->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        playButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        
        playButton->onClick = [this]() {
            if (onPlayClicked) {
                onPlayClicked();
            }
        };
    }
    
    void paint(juce::Graphics& g) override {
        // Aucun dessin de cadre ou de titre
    }
    
    void resized() override {
        auto area = getLocalBounds().reduced(5);
        
        // Zone des boutons avec hauteur réduite
        auto buttonArea = area;
        // Réduire la hauteur en conservant les boutons centrés verticalement
        int buttonHeight = 30; // Hauteur réduite des boutons
        int verticalOffset = (buttonArea.getHeight() - buttonHeight) / 2;
        buttonArea = buttonArea.withTrimmedTop(verticalOffset).withHeight(buttonHeight);
        
        generateButton->setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5, 0));
        playButton->setBounds(buttonArea.reduced(5, 0));
    }
    
    // Méthodes pour mettre à jour l'état des boutons
    void setPlayButtonEnabled(bool enabled) {
        playButton->setEnabled(enabled);
    }
    
    void setGenerateButtonEnabled(bool enabled) {
        generateButton->setEnabled(enabled);
    }
    
    void setPlayButtonText(const juce::String& text) {
        playButton->setButtonText(text);
    }
    
    // Callbacks pour les boutons
    std::function<void()> onGenerateClicked;
    std::function<void()> onPlayClicked;
    
private:
    std::unique_ptr<juce::TextButton> generateButton;
    std::unique_ptr<juce::TextButton> playButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationPanel)
}; 