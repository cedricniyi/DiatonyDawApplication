#pragma once

#include <JuceHeader.h>
#include "ui/extra/StyledButton.h"
#include "ui/extra/ColoredPanel.h"
#include "utils/FontManager.h"

//==============================================================================
class GenerationButtons : public ColoredPanel
{
public:
    GenerationButtons()
        : ColoredPanel(juce::Colour::fromString("#ffddebf0")),
          generateButton("Generate", 
                        juce::Colour::fromString("#ff22c55e"), // Vert
                        juce::Colour::fromString("#ff16a34a"), // Vert plus foncé pour highlight
                        14.0f,
                        FontManager::FontWeight::Medium),
          moreButton("...", 
                    juce::Colour::fromString("#ff6b7280"), // Gris
                    juce::Colour::fromString("#ff4b5563"), // Gris plus foncé pour highlight
                    14.0f,
                    FontManager::FontWeight::Medium)
    {
        // Ajouter les boutons au composant
        addAndMakeVisible(generateButton);
        addAndMakeVisible(moreButton);
        
        // Configuration du FlexBox
        flexBox.flexDirection = juce::FlexBox::Direction::row;
        flexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        flexBox.alignItems = juce::FlexBox::AlignItems::center;
        flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
        
        // Ajouter les éléments au FlexBox avec espacement
        flexBox.items.add(juce::FlexItem(generateButton)
            .withWidth(80.0f)  // Rectangulaire
            .withHeight(32.0f)
            .withMargin(juce::FlexItem::Margin(0, 12, 0, 0))); // Plus d'espacement entre les boutons
            
        flexBox.items.add(juce::FlexItem(moreButton)
            .withWidth(32.0f)  // Carré
            .withHeight(32.0f)
            .withMargin(juce::FlexItem::Margin(0, 0, 0, 0)));
    }
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Marges adaptatives pour éviter les problèmes aux petites tailles
        int horizontalMargin = juce::jmin(4, bounds.getWidth() / 15);
        int verticalMargin = juce::jmin(6, bounds.getHeight() / 8);
        
        auto area = bounds.reduced(horizontalMargin, verticalMargin);
        flexBox.performLayout(area.toFloat());
    }
    
    // Méthodes pour accéder aux boutons si nécessaire
    StyledButton& getGenerateButton() { return generateButton; }
    StyledButton& getMoreButton() { return moreButton; }
    
    // Méthode pour calculer la taille préférée du composant
    juce::Rectangle<int> getPreferredSize() const
    {
        // Calcul basé sur le nombre de boutons et leurs dimensions
        int generateButtonWidth = 80;
        int moreButtonWidth = 32;
        int buttonSpacing = 12; // Espacement augmenté
        int padding = 16; // 8px de chaque côté
        
        int totalWidth = generateButtonWidth + moreButtonWidth + buttonSpacing + padding;
        int totalHeight = 32 + 12; // hauteur bouton + padding vertical
        
        return juce::Rectangle<int>(0, 0, totalWidth, totalHeight);
    }
    
private:
    StyledButton generateButton;
    StyledButton moreButton;
    juce::FlexBox flexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationButtons)
}; 