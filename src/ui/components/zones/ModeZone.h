#pragma once

#include <JuceHeader.h>
#include "BaseZone.h"

//==============================================================================
/**
 * Zone pour la sélection du mode
 */
class ModeZone : public BaseZone
{
public:
    ModeZone()
    : BaseZone(juce::String::fromUTF8("Mode")), currentMode("Major") // Défaut à Major
    {
        majorButton = std::make_unique<juce::TextButton>("Major");
        minorButton = std::make_unique<juce::TextButton>("Minor");

        configureButton(majorButton.get(), "Major");
        configureButton(minorButton.get(), "Minor");

        addAndMakeVisible(*majorButton);
        addAndMakeVisible(*minorButton);

        setSelectedModeButton(currentMode); 
    }
    
    ~ModeZone() override = default;

    // Callback pour notifier le parent du changement de mode
    std::function<void(const juce::String& mode)> onModeSelected;

protected:
    void paintContent(juce::Graphics& g, const juce::Rectangle<int>& contentBounds) override
    {
        // Plus de texte d'espace réservé, les boutons sont le contenu.
    }
    
    void resizeContent(const juce::Rectangle<int>& initialContentBounds) override
    {
        if (initialContentBounds.isEmpty()) return;

        const int internalPadding = 15; 
        auto contentBounds = initialContentBounds.reduced(internalPadding);

        if (contentBounds.isEmpty()) return;

        const int buttonHeight = (contentBounds.getHeight() - internalPadding) / 2; // Hauteur pour chaque bouton, avec un petit espace entre eux
        const int buttonWidth = contentBounds.getWidth() > 100 ? contentBounds.getWidth() - 2*internalPadding : contentBounds.getWidth(); // Largeur des boutons, avec padding latéral si assez large

        int currentY = contentBounds.getY();
        int buttonX = contentBounds.getX() + (contentBounds.getWidth() - buttonWidth) / 2; // Centrer horizontalement

        majorButton->setBounds(buttonX, currentY, buttonWidth, buttonHeight);
        currentY += buttonHeight + internalPadding; // Ajoute de l'espace entre les boutons
        minorButton->setBounds(buttonX, currentY, buttonWidth, buttonHeight);
        
        // S'assurer que les boutons sont redessinés avec le bon état après un redimensionnement
        setSelectedModeButton(currentMode);
    }

private:
    std::unique_ptr<juce::TextButton> majorButton;
    std::unique_ptr<juce::TextButton> minorButton;
    juce::String currentMode;

    // Fonction d'aide pour configurer l'apparence d'un bouton
    void updateButtonAppearance(juce::TextButton* button,
                                bool isSelected,
                                const juce::Colour& selectedBg,
                                const juce::Colour& selectedText,
                                const juce::Colour& unselectedBg,
                                const juce::Colour& unselectedText)
    {
        const auto& currentBgColour = isSelected ? selectedBg : unselectedBg;
        const auto& currentTextColour = isSelected ? selectedText : unselectedText;

        button->setColour(juce::TextButton::buttonColourId, currentBgColour);
        button->setColour(juce::TextButton::textColourOffId, currentTextColour);
        button->setColour(juce::TextButton::buttonOnColourId, currentBgColour); // On applique la même logique pour l'état ON
        button->setColour(juce::TextButton::textColourOnId, currentTextColour);   // pour la cohérence
        button->setToggleState(isSelected, juce::dontSendNotification);
    }

    void configureButton(juce::TextButton* button, const juce::String& modeName)
    {
        button->onClick = [this, modeName]() {
            handleModeButtonClick(modeName);
        };
        button->setWantsKeyboardFocus(false);
        // Les couleurs sont gérées par setSelectedModeButton
    }

    void handleModeButtonClick(const juce::String& modeName)
    {
        if (currentMode == modeName) return; // Pas de changement si on clique sur le mode déjà sélectionné

        DBG(juce::String::fromUTF8("Mode sélectionné: ") + modeName);
        currentMode = modeName;
        setSelectedModeButton(modeName);
        
        if (onModeSelected)
        {
            onModeSelected(currentMode);
        }
    }
    
    void setSelectedModeButton(const juce::String& selectedMode)
    {
        // Style pour le bouton sélectionné
        const auto selectedButtonColour = juce::Colour::fromRGB(135, 206, 235); // Bleu clair doux
        const auto selectedTextColour = juce::Colours::white;

        // Style pour le bouton non sélectionné
        const auto unselectedButtonColour = juce::Colours::ivory;
        const auto unselectedTextColour = juce::Colours::black;

        updateButtonAppearance(majorButton.get(),
                               selectedMode == "Major",
                               selectedButtonColour, selectedTextColour,
                               unselectedButtonColour, unselectedTextColour);

        updateButtonAppearance(minorButton.get(),
                               selectedMode == "Minor",
                               selectedButtonColour, selectedTextColour,
                               unselectedButtonColour, unselectedTextColour);
        
        majorButton->repaint();
        minorButton->repaint();
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeZone)
}; 