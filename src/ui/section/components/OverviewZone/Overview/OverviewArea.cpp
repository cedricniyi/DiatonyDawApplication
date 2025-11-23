#include "OverviewArea.h"
#include "utils/FontManager.h"

//==============================================================================
OverviewArea::OverviewArea() 
    : ColoredPanel(juce::Colour::fromString("#ffd4d4ff")),
      actionButton("+", juce::Colour::fromString("#FF4747b5"), juce::Colour::fromString("#FF4747b5"), 2.5f, 
      16.0f, FontManager::FontWeight::Bold)
{
    setAlpha(0.9f);
    
    // Configuration du label Overview
    overviewLabel.setText("OVERVIEW", juce::dontSendNotification);
    overviewLabel.setJustificationType(juce::Justification::centredLeft);
    overviewLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
    
    // Convertir FontOptions en Font
    auto fontOptions = fontManager->getSFProText(16.0f, FontManager::FontWeight::Bold);
    overviewLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(overviewLabel);
    
    // Ajouter la zone de contenu
    addAndMakeVisible(contentArea);
    
    // Ajouter le bouton à l'interface
    addAndMakeVisible(actionButton);
    
    // Connecter le bouton à la création de nouveaux panels
    actionButton.onClick = [this]() {
        contentArea.addSmallPanel();
    };
}

void OverviewArea::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
}

void OverviewArea::resized()
{
    ColoredPanel::resized();
    
    // Zone de contenu avec padding
    auto contentArea = getLocalBounds().reduced(12, 8);
    
    // 1) Mesurer la largeur exacte du texte avec GlyphArrangement
    juce::GlyphArrangement ga;
    ga.addLineOfText(overviewLabel.getFont(),
                     overviewLabel.getText(),
                     0, 0);
    auto labelWidth = static_cast<int>(std::ceil(ga.getBoundingBox(0, -1, false).getWidth()+10));
    
    // 2) Positionner le label à gauche avec sa largeur exacte
    overviewLabel.setBounds(contentArea.removeFromLeft(labelWidth));
    
    // 3) Réserver l'espace pour le bouton à droite
    auto buttonWidth = 80;
    auto buttonArea = contentArea.removeFromRight(buttonWidth);
    
    // 4) La zone de contenu prend l'espace restant au centre
    this->contentArea.setBounds(contentArea.reduced(5, 0)); // Petit padding horizontal
    
    // 5) Positionner le bouton à droite avec FlexBox
    juce::FlexBox flexContainer;
    flexContainer.flexDirection = juce::FlexBox::Direction::row;
    flexContainer.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    flexContainer.alignItems = juce::FlexBox::AlignItems::center;
    
    // Créer le FlexItem pour le bouton
    juce::FlexItem buttonItem(actionButton);
    buttonItem.width = 80.0f;
    buttonItem.height = 30.0f;
    buttonItem.flexGrow = 0.0f; // Taille fixe
    
    // Ajouter le bouton au FlexBox
    flexContainer.items.add(buttonItem);
    
    // Appliquer le layout sur l'espace du bouton
    flexContainer.performLayout(buttonArea.toFloat());
}

juce::Rectangle<int> OverviewArea::getPreferredSize() const
{
    return juce::Rectangle<int>(0, 0, 200, 75); // Hauteur augmentée à 75 pour assurer l'espace
}
