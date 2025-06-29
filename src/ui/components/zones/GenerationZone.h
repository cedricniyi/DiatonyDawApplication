#pragma once

#include <JuceHeader.h>
#include "../extra/OutlineTextButton.h"

// Couleurs personnalisées
const juce::Colour emeraudGreen (0xFF2ECC71);
const juce::Colour lightGrey (0xFFCCCCCC);
const juce::Colour lightBlue (0xFF6495ED);  // Bleu clair (cornflower blue)
const juce::Colour orangeColor (0xFFFF9933); // Orange pour le bouton de drag

//==============================================================================
/**
 * Zone de génération avec un titre à gauche et une zone de contenu à droite.
 */
class GenerationZone : public juce::Component
{
public:
    // Paramètres configurables facilement modifiables ici
    const int titleWidth = 150;             // Largeur de la zone du titre
    const int buttonLeftPadding = 35;       // Espace entre la séparation verticale et le bouton
    const int buttonHeight = 40;            // Hauteur du bouton
    const int buttonSpacing = 35;           // Espacement entre les boutons
    const int buttonTextPadding = 20;       // Padding horizontal supplémentaire pour le texte du bouton

    GenerationZone()
    {
        setOpaque(false); // Nécessaire pour voir le fond dessiné dans paint()

        // Initialisation du Label
        generationLabel = std::make_unique<juce::Label>("generationLabel", juce::String::fromUTF8("Solution"));
        generationLabel->setFont(juce::FontOptions(20.0f, juce::Font::bold));
        generationLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        generationLabel->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*generationLabel);

        // Création et initialisation des boutons avec une fonction helper
        createButton(generateSolutionButton, "Generate a solution", emeraudGreen, true);
        createButton(playSolutionButton, juce::String::fromUTF8("▶ Play solution"), lightBlue, false);
        createButton(dragMidiButton, juce::String::fromUTF8("Drag solution as MIDI track ↗"), orangeColor, false);
        createButton(midiToMusicXMLButton, "Midi to MusicXML", lightGrey, false);
    }
    
    // Méthode helper pour créer un bouton (réduit la duplication de code)
    void createButton(std::unique_ptr<extra::OutlineTextButton>& button, 
                     const juce::String& text, const juce::Colour& color, bool enabled)
    {
        button = std::make_unique<extra::OutlineTextButton>(text, color, color, 2.0f);
        button->setEnabled(enabled);
        addAndMakeVisible(*button);
    }
    
    // Méthode pour activer/désactiver les boutons selon que la solution est générée ou non
    void setSolutionGenerated(bool isGenerated)
    {
        playSolutionButton->setEnabled(isGenerated);
        dragMidiButton->setEnabled(isGenerated);
        midiToMusicXMLButton->setEnabled(isGenerated);
    }

    ~GenerationZone() override = default;

    void paint(juce::Graphics& g) override
    {
        const float cornerSize = 8.0f;
        const int zoneBorderThickness = 1;
        const int padding = 5;

        auto bounds = getLocalBounds().reduced(padding);

        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, zoneBorderThickness);

        // Dessiner la ligne de séparation verticale
        if (bounds.getWidth() > titleWidth)
            g.drawLine(padding + titleWidth, padding, padding + titleWidth, getHeight() - padding, 1.0f);
    }

    // Version simplifiée qui évite les variables intermédiaires inutiles
    int calculateButtonWidth(juce::Button* button)
    {
        if (!button) return 100;
        
        juce::Font font(juce::FontOptions(14.0f, juce::Font::bold));
        juce::GlyphArrangement glyphs;
        glyphs.addLineOfText(font, button->getButtonText(), 0.0f, 0.0f);
        
        // Retourner directement le résultat du calcul
        return static_cast<int>(glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true).getWidth()) 
                + buttonTextPadding * 2;
    }

    void resized() override
    {
        const int padding = 5;
        auto contentBounds = getLocalBounds().reduced(padding);
        auto titleZone = contentBounds.removeFromLeft(titleWidth);
        
        // Positionner le label
        if (generationLabel)
            generationLabel->setBounds(titleZone);
        
        // Utiliser une approche plus fonctionnelle pour positionner les boutons
        int x = contentBounds.getX() + buttonLeftPadding;
        int y = contentBounds.getCentreY() - buttonHeight / 2;
        
        // Lambda pour positionner chaque bouton et mettre à jour x
        auto positionButton = [&](auto& btn) {
            if (btn) {
                int width = calculateButtonWidth(btn.get());
                btn->setBounds(x, y, width, buttonHeight);
                x += width + buttonSpacing;
            }
        };
        
        // Positionner tous les boutons en séquence
        positionButton(generateSolutionButton);
        positionButton(playSolutionButton);
        positionButton(dragMidiButton);
        positionButton(midiToMusicXMLButton);
    }

private:
    std::unique_ptr<juce::Label> generationLabel;
    std::unique_ptr<extra::OutlineTextButton> generateSolutionButton;
    std::unique_ptr<extra::OutlineTextButton> playSolutionButton;
    std::unique_ptr<extra::OutlineTextButton> dragMidiButton;
    std::unique_ptr<extra::OutlineTextButton> midiToMusicXMLButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationZone)
}; 