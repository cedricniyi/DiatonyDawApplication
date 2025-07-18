#pragma once

#include <JuceHeader.h>
#include "../../../utils/FontManager.h"

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
    
private:
    juce::String name;
    bool isBlack;
    bool isActive;
    juce::Label label;
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoKey)
};

//==============================================================================
// Zone de contenu pour le piano (octave)
class InteractivePianoContentArea : public juce::Component
{
public:
    InteractivePianoContentArea()
        : whiteKeyC("C"),
          whiteKeyD("D"),
          whiteKeyE("E"),
          whiteKeyF("F"),
          whiteKeyG("G"),
          whiteKeyA("A"),
          whiteKeyB("B"),
          blackKeyCSharp("C#", true),
          blackKeyDSharp("D#", true),
          blackKeyFSharp("F#", true),
          blackKeyGSharp("G#", true),
          blackKeyASharp("A#", true)
    {
        // Ajouter les touches blanches
        for (auto* key : { &whiteKeyC, &whiteKeyD, &whiteKeyE, &whiteKeyF, &whiteKeyG, &whiteKeyA, &whiteKeyB })
        {
            addAndMakeVisible(key);
        }
        
        // Ajouter les touches noires
        for (auto* key : { &blackKeyCSharp, &blackKeyDSharp, &blackKeyFSharp, &blackKeyGSharp, &blackKeyASharp })
        {
            addAndMakeVisible(key);
        }
        
        // Configuration du Grid pour les touches blanches
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;
        
        keyGrid.templateRows = { Track(Fr(1)) };  // une seule rangée
        keyGrid.templateColumns = {
            Track(Fr(1)), Track(Fr(1)), Track(Fr(1)),  // C, D, E
            Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1))  // F, G, A, B
        };
        
        keyGrid.items = {
            juce::GridItem(&whiteKeyC),
            juce::GridItem(&whiteKeyD),
            juce::GridItem(&whiteKeyE),
            juce::GridItem(&whiteKeyF),
            juce::GridItem(&whiteKeyG),
            juce::GridItem(&whiteKeyA),
            juce::GridItem(&whiteKeyB)
        };
        
        keyGrid.setGap(juce::Grid::Px(2));
    }
    
    void paint(juce::Graphics& g) override
    {
        // Fond transparent, les touches se dessinent elles-mêmes
        g.setColour(juce::Colours::transparentBlack);
        g.fillAll();
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // 1. Placer les touches blanches avec Grid
        keyGrid.performLayout(bounds);
        
        // 2. Placer les touches noires en superposition
        auto whiteKeyWidth = bounds.getWidth() / 7.0f;
        auto blackKeyWidth = whiteKeyWidth * 0.6f;
        auto blackKeyHeight = bounds.getHeight() * 0.6f;
        
        // Positions des touches noires entre les touches blanches
        // C# entre C et D
        blackKeyCSharp.setBounds(
            static_cast<int>(whiteKeyWidth * 0.7f),
            bounds.getY(),
            static_cast<int>(blackKeyWidth),
            static_cast<int>(blackKeyHeight)
        );
        
        // D# entre D et E
        blackKeyDSharp.setBounds(
            static_cast<int>(whiteKeyWidth * 1.7f),
            bounds.getY(),
            static_cast<int>(blackKeyWidth),
            static_cast<int>(blackKeyHeight)
        );
        
        // F# entre F et G
        blackKeyFSharp.setBounds(
            static_cast<int>(whiteKeyWidth * 3.7f),
            bounds.getY(),
            static_cast<int>(blackKeyWidth),
            static_cast<int>(blackKeyHeight)
        );
        
        // G# entre G et A
        blackKeyGSharp.setBounds(
            static_cast<int>(whiteKeyWidth * 4.7f),
            bounds.getY(),
            static_cast<int>(blackKeyWidth),
            static_cast<int>(blackKeyHeight)
        );
        
        // A# entre A et B
        blackKeyASharp.setBounds(
            static_cast<int>(whiteKeyWidth * 5.7f),
            bounds.getY(),
            static_cast<int>(blackKeyWidth),
            static_cast<int>(blackKeyHeight)
        );
    }
    
private:
    juce::Grid keyGrid;
    
    // Touches blanches
    PianoKey whiteKeyC, whiteKeyD, whiteKeyE, whiteKeyF, whiteKeyG, whiteKeyA, whiteKeyB;
    
    // Touches noires
    PianoKey blackKeyCSharp, blackKeyDSharp, blackKeyFSharp, blackKeyGSharp, blackKeyASharp;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractivePianoContentArea)
}; 