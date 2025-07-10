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
        
        // Configuration de la flèche (en tant que Label)
        sidebarToggleLabel.setText(juce::String::fromUTF8("🗄️➡️"), juce::dontSendNotification);
        sidebarToggleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        sidebarToggleLabel.setFont(juce::Font(juce::FontOptions(20.0f)));
        sidebarToggleLabel.setJustificationType(juce::Justification::centred);
        sidebarToggleLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(sidebarToggleLabel);

        // Ajouter le listener pour le clic sur la flèche
        sidebarToggleLabel.addMouseListener(this, false);

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
        
        // Séparation verticale entre la flèche et le titre (50 pixels depuis la gauche)
        g.setColour(juce::Colour(0xFF444444));
        g.fillRect(50, 0, 1, getHeight());
    }

    //==============================================================================
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Le titre dans la première zone (largeur fixe)
        titleWidth = 300;
        auto titleZone = bounds.removeFromLeft(titleWidth);
        
        // Zone de la flèche (50 premiers pixels)
        auto toggleZone = titleZone.removeFromLeft(50);
        sidebarToggleLabel.setBounds(toggleZone);
        
        // Positionner le titre dans le reste de la zone titre
        titleLabel.setBounds(titleZone);  // Utiliser toute la zone restante
        
        // Configuration des dimensions de boutons
        int buttonWidth = 120;
        int buttonHeight = 30;
        int buttonY = (getHeight() - buttonHeight) / 2;
        
        // Boutons Diatony et Harmonizer à gauche de la seconde zone
        int buttonsX = titleWidth + 20;
        diatonyButton.setBounds(buttonsX, buttonY, buttonWidth, buttonHeight);
        harmonizerButton.setBounds(diatonyButton.getRight() + 10, buttonY, buttonWidth, buttonHeight);
        
        // Bouton Settings à droite de la seconde zone
        int settingsX = getWidth() - buttonWidth - 20;
        settingsButton.setBounds(settingsX, buttonY, buttonWidth, buttonHeight);
    }
    
    // Callbacks pour les boutons
    std::function<void()> onDiatonyClicked;
    std::function<void()> onHarmonizerClicked;
    std::function<void()> onSettingsClicked;
    std::function<void()> onToggleSidebarClicked;

    // Ajouter la gestion du clic sur la flèche
    void mouseUp(const juce::MouseEvent& event) override
    {
        if (event.eventComponent == &sidebarToggleLabel)
        {
            isFlipped = !isFlipped;
            sidebarToggleLabel.setText(isFlipped ? juce::String::fromUTF8("⬅️") : juce::String::fromUTF8("🗄️➡️"), 
                                     juce::dontSendNotification);
            // sidebarToggleLabel.setFont(juce::Font(juce::FontOptions(.0f, juce::Font::bold)));
            if (onToggleSidebarClicked) onToggleSidebarClicked();
        }
    }

private:
    juce::Label titleLabel;
    juce::Label sidebarToggleLabel;  // Remplacé TextButton par Label
    juce::TextButton diatonyButton;
    juce::TextButton harmonizerButton;
    juce::TextButton settingsButton;
    int titleWidth = 0; // Pour mémoriser la largeur du titre pour la séparation
    bool isFlipped = false;  // Ajouter cette variable pour suivre l'état
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
}; 