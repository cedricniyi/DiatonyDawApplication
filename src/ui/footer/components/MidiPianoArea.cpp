#include "MidiPianoArea.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "utils/FontManager.h"
#include "MidiPianoContentArea.h"

//==============================================================================
MidiPianoArea::MidiPianoArea() 
    : ColoredPanel(juce::Colours::white),
      contentArea()
{
    // Label "Midi Piano"
    headerLabel.setText(juce::String::fromUTF8("INFO + CONTROL"), juce::dontSendNotification);
    headerLabel.setJustificationType (juce::Justification::centredLeft);
    headerLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    
    // Correction : Convertir FontOptions en Font
    auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
    headerLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible (headerLabel);
    addAndMakeVisible (contentArea);
}

void MidiPianoArea::paint(juce::Graphics& g)
{
    ColoredPanel::paint(g);
}

void MidiPianoArea::resized()
{
    // Appliquer le padding à toute la zone
    auto area = getLocalBounds().reduced(20, 10);

    // Calculer la hauteur nécessaire pour le header
    auto headerHeight = juce::jmax(30, static_cast<int>(headerLabel.getFont().getHeight() + 5)); // hauteur du texte + marge
    auto headerArea = area.removeFromTop(headerHeight);
    
    // Positionner le label "Midi Piano" dans le header
    headerLabel.setBounds(headerArea);

    int headerBottomMargin = 8;
    area.removeFromTop (headerBottomMargin);
    
    // Le reste de l'espace va au contentArea
    contentArea.setBounds(area);
}

MidiPianoContentArea& MidiPianoArea::getContentArea()
{
    return contentArea;
} 