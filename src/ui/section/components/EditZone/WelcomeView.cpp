#include "WelcomeView.h"
#include "utils/FontManager.h"

//==============================================================================
WelcomeView::WelcomeView()
{
    // 1. Icône emoji 🎼
    welcomeIconLabel.setText(juce::String::fromUTF8("🎼"), juce::dontSendNotification);
    welcomeIconLabel.setJustificationType(juce::Justification::centred);
    welcomeIconLabel.setFont(juce::Font(fontManager->getDisplayFont(72.0f)));
    welcomeIconLabel.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    // welcomeIconLabel.setColour(juce::Label::backgroundColourId, juce::Colours::lightblue.withAlpha(0.3f)); // Debug
    addAndMakeVisible(welcomeIconLabel);
    
    // 2. Titre principal
    welcomeTitleLabel.setText("Welcome to DiatonyDAW (v0.1.3)", juce::dontSendNotification);
    welcomeTitleLabel.setJustificationType(juce::Justification::centred);
    welcomeTitleLabel.setFont(juce::Font(fontManager->getBoldFont(24.0f)));
    welcomeTitleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff2c3e50));
    addAndMakeVisible(welcomeTitleLabel);
    // welcomeTitleLabel.setColour(juce::Label::backgroundColourId, juce::Colours::lightgreen.withAlpha(0.3f)); // Debug
    
    // 3. Description
    welcomeDescLabel.setText("Start by creating your first section to be able to submit your piece to get a solution.", 
                            juce::dontSendNotification);
    welcomeDescLabel.setJustificationType(juce::Justification::centred);
    welcomeDescLabel.setFont(juce::Font(fontManager->getTextFont(16.0f)));
    welcomeDescLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    // welcomeDescLabel.setColour(juce::Label::backgroundColourId, juce::Colours::lightyellow.withAlpha(0.3f)); // Debug
    addAndMakeVisible(welcomeDescLabel);
}

void WelcomeView::paint(juce::Graphics& g)
{
    // Laisser le fond du composant parent visible (transparent)
    // Pas de fillAll() pour que le fond parent soit visible
}

void WelcomeView::resized()
{
    auto bounds = getLocalBounds();
    
    // === MESURER LES TAILLES EXACTES ===
    
    // Icône
    juce::GlyphArrangement iconGA;
    iconGA.addLineOfText(welcomeIconLabel.getFont(), welcomeIconLabel.getText(), 0, 0);
    auto iconBounds = iconGA.getBoundingBox(0, -1, false);
    int iconWidth = static_cast<int>(std::ceil(iconBounds.getWidth())) + 10;
    int iconHeight = static_cast<int>(std::ceil(iconBounds.getHeight())) + 10;
    
    // Titre
    juce::GlyphArrangement titleGA;
    titleGA.addLineOfText(welcomeTitleLabel.getFont(), welcomeTitleLabel.getText(), 0, 0);
    auto titleBounds = titleGA.getBoundingBox(0, -1, false);
    int titleWidth = static_cast<int>(std::ceil(titleBounds.getWidth())) + 20;
    int titleHeight = static_cast<int>(std::ceil(titleBounds.getHeight())) + 10;
    
    // Description
    juce::GlyphArrangement descGA;
    int maxDescWidth = std::min(bounds.getWidth() - 40, 600);
    descGA.addFittedText(welcomeDescLabel.getFont(), 
                        welcomeDescLabel.getText(),
                        0, 0, 
                        static_cast<float>(maxDescWidth), 
                        200.0f,
                        juce::Justification::centred,
                        3);
    auto descBounds = descGA.getBoundingBox(0, -1, false);
    int descWidth = static_cast<int>(std::ceil(descBounds.getWidth() + 20));
    int descHeight = static_cast<int>(std::ceil(descBounds.getHeight() + 10));
    descWidth = std::min(descWidth, maxDescWidth);
    
    // === FLEXBOX LAYOUT POUR CENTRER LE GROUPE ===
    
    juce::FlexBox welcomeFlexBox;
    welcomeFlexBox.flexDirection = juce::FlexBox::Direction::column;
    welcomeFlexBox.justifyContent = juce::FlexBox::JustifyContent::center; // Centre verticalement
    welcomeFlexBox.alignItems = juce::FlexBox::AlignItems::center;        // Centre horizontalement
    
    // Créer les FlexItems avec les tailles mesurées
    juce::FlexItem iconItem(welcomeIconLabel);
    iconItem.width = static_cast<float>(iconWidth);
    iconItem.height = static_cast<float>(iconHeight);
    iconItem.margin = juce::FlexItem::Margin(0, 0, 6, 0); // marge bottom 12px
    
    juce::FlexItem titleItem(welcomeTitleLabel);
    titleItem.width = static_cast<float>(titleWidth);
    titleItem.height = static_cast<float>(titleHeight);
    titleItem.margin = juce::FlexItem::Margin(0, 0, 4, 0); // marge bottom 8px
    
    juce::FlexItem descItem(welcomeDescLabel);
    descItem.width = static_cast<float>(descWidth);
    descItem.height = static_cast<float>(descHeight);
    
    // Ajouter les items au FlexBox
    welcomeFlexBox.items.add(iconItem);
    welcomeFlexBox.items.add(titleItem);
    welcomeFlexBox.items.add(descItem);
    
    // Appliquer le layout sur toute la zone
    welcomeFlexBox.performLayout(bounds.toFloat());
} 