#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class DiatonyLookAndFeel : public juce::LookAndFeel_V4 {
public:
    DiatonyLookAndFeel() {
        // Couleur du fond
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xFF1F1F1F));
        
        // Couleurs des composants
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF2C7CE0));
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF333333));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::ComboBox::arrowColourId, juce::Colours::white);
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF444444));
        
        setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF333333));
        setColour(juce::TextEditor::textColourId, juce::Colours::white);
        setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFF444444));
        setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xFF2C7CE0));
        
        setColour(juce::Label::textColourId, juce::Colours::white);
        
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xFF2D2D2D));
        setColour(juce::PopupMenu::textColourId, juce::Colours::white);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xFF2C7CE0));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        auto baseColour = backgroundColour;
        
        if (shouldDrawButtonAsDown || button.getToggleState()) {
            baseColour = baseColour.darker(0.2f);
        } else if (shouldDrawButtonAsHighlighted) {
            baseColour = baseColour.brighter(0.1f);
        }
        
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, 5.0f);
        
        g.setColour(button.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds, 5.0f, 1.0f);
    }
    
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override {
        auto cornerSize = 5.0f;
        auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f, 0.5f);
        
        g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle(bounds, cornerSize);
        
        g.setColour(box.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
        
        auto arrowZone = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH).toFloat();
        auto arrowColour = box.findColour(juce::ComboBox::arrowColourId);
        
        g.setColour(arrowColour);
        
        auto arrowPath = juce::Path();
        auto arrowSize = juce::jmin(12.0f, buttonW * 0.3f);
        auto centerX = arrowZone.getCentreX();
        auto centerY = arrowZone.getCentreY();
        
        arrowPath.addTriangle(centerX - arrowSize, centerY - arrowSize * 0.5f,
                             centerX + arrowSize, centerY - arrowSize * 0.5f,
                             centerX, centerY + arrowSize * 0.5f);
        
        g.fillPath(arrowPath);
    }
}; 