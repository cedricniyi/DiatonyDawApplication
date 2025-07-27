#include "InteractivePianoContentArea.h"
#include "PianoKey.h"

//==============================================================================
InteractivePianoContentArea::InteractivePianoContentArea()
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
    setupWhiteKeys();
    setupBlackKeys();
    setupKeyGrid();
}

void InteractivePianoContentArea::paint(juce::Graphics& g)
{
    // Fond transparent, les touches se dessinent elles-mêmes
    g.setColour(juce::Colours::transparentBlack);
    g.fillAll();
}

void InteractivePianoContentArea::resized()
{
    auto bounds = getLocalBounds();
    
    // 1. Placer les touches blanches avec Grid
    keyGrid.performLayout(bounds);
    
    // 2. Placer les touches noires en superposition
    layoutBlackKeys(bounds);
}

void InteractivePianoContentArea::setupWhiteKeys()
{
    for (auto* key : { &whiteKeyC, &whiteKeyD, &whiteKeyE, &whiteKeyF, &whiteKeyG, &whiteKeyA, &whiteKeyB })
    {
        addAndMakeVisible(key);
    }
}

void InteractivePianoContentArea::setupBlackKeys()
{
    for (auto* key : { &blackKeyCSharp, &blackKeyDSharp, &blackKeyFSharp, &blackKeyGSharp, &blackKeyASharp })
    {
        addAndMakeVisible(key);
    }
}

void InteractivePianoContentArea::setupKeyGrid()
{
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

void InteractivePianoContentArea::layoutBlackKeys(const juce::Rectangle<int>& bounds)
{
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