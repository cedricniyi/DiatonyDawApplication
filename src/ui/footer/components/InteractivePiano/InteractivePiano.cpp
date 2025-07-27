#include "InteractivePiano.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "utils/FontManager.h"
#include "InteractivePianoContentArea.h"

//==============================================================================
InteractivePiano::InteractivePiano(juce::Colour color) 
    : ColoredPanel(color),
      fontManager(),
      contentArea()
{
    addAndMakeVisible(contentArea);
}

void InteractivePiano::paint(juce::Graphics& g)
{
    // Appeler le paint de ColoredPanel pour le fond
    ColoredPanel::paint(g);
}

void InteractivePiano::resized()
{
    // Appliquer un padding de 10px tout autour
    auto bounds = getLocalBounds();
    auto contentBounds = bounds.reduced(10);
    
    contentArea.setBounds(contentBounds);
} 