#pragma once

#include <JuceHeader.h>

//==============================================================================
class HeaderPanel : public juce::Component
{
public:
    HeaderPanel()
    {
        // Configuration du titre
        titleLabel.setText("Diatony DAW Application", juce::dontSendNotification);
        titleLabel.setFont(juce::Font(juce::FontOptions(20.0f, juce::Font::bold)));
        titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        titleLabel.setJustificationType(juce::Justification::centred); // Centre le texte
        addAndMakeVisible(titleLabel);
        
        // Configuration du bouton Diatony (orange avec coins arrondis)
        diatonyButton.setButtonText("Diatony");
        diatonyButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFF28C28)); // Orange
        diatonyButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        diatonyButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        diatonyButton.onClick = [this]() {
            if (onDiatonyClicked) onDiatonyClicked();
        };
        addAndMakeVisible(diatonyButton);
        
        // Configuration du bouton Harmonizer (vert)
        harmonizerButton.setButtonText("Harmonizer");
        harmonizerButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF4CAF50)); // Vert
        harmonizerButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        harmonizerButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        harmonizerButton.onClick = [this]() {
            if (onHarmonizerClicked) onHarmonizerClicked();
        };
        addAndMakeVisible(harmonizerButton);
        
        // Configuration du bouton Settings (gris)
        settingsButton.setButtonText("Settings");
        settingsButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF555555)); // Gris
        settingsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        settingsButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        settingsButton.onClick = [this]() {
            if (onSettingsClicked) onSettingsClicked();
        };
        addAndMakeVisible(settingsButton);
    }
    
    ~HeaderPanel() override = default;

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        
        // Zone du titre (partie gauche) - couleur un peu plus foncée
        auto titleZone = bounds.removeFromLeft(titleWidth);
        g.setColour(juce::Colour(0xFF1E1E1E)); // Un peu plus foncé
        g.fillRect(titleZone);
        
        // Zone des boutons (partie droite)
        g.setColour(juce::Colour(0xFF252525));
        g.fillRect(bounds);
        
        // Bordure en bas de la barre
        g.setColour(juce::Colour(0xFF444444));
        g.fillRect(0, getHeight() - 1, getWidth(), 1);
        
        // Séparation verticale après la zone du titre
        g.setColour(juce::Colour(0xFF444444));
        g.fillRect(titleWidth, 0, 1, getHeight());
    }

    //==============================================================================
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Le titre dans la première zone (largeur fixe)
        titleWidth = 300; // Zone plus large pour le titre centré
        auto titleZone = bounds.removeFromLeft(titleWidth);
        titleLabel.setBounds(titleZone.reduced(10, 0)); // Réduction des marges pour centrer
        
        // Configuration des dimensions de boutons
        int buttonWidth = 120;
        int buttonHeight = 30;
        int buttonY = (getHeight() - buttonHeight) / 2;
        
        // Boutons Diatony et Harmonizer à gauche de la seconde zone
        int buttonsX = titleWidth + 20; // 20 pixels de marge après la séparation
        diatonyButton.setBounds(buttonsX, buttonY, buttonWidth, buttonHeight);
        harmonizerButton.setBounds(diatonyButton.getRight() + 10, buttonY, buttonWidth, buttonHeight);
        
        // Bouton Settings à droite de la seconde zone
        int settingsX = getWidth() - buttonWidth - 20; // 20 pixels de marge à droite
        settingsButton.setBounds(settingsX, buttonY, buttonWidth, buttonHeight);
    }
    
    // Callbacks pour les boutons
    std::function<void()> onDiatonyClicked;
    std::function<void()> onHarmonizerClicked;
    std::function<void()> onSettingsClicked;

private:
    juce::Label titleLabel;
    juce::TextButton diatonyButton;
    juce::TextButton harmonizerButton;
    juce::TextButton settingsButton;
    int titleWidth = 0; // Pour mémoriser la largeur du titre pour la séparation
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
}; 