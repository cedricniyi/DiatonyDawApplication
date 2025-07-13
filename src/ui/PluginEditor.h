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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
}; 