#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class GenerationPanel : public juce::Component, private juce::Timer {
public:
    GenerationPanel() {
        // Initialisation des composants
        generateButton = std::make_unique<juce::TextButton>();
        playButton = std::make_unique<juce::TextButton>();
        generationStatusLabel = std::make_unique<juce::Label>();
        playbackStatusLabel = std::make_unique<juce::Label>();
        
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

        // Configuration des labels de statut
        addAndMakeVisible(*generationStatusLabel);
        addAndMakeVisible(*playbackStatusLabel);

        generationStatusLabel->setText(juce::String::fromUTF8("Prêt à générer"), juce::dontSendNotification);
        generationStatusLabel->setJustificationType(juce::Justification::centred);
        playbackStatusLabel->setJustificationType(juce::Justification::centred);
    }
    
    ~GenerationPanel() override {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override {
        // Dessiner les cadres de statut
        auto area = getLocalBounds().reduced(15);
        auto statusArea = area.removeFromTop(100);

        // Cadre pour le statut de génération
        auto generationArea = statusArea.removeFromTop(45);
        auto generationTitleArea = generationArea.removeFromTop(20);  // Zone pour le titre

        // Dessiner le cadre de génération
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(generationArea.toFloat(), 5.0f, 2.0f);
        g.setColour(juce::Colours::white.withAlpha(0.05f));
        g.fillRoundedRectangle(generationArea.toFloat(), 5.0f);
        
        // Label "Statut de génération"
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(12.0f);
        g.drawText(juce::String::fromUTF8("Statut de génération"), generationTitleArea, juce::Justification::centred, false);

        statusArea.removeFromTop(10);  // Espace entre les cadres

        // Cadre pour le statut de lecture
        auto playbackArea = statusArea.removeFromTop(45);
        auto playbackTitleArea = playbackArea.removeFromTop(20);  // Zone pour le titre

        // Dessiner le cadre de lecture
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(playbackArea.toFloat(), 5.0f, 2.0f);
        g.setColour(juce::Colours::white.withAlpha(0.05f));
        g.fillRoundedRectangle(playbackArea.toFloat(), 5.0f);
        
        // Label "Statut de lecture"
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(12.0f);
        g.drawText(juce::String::fromUTF8("Statut de lecture"), playbackTitleArea, juce::Justification::centred, false);
    }
    
    void resized() override {
        auto area = getLocalBounds().reduced(15);
        
        // Zone des statuts
        auto statusArea = area.removeFromTop(100);

        // Generation label
        auto generationArea = statusArea.removeFromTop(45);
        generationArea.removeFromTop(20);  // Espace pour le titre
        generationStatusLabel->setBounds(generationArea.reduced(10, 5));
        
        statusArea.removeFromTop(10);
        
        // Playback label
        auto playbackArea = statusArea.removeFromTop(45);
        playbackArea.removeFromTop(20);  // Espace pour le titre
        playbackStatusLabel->setBounds(playbackArea.reduced(10, 5));
        
        area.removeFromTop(20);
        
        // Zone des boutons
        auto buttonArea = area.removeFromTop(40);
        generateButton->setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5));
        playButton->setBounds(buttonArea.reduced(5));
    }
    
    // Mettre à jour l'état des boutons et textes
    void setGenerationStatus(const juce::String& status) {
        generationStatusLabel->setText(status, juce::dontSendNotification);
        
        // Auto-effacer le message après un délai
        if (status.isNotEmpty()) {
            startTimer(2000);
        }
    }
    
    void setPlaybackStatus(const juce::String& status) {
        playbackStatusLabel->setText(status, juce::dontSendNotification);
    }
    
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
    void timerCallback() override {
        generationStatusLabel->setText("", juce::dontSendNotification);
        stopTimer();
    }
    
    std::unique_ptr<juce::TextButton> generateButton;
    std::unique_ptr<juce::TextButton> playButton;
    std::unique_ptr<juce::Label> generationStatusLabel;
    std::unique_ptr<juce::Label> playbackStatusLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationPanel)
}; 