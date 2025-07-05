#pragma once

#include <JuceHeader.h>
#include "BaseZone.h"
#include "../../../model/Progression.h"
#include "../../../utils/DiatonyConstants.h"

//==============================================================================
/**
 * Zone pour la création et l'édition de progression d'accords
 * Version initiale avec progression hardcodée pour tester le flux
 */
class ProgressionZone : public BaseZone
{
public:
    ProgressionZone()
    : BaseZone(juce::String::fromUTF8("Progression"))
    {
        // Créer un bouton pour charger une progression de test
        testProgressionButton = std::make_unique<juce::TextButton>(juce::String::fromUTF8("Load Test Progression"));
        testProgressionButton->setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
        testProgressionButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        
        testProgressionButton->onClick = [this]() {
            loadTestProgression();
        };
        
        addAndMakeVisible(*testProgressionButton);
        
        // Label pour afficher la progression actuelle
        progressionLabel = std::make_unique<juce::Label>();
        progressionLabel->setText(juce::String::fromUTF8("No progression loaded"), juce::dontSendNotification);
        progressionLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        progressionLabel->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*progressionLabel);
    }
    
    ~ProgressionZone() override = default;
    
    // Callback pour notifier les changements
    std::function<void(const Progression&)> onProgressionChanged;

protected:
    void paintContent(juce::Graphics& g, const juce::Rectangle<int>& contentBounds) override
    {
        // Pas besoin de dessiner, les composants gèrent l'affichage
    }
    
    void resizeContent(const juce::Rectangle<int>& contentBounds) override
    {
        if (contentBounds.isEmpty()) return;
        
        const int padding = 10;
        auto area = contentBounds.reduced(padding);
        
        // Bouton en haut
        auto buttonArea = area.removeFromTop(35);
        testProgressionButton->setBounds(buttonArea);
        
        area.removeFromTop(10); // Espacement
        
        // Label en bas
        progressionLabel->setBounds(area);
    }

private:
    std::unique_ptr<juce::TextButton> testProgressionButton;
    std::unique_ptr<juce::Label> progressionLabel;
    
    void loadTestProgression()
    {
        // Progression de test identique à main.cpp
        Progression testProgression;
        
        // Accords : I VI II V5da V I
        std::vector<int> chords = {
            FIRST_DEGREE, 
            SIXTH_DEGREE, 
            SECOND_DEGREE, 
            FIFTH_DEGREE_APPOGIATURA, 
            FIFTH_DEGREE, 
            FIRST_DEGREE
        };
        
        // États : Fund Fund Fund 2nd Fund Fund
        std::vector<int> states = {
            FUNDAMENTAL_STATE, 
            FUNDAMENTAL_STATE, 
            FUNDAMENTAL_STATE, 
            SECOND_INVERSION, 
            FUNDAMENTAL_STATE, 
            FUNDAMENTAL_STATE
        };
        
        // Qualités par défaut (seront calculées par le solveur)
        std::vector<int> qualities = {}; // Vide = qualités par défaut
        
        testProgression.setChords(chords);
        testProgression.setStates(states);
        testProgression.setQualities(qualities);
        
        // Mettre à jour l'affichage
        progressionLabel->setText(
            juce::String::fromUTF8("Test Progression: I VI II V5da V I (") + 
            juce::String(testProgression.size()) + juce::String::fromUTF8(" chords)"), 
            juce::dontSendNotification
        );
        
        // Notifier le changement
        if (onProgressionChanged) {
            onProgressionChanged(testProgression);
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionZone)
}; 