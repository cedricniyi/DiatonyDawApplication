#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class StatusPanel : public juce::Component, private juce::Timer {
public:
    StatusPanel() {
        // Initialisation des labels de statut
        generationStatusLabel = std::make_unique<juce::Label>();
        playbackStatusLabel = std::make_unique<juce::Label>();

        // Configuration des labels de statut
        addAndMakeVisible(*generationStatusLabel);
        addAndMakeVisible(*playbackStatusLabel);

        generationStatusLabel->setText(juce::String::fromUTF8("Prêt à générer"), juce::dontSendNotification);
        generationStatusLabel->setJustificationType(juce::Justification::centred);
        playbackStatusLabel->setJustificationType(juce::Justification::centred);
    }
    
    ~StatusPanel() override {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override {
        // Dessiner les cadres de statut
        auto area = getLocalBounds().reduced(12); // Marge réduite de 15 à 12
        auto statusArea = area;

        // Cadre pour le statut de génération
        auto generationArea = statusArea.removeFromTop(40); // Hauteur réduite de 45 à 40
        auto generationTitleArea = generationArea.removeFromTop(18);  // Hauteur du titre réduite de 20 à 18

        // Dessiner le cadre de génération
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(generationArea.toFloat(), 5.0f, 2.0f);
        g.setColour(juce::Colours::white.withAlpha(0.05f));
        g.fillRoundedRectangle(generationArea.toFloat(), 5.0f);
        
        // Label "Statut de génération"
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(12.0f);
        g.drawText(juce::String::fromUTF8("Statut de génération"), generationTitleArea, juce::Justification::centred, false);

        statusArea.removeFromTop(8);  // Espace réduit entre les cadres, de 10 à 8

        // Cadre pour le statut de lecture
        auto playbackArea = statusArea.removeFromTop(40); // Hauteur réduite de 45 à 40
        auto playbackTitleArea = playbackArea.removeFromTop(18);  // Hauteur du titre réduite de 20 à 18

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
        auto area = getLocalBounds().reduced(12); // Marge réduite de 15 à 12
        
        // Generation label
        auto generationArea = area.removeFromTop(40); // Hauteur réduite de 45 à 40
        generationArea.removeFromTop(18);  // Espace pour le titre réduit de 20 à 18
        generationStatusLabel->setBounds(generationArea.reduced(8, 3)); // Marges réduites
        
        area.removeFromTop(8); // Espace réduit entre les zones, de 10 à 8
        
        // Playback label
        auto playbackArea = area.removeFromTop(40); // Hauteur réduite de 45 à 40
        playbackArea.removeFromTop(18);  // Espace pour le titre réduit de 20 à 18
        playbackStatusLabel->setBounds(playbackArea.reduced(8, 3)); // Marges réduites
    }
    
    // Mettre à jour l'état des textes
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
    
private:
    void timerCallback() override {
        generationStatusLabel->setText("", juce::dontSendNotification);
        stopTimer();
    }
    
    std::unique_ptr<juce::Label> generationStatusLabel;
    std::unique_ptr<juce::Label> playbackStatusLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatusPanel)
}; 