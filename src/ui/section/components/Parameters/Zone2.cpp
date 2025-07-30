#include "Zone2.h"

Zone2::Zone2()
{
}

Zone2::~Zone2()
{
}

void Zone2::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré
    g.setColour(backgroundColour);
    g.fillRect(getLocalBounds());
    
    // Dessiner la bordure
    g.setColour(borderColour);
    g.drawRect(getLocalBounds(), 1);
    
    // Dessiner le texte centré
    g.setColour(juce::Colours::black);
    g.drawText("Zone 2", getLocalBounds(), juce::Justification::centred);
}

void Zone2::resized()
{
    // Layout logic for Zone2 components will go here
}