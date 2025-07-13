#pragma once

#include <JuceHeader.h>
#include "../core/PluginProcessor.h"
#include "extra/SimpleToastComponent.h"
#include "extra/DiatonyAlertWindow.h"
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "footer/FooterPanel.h"
#include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Référence au processeur
    AudioPluginAudioProcessor& audioProcessor;
    
    // Melatonin Inspector pour déboguer l'interface
    melatonin::Inspector inspector { *this, false };

    
    // Composants de notification
    std::unique_ptr<SimpleToastComponent> toast;
    
    // Éléments de base pour commencer
    juce::Label titleLabel;
    juce::TextButton testButton;

    // 3 Parties de l'interface 
    HeaderPanel headerPanel;
    SectionPanel sectionPanel;
    FooterPanel footerPanel;

    // Constrainer pour la taille
    std::unique_ptr<juce::ComponentBoundsConstrainer> constrainer;
    
    // Animation pour les valeurs flex
    void startFlexAnimation();
    float headerFlex  = 7.5f;
    float sectionFlex = 47.5f;
    float footerFlex  = 25.0f;
    
    // Timer pour l'animation
    class AnimationTimer : public juce::Timer
    {
    public:
        AnimationTimer(AudioPluginAudioProcessorEditor& editor) : owner(editor) {}
        
        void timerCallback() override
        {
            owner.updateFlexAnimation();
        }
        
    private:
        AudioPluginAudioProcessorEditor& owner;
    };
    
    void updateFlexAnimation();
    std::unique_ptr<AnimationTimer> animationTimer;
    
    // Variables d'animation
    float targetFooterFlex = 25.0f;
    float animationSpeed = 0.1f;
    double animationStart = 0.0;
    double animationDuration = 300.0; // ms

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
}; 