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
    // const int buttonWidth = 200;         // Supprimé: largeur calculée dynamiquement
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

        // Initialisation du premier bouton (vert) - Toujours activé
        generateSolutionButton = std::make_unique<extra::OutlineTextButton>(
            "Generate a solution", // Texte du bouton
            emeraudGreen,          // Couleur du texte
            emeraudGreen,          // Couleur du contour
            2.0f                   // Épaisseur du contour
        );
        addAndMakeVisible(*generateSolutionButton);
        
        // Initialisation du deuxième bouton (bleu clair) avec symbole ▶ - Désactivé
        playSolutionButton = std::make_unique<extra::OutlineTextButton>(
            juce::String::fromUTF8("▶ Play solution"),      // Texte du bouton avec symbole unicode de lecture
            lightBlue,                                      // Couleur du texte
            lightBlue,                                      // Couleur du contour
            2.0f                                            // Épaisseur du contour
        );
        playSolutionButton->setEnabled(false); // Désactivé par défaut
        addAndMakeVisible(*playSolutionButton);
        
        // Initialisation du bouton Drag (orange) - Désactivé
        dragMidiButton = std::make_unique<extra::OutlineTextButton>(
            juce::String::fromUTF8("Drag solution as MIDI track ↗"),   // Texte du bouton avec flèche
            orangeColor,                        // Couleur du texte
            orangeColor,                        // Couleur du contour
            2.0f                                // Épaisseur du contour
        );
        dragMidiButton->setEnabled(false); // Désactivé par défaut
        addAndMakeVisible(*dragMidiButton);
                
        // Initialisation du quatrième bouton (gris) - Désactivé
        midiToMusicXMLButton = std::make_unique<extra::OutlineTextButton>(
            "Midi to MusicXML",    // Texte du bouton
            lightGrey,             // Couleur du texte
            lightGrey,             // Couleur du contour
            2.0f                   // Épaisseur du contour
        );
        midiToMusicXMLButton->setEnabled(false); // Désactivé par défaut
        addAndMakeVisible(*midiToMusicXMLButton);
    }
    
    // Méthode pour activer/désactiver les boutons selon que la solution est générée ou non
    void setSolutionGenerated(bool isGenerated)
    {
        // Si une solution est générée, activer tous les boutons sauf "Generate"
        // Si aucune solution n'est générée, seul le bouton "Generate" est actif
        playSolutionButton->setEnabled(isGenerated);
        dragMidiButton->setEnabled(isGenerated);
        midiToMusicXMLButton->setEnabled(isGenerated);
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

    // Fonction utilitaire pour calculer la largeur nécessaire pour un bouton
    int calculateButtonWidth(juce::Button* button)
    {
        if (button == nullptr)
            return 100; // Valeur par défaut

        // Utiliser une police standard pour les calculs
        juce::Font buttonFont(juce::FontOptions(14.0f, juce::Font::bold));
        
        // Utiliser GlyphArrangement pour calculer la largeur (méthode non dépréciée)
        juce::GlyphArrangement glyphs;
        glyphs.addLineOfText(buttonFont, button->getButtonText(), 0.0f, 0.0f);
        auto textBounds = glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true);
        
        // Ajouter un padding horizontal pour éviter que le texte ne soit trop serré
        return static_cast<int>(textBounds.getWidth()) + buttonTextPadding * 2;
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

        // Calculer la largeur de chaque bouton en fonction de son texte
        int width1 = calculateButtonWidth(generateSolutionButton.get());
        int width2 = calculateButtonWidth(playSolutionButton.get());
        int width3 = calculateButtonWidth(dragMidiButton.get());
        int width4 = calculateButtonWidth(midiToMusicXMLButton.get());
        
        // Position du premier bouton (Generate solution)
        int firstButtonX = contentZone.getX() + buttonLeftPadding;
        int buttonsY = contentZone.getCentreY() - buttonHeight / 2;
        
        // Position du deuxième bouton (Play solution)
        int secondButtonX = firstButtonX + width1 + buttonSpacing;
        
        // Position du troisième bouton (Drag solution)
        int thirdButtonX = secondButtonX + width2 + buttonSpacing;
        
        // Position du quatrième bouton (Midi to MusicXML)
        int fourthButtonX = thirdButtonX + width3 + buttonSpacing;
        
        if (generateSolutionButton)
        {
            generateSolutionButton->setBounds(firstButtonX, 
                                              buttonsY,
                                              width1,
                                              buttonHeight);
        }
        
        if (playSolutionButton)
        {
            playSolutionButton->setBounds(secondButtonX,
                                           buttonsY,
                                           width2,
                                           buttonHeight);
        }
        
        if (dragMidiButton)
        {
            dragMidiButton->setBounds(thirdButtonX,
                                      buttonsY,
                                      width3,
                                      buttonHeight);
        }
        
        if (midiToMusicXMLButton)
        {
            midiToMusicXMLButton->setBounds(fourthButtonX,
                                            buttonsY,
                                            width4,
                                            buttonHeight);
        }
    }

private:
    std::unique_ptr<juce::Label> generationLabel;
    std::unique_ptr<extra::OutlineTextButton> generateSolutionButton;  // Premier bouton (vert)
    std::unique_ptr<extra::OutlineTextButton> playSolutionButton;      // Deuxième bouton (bleu) avec symbole ▶
    std::unique_ptr<extra::OutlineTextButton> dragMidiButton;          // Troisième bouton (orange) pour drag
    std::unique_ptr<extra::OutlineTextButton> midiToMusicXMLButton;    // Quatrième bouton (gris)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationZone)
}; 