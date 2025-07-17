#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "ui/extra/OutlineTextButton.h"
#include "utils/FontManager.h"

//==============================================================================
class OverviewArea : public ColoredPanel
{
public:
    OverviewArea() 
        : ColoredPanel(juce::Colour::fromString("#f5f5f5ff")),
          actionButton("+", juce::Colour::fromString("#FF4747b5"), juce::Colour::fromString("#FF4747b5"), 2.5f, 
          16.0f, FontManager::FontWeight::Bold)
    {
        setAlpha(0.9f);
        
        // Ajouter le bouton à l'interface
        addAndMakeVisible(actionButton);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
        
        // Optionnel: ajouter du contenu visuel avec padding interne
        g.setColour(juce::Colours::darkblue);
        g.setFont(fontManager->getSFProText(16.0f, FontManager::FontWeight::Bold));
        
        // Appliquer un padding interne pour le texte (zone réduite car le bouton prend de la place)
        auto textArea = getLocalBounds().reduced(12, 8);
        // textArea.removeFromRight(100); // Laisser de la place pour le bouton
        g.drawText("OVERVIEW", textArea, juce::Justification::left);
    }
    
    void resized() override
    {
        ColoredPanel::resized();
        
        // Utiliser FlexBox pour le layout horizontal (1D)
        juce::FlexBox flexContainer;
        flexContainer.flexDirection = juce::FlexBox::Direction::row;
        flexContainer.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        flexContainer.alignItems = juce::FlexBox::AlignItems::center;
        
        // Zone de contenu avec padding
        auto contentArea = getLocalBounds().reduced(12, 8);
        
        // Créer un espace flexible pour pousser le bouton à droite
        juce::FlexItem spacer;
        spacer.flexGrow = 1.0f; // Prend tout l'espace disponible
        
        // Créer le FlexItem pour le bouton
        juce::FlexItem buttonItem(actionButton);
        buttonItem.width = 80.0f;
        buttonItem.height = 30.0f;
        buttonItem.flexGrow = 0.0f; // Taille fixe
        
        // Ajouter les éléments au FlexBox
        flexContainer.items.add(spacer);
        flexContainer.items.add(buttonItem);
        
        // Appliquer le layout
        flexContainer.performLayout(contentArea.toFloat());
    }
    
    // Méthode pour obtenir la taille préférée
    juce::Rectangle<int> getPreferredSize() const
    {
        return juce::Rectangle<int>(0, 0, 200, 55); // Largeur flexible, hauteur fixe
    }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    OutlineTextButton actionButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewArea)
}; 