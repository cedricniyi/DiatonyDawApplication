#include "HeaderPanel.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"
#include "ui/UIStateIdentifiers.h"
#include "utils/FontManager.h"
#include "utils/FileUtils.h"

//==============================================================================
HeaderPanel::HeaderPanel()
    : ColoredPanel (juce::Colours::white),
      dButton (juce::String::fromUTF8("D"),
               juce::Colour::fromString ("ff808080"), // Gris
               juce::Colour::fromString ("ff606060"), // Gris plus foncé au survol
               14.0f, FontManager::FontWeight::Black)
{
    // Configuration du label principal
    mainLabel.setText(juce::String::fromUTF8("DiatonyDAWPlugin"),juce::dontSendNotification);
    mainLabel.setJustificationType (juce::Justification::centredLeft);
    mainLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    
    // Application de la police via FontManager
    auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
    mainLabel.setFont(juce::Font(fontOptions));
   
    addAndMakeVisible (mainLabel);

    // Configuration du bouton D - ouvre le dossier des solutions MIDI
    dButton.setTooltip("Ouvrir le dossier des solutions MIDI");
    dButton.onClick = []() {
        FileUtils::openMidiSolutionsFolder();
    };
    addAndMakeVisible (dButton);
}

HeaderPanel::~HeaderPanel()
{
    if (appState.isValid())
        appState.removeListener(this);
}

void HeaderPanel::setAppState(juce::ValueTree& state)
{
    if (appState.isValid())
        appState.removeListener(this);
        
    appState = state;
    appState.addListener(this);
    
    // Synchronisation de l'état initial
    updateDockState();
}

void HeaderPanel::resized()
{
    auto area = getLocalBounds().reduced (20, 10);

    // Calcul de la largeur exacte du texte
    juce::GlyphArrangement ga;
    ga.addLineOfText (mainLabel.getFont(),
                      mainLabel.getText(),
                      0, 0);
    auto labelWidth = static_cast<int>(std::ceil(ga.getBoundingBox(0, -1, false).getWidth()+10));

    // Positionnement du label à gauche
    mainLabel.setBounds(area.removeFromLeft(labelWidth));

    // Positionnement du bouton D à droite (bouton carré)
    auto buttonSize = area.getHeight();
    juce::FlexBox buttonFlex;
    buttonFlex.flexDirection = juce::FlexBox::Direction::row;
    buttonFlex.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    buttonFlex.alignItems = juce::FlexBox::AlignItems::center;
    buttonFlex.items.add(juce::FlexItem(dButton).withMinWidth(buttonSize).withMinHeight(buttonSize));
    
    buttonFlex.performLayout(area);
}

void HeaderPanel::paint (juce::Graphics& g)
{
    ColoredPanel::paint (g);
}

void HeaderPanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                          const juce::Identifier& property)
{
    if (property == UIStateIdentifiers::dockVisible)
    {
        updateDockState();
    }
}

void HeaderPanel::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void HeaderPanel::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void HeaderPanel::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void HeaderPanel::valueTreeParentChanged(juce::ValueTree&) {}

void HeaderPanel::updateDockState()
{
    // Note: Cette méthode est conservée pour une éventuelle utilisation future
    // avec d'autres états UI via le ValueTree
} 