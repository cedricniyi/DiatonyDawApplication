#pragma once

#include <JuceHeader.h>
#include "../core/PluginProcessor.h"
#include "extra/SimpleToastComponent.h"
#include "extra/DiatonyAlertWindow.h"
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "footer/FooterPanel.h"
#include "controller/RootUIController.h"
#include "melatonin_inspector/melatonin_inspector.h"

// Forward declaration pour éviter les dépendances circulaires
class FooterAnimator;

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
    
    // NOUVEAU: État global de l'application (source de vérité unique)
    juce::ValueTree appState;
    
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

    // MODIFIÉ: Contrôleur principal qui orchestre l'état (ne le crée plus)
    std::unique_ptr<RootUIController> uiController;
    
    // Animator pour gérer les animations du footer
    std::unique_ptr<FooterAnimator> footerAnimator;

    // Constrainer pour la taille
    std::unique_ptr<juce::ComponentBoundsConstrainer> constrainer;
    
    // Valeurs flex (animation maintenant gérée par FooterAnimator)
    float headerFlex  = 7.5f;
    float sectionFlex = 57.5f;
    float footerFlex  = 15.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
}; 