#include "Zone4.h"

Zone4::Zone4()
{
}

Zone4::~Zone4()
{
}

void Zone4::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré
    g.setColour(backgroundColour);
    g.fillRect(getLocalBounds());
    
    // Dessiner la bordure
    g.setColour(borderColour);
    g.drawRect(getLocalBounds(), 1);
    
    // Dessiner le texte centré
    g.setColour(juce::Colours::black);
    g.drawText("Zone 4", getLocalBounds(), juce::Justification::centred);
}

void Zone4::resized()
{
    // Layout logic for Zone4 components will go here
}