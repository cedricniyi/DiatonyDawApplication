#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../utils/DiatonyConstants.h"

class TonalityPanel : public juce::Component {
public:
    TonalityPanel() {
        // Initialisation des composants
        tonalityLabel = std::make_unique<juce::Label>();
        tonalityComboBox = std::make_unique<juce::ComboBox>();
        modeLabel = std::make_unique<juce::Label>();
        modeComboBox = std::make_unique<juce::ComboBox>();
        
        // Configuration du label de tonalité
        addAndMakeVisible(*tonalityLabel);
        tonalityLabel->setText(juce::String::fromUTF8("Tonalité:"), juce::dontSendNotification);
        tonalityLabel->setJustificationType(juce::Justification::right);
        
        // Configuration du combobox de tonalité
        addAndMakeVisible(*tonalityComboBox);
        setupTonalityComboBox();
        
        // Configuration du label de mode
        addAndMakeVisible(*modeLabel);
        modeLabel->setText(juce::String::fromUTF8("Mode:"), juce::dontSendNotification);
        modeLabel->setJustificationType(juce::Justification::right);
        
        // Configuration du combobox de mode
        addAndMakeVisible(*modeComboBox);
        setupModeComboBox();
    }
    
    ~TonalityPanel() override = default;
    
    void paint(juce::Graphics& g) override {
        // Optionnel : dessin spécifique pour ce panneau
    }
    
    void resized() override {
        auto area = getLocalBounds().reduced(15);
        
        // Configuration de la zone tonalité
        auto tonalityArea = area.removeFromTop(35);
        tonalityLabel->setBounds(tonalityArea.removeFromLeft(90));
        tonalityComboBox->setBounds(tonalityArea);

        area.removeFromTop(15);  // Espace entre les composants

        // Zone du mode
        auto modeArea = area.removeFromTop(35);
        modeLabel->setBounds(modeArea.removeFromLeft(90));
        modeComboBox->setBounds(modeArea);
    }
    
    // Callbacks pour les changements de tonalité et de mode
    std::function<void(int)> onTonalityChanged;
    std::function<void(bool)> onModeChanged;
    
private:
    void setupTonalityComboBox() {
        // Remplir le combobox avec les tonalités disponibles
        for (size_t i = 0; i < DiatonyConstants::NOTES.size(); ++i) {
            const auto& note = DiatonyConstants::NOTES[i];
            tonalityComboBox->addItem(note.name, static_cast<int>(i + 1));
        }
        
        tonalityComboBox->setSelectedId(1); // C par défaut
        
        tonalityComboBox->onChange = [this]() {
            int selectedIndex = tonalityComboBox->getSelectedId() - 1;
            int noteValue = DiatonyConstants::NOTES[selectedIndex].value;
            
            if (onTonalityChanged) {
                onTonalityChanged(noteValue);
            }
        };
    }
    
    void setupModeComboBox() {
        modeComboBox->addItem("Majeur", 1);
        modeComboBox->addItem("Mineur", 2);
        modeComboBox->setSelectedId(1); // Majeur par défaut
        
        modeComboBox->onChange = [this]() {
            bool isMajor = modeComboBox->getSelectedId() == 1;
            
            if (onModeChanged) {
                onModeChanged(isMajor);
            }
        };
    }

    std::unique_ptr<juce::Label> tonalityLabel;
    std::unique_ptr<juce::ComboBox> tonalityComboBox;
    std::unique_ptr<juce::Label> modeLabel;
    std::unique_ptr<juce::ComboBox> modeComboBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TonalityPanel)
}; 