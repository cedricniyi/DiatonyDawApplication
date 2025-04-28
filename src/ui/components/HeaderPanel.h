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
    }
    
    ~HeaderPanel() override = default;

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        // Fond de la barre d'en-tête
        g.fillAll(juce::Colour(0xFF252525)); // Même couleur que la sidebar pour cohérence
        
        // Bordure en bas de la barre
        g.setColour(juce::Colour(0xFF444444));
        g.fillRect(0, getHeight() - 1, getWidth(), 1);
        
        // Séparation verticale après le titre
        g.setColour(juce::Colour(0xFF444444));
        g.fillRect(titleWidth + 10, 8, 1, getHeight() - 16);
    }

    //==============================================================================
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Le titre à gauche (largeur fixe)
        titleWidth = 250;
        titleLabel.setBounds(bounds.removeFromLeft(titleWidth).reduced(15, 0));
        
        // Espace pour la séparation
        bounds.removeFromLeft(20);
        
        // Les boutons juste après la séparation
        int buttonWidth = 120;
        int buttonHeight = 30;
        int buttonY = (getHeight() - buttonHeight) / 2;
        
        diatonyButton.setBounds(bounds.getX(), buttonY, buttonWidth, buttonHeight);
        harmonizerButton.setBounds(diatonyButton.getRight() + 10, buttonY, buttonWidth, buttonHeight);
    }
    
    // Callbacks pour les boutons
    std::function<void()> onDiatonyClicked;
    std::function<void()> onHarmonizerClicked;

private:
    juce::Label titleLabel;
    juce::TextButton diatonyButton;
    juce::TextButton harmonizerButton;
    int titleWidth = 0; // Pour mémoriser la largeur du titre pour la séparation
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
}; 