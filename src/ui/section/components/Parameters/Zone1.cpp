#include "Zone1.h"

Zone1::Zone1()
{
}

Zone1::~Zone1()
{
}

void Zone1::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré
    g.setColour(backgroundColour);
    g.fillRect(getLocalBounds());
    
    // Dessiner la bordure
    g.setColour(borderColour);
    g.drawRect(getLocalBounds(), 1);
    
    // Dessiner le texte centré
    g.setColour(juce::Colours::black);
    g.drawText("Zone 1", getLocalBounds(), juce::Justification::centred);
}

void Zone1::resized()
{
    // Layout logic for Zone1 components will go here
}