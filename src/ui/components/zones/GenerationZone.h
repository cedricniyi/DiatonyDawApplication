#pragma once

#include <JuceHeader.h>
#include "../extra/OutlineTextButton.h"

// Couleur vert émeraude personnalisée
const juce::Colour emeraudGreen (0xFF2ECC71);

//==============================================================================
/**
 * Zone de génération avec un titre à gauche et une zone de contenu à droite.
 */
class GenerationZone : public juce::Component
{
public:
    GenerationZone()
    {
        setOpaque(false); // Nécessaire pour voir le fond dessiné dans paint()

        // Initialisation du Label
        generationLabel = std::make_unique<juce::Label>("generationLabel", juce::String::fromUTF8("Solution"));
        generationLabel->setFont(juce::FontOptions(20.0f, juce::Font::bold));
        generationLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        generationLabel->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*generationLabel);

        // Initialisation du bouton personnalisé en utilisant la classe modulaire
        generateSolutionButton = std::make_unique<extra::OutlineTextButton>(
            "Generate a solution", // Texte du bouton
            emeraudGreen,          // Couleur du texte
            emeraudGreen,          // Couleur du contour
            2.0f                   // Épaisseur du contour
        );
        addAndMakeVisible(*generateSolutionButton);
    }

    ~GenerationZone() override = default;

    void paint(juce::Graphics& g) override
    {
        const float cornerSize = 8.0f;
        const int zoneBorderThickness = 1;
        const int padding = 5; // Padding unique autour du contenu

        auto bounds = getLocalBounds().reduced(padding);

        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, zoneBorderThickness);

        if (bounds.getWidth() > titleWidth)
        {
            g.setColour(juce::Colours::grey.withAlpha(0.3f));
            g.drawLine(padding + titleWidth, padding,
                       padding + titleWidth, getHeight() - padding,
                       1.0f);
        }
    }

    void resized() override
    {
        const int padding = 5;
        auto contentBounds = getLocalBounds().reduced(padding);
        auto titleZone = contentBounds.removeFromLeft(titleWidth);
        auto contentZone = contentBounds; // Espace restant après la titleZone

        if (generationLabel)
        {
            generationLabel->setBounds(titleZone);
        }

        if (generateSolutionButton)
        {
            const int buttonWidth = 200;
            const int buttonHeight = 40;
            const int buttonPaddingFromSeparator = 10; // Espace entre la ligne de séparation et le bouton

            // Placer le bouton à gauche de la contentZone, centré verticalement
            generateSolutionButton->setBounds(contentZone.getX() + buttonPaddingFromSeparator,
                                              contentZone.getCentreY() - buttonHeight / 2,
                                              buttonWidth,
                                              buttonHeight);
        }
    }

private:
    std::unique_ptr<juce::Label> generationLabel;
    std::unique_ptr<extra::OutlineTextButton> generateSolutionButton; // Bouton personnalisé depuis le module extra
    const int titleWidth = 150;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationZone)
}; 