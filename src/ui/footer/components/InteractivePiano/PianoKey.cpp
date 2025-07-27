#include "PianoKey.h" 
#include "../../../../utils/FontManager.h"

//==============================================================================
PianoKey::PianoKey(const juce::String& noteName, bool isBlackKey)
    : name(noteName), isBlack(isBlackKey), isActive(false), fontManager()
{
    label.setText(noteName, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centredBottom);
    label.setColour(juce::Label::textColourId, isBlack ? juce::Colours::white : juce::Colours::darkgrey);
    
    // Utiliser SF Pro Display Bold
    auto fontOptions = fontManager->getSFProDisplay(10.0f, FontManager::FontWeight::Bold);
    label.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(label);
}

void PianoKey::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Créer un path avec des coins arrondis seulement en bas
    juce::Path keyPath;
    keyPath.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 
                               6.0f, 6.0f,    // cornerSizeX, cornerSizeY
                               false, false,   // curveTopLeft, curveTopRight
                               true, true);    // curveBottomLeft, curveBottomRight
    
    if (isBlack)
    {
        g.setColour(isActive ? juce::Colours::darkred : juce::Colours::black);
        g.fillPath(keyPath);
        
        g.setColour(juce::Colours::white);
    }
    else
    {
        g.setColour(isActive ? juce::Colours::lightblue : juce::Colours::white);
        g.fillPath(keyPath);
        
        g.setColour(juce::Colours::darkgrey);
    }
}

void PianoKey::resized()
{
    label.setBounds(getLocalBounds().reduced(2));
}

void PianoKey::mouseDown(const juce::MouseEvent&)
{
    setActive(true);
}

void PianoKey::mouseUp(const juce::MouseEvent&)
{
    setActive(false);
}

void PianoKey::setActive(bool active)
{
    isActive = active;
    repaint();
}

const juce::String& PianoKey::getNoteName() const
{
    return name;
}

bool PianoKey::isBlackKey() const
{
    return isBlack;
} 