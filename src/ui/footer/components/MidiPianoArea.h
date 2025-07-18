#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"
#include "../../../utils/FontManager.h"
#include "MidiPianoContentArea.h"

//==============================================================================
class MidiPianoArea : public ColoredPanel
{
public:
    MidiPianoArea() 
        : ColoredPanel(juce::Colours::white),
          contentArea(),
          onResizeToggle(*this)
    {
        // Label "Midi Piano"
        headerLabel.setText(juce::String::fromUTF8("INFO + CONTROL"), juce::dontSendNotification);
        headerLabel.setJustificationType (juce::Justification::centredLeft);
        headerLabel.setColour (juce::Label::textColourId, juce::Colours::black);
        
        // Correction : Convertir FontOptions en Font
        auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
        headerLabel.setFont(juce::Font(fontOptions));
        
        addAndMakeVisible (headerLabel);
        addAndMakeVisible (contentArea);
    }
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        // Appliquer le padding à toute la zone
        auto area = getLocalBounds().reduced(20, 10);

        // Calculer la hauteur nécessaire pour le header
        auto headerHeight = juce::jmax(30, static_cast<int>(headerLabel.getFont().getHeight() + 5)); // hauteur du texte + marge
        auto headerArea = area.removeFromTop(headerHeight);
        
        // Positionner le label "Midi Piano" dans le header
        headerLabel.setBounds(headerArea);

        int headerBottomMargin = 8;
        area.removeFromTop (headerBottomMargin);
        
        // Le reste de l'espace va au contentArea
        contentArea.setBounds(area);
    }
    
    /** Property pour le callback qui propage automatiquement au contentArea */
    class OnResizeToggleProperty {
    public:
        OnResizeToggleProperty(MidiPianoArea& parent) : parent_(parent) {}
        
        // Opérateur d'assignation qui propage automatiquement
        OnResizeToggleProperty& operator=(const std::function<void()>& callback) {
            callback_ = callback;
            parent_.contentArea.onResizeToggle = callback;
            return *this;
        }
        
        // Opérateur de conversion pour utiliser comme std::function
        operator std::function<void()>() const { return callback_; }
        
        // Opérateur d'appel
        void operator()() const { if (callback_) callback_(); }
        
    private:
        MidiPianoArea& parent_;
        std::function<void()> callback_;
    };
    
    OnResizeToggleProperty onResizeToggle;
    
    /** Getter pour accéder au contentArea directement */
    MidiPianoContentArea& getContentArea() { return contentArea; }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Label headerLabel;
    MidiPianoContentArea contentArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPianoArea)
}; 