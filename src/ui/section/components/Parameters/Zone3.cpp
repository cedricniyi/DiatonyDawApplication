#include "Zone3.h"

Zone3::Zone3()
{
}

Zone3::~Zone3()
{
}

void Zone3::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré
    g.setColour(backgroundColour);
    g.fillRect(getLocalBounds());
    
    // Dessiner la bordure
    g.setColour(borderColour);
    g.drawRect(getLocalBounds(), 1);
    
    // Dessiner le texte centré
    g.setColour(juce::Colours::black);
    g.drawText("Zone 3", getLocalBounds(), juce::Justification::centred);
}

void Zone3::resized()
{
    // Layout logic for Zone3 components will go here
}