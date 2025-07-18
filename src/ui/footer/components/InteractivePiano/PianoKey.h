#pragma once

#include <JuceHeader.h>
#include "../../../../utils/FontManager.h"

//==============================================================================
// Classe pour une touche de piano
class PianoKey : public juce::Component
{
public:
    PianoKey(const juce::String& noteName, bool isBlackKey = false)
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
    
    void paint(juce::Graphics& g) override
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
    
    void resized() override
    {
        label.setBounds(getLocalBounds().reduced(2));
    }
    
    void mouseDown(const juce::MouseEvent&) override
    {
        setActive(true);
    }
    
    void mouseUp(const juce::MouseEvent&) override
    {
        setActive(false);
    }
    
    void setActive(bool active)
    {
        isActive = active;
        repaint();
    }
    
    const juce::String& getNoteName() const { return name; }
    bool isBlackKey() const { return isBlack; }
    
private:
    juce::String name;
    bool isBlack;
    bool isActive;
    juce::Label label;
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoKey)
}; 