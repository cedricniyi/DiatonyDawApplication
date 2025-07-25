#pragma once

#include <JuceHeader.h>
#include "../controller/core/PluginProcessor.h"
#include "extra/SimpleToastComponent.h"
#include "extra/DiatonyAlertWindow.h"
#include "MainContentComponent.h"
#include "UIStateIdentifiers.h"
#include "melatonin_inspector/melatonin_inspector.h"

// Forward declarations pour éviter les dépendances circulaires
class RootAnimator;
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
    
    // État global de l'application (source de vérité unique)
    juce::ValueTree appState;
    
    // Melatonin Inspector pour déboguer l'interface
    melatonin::Inspector inspector { *this, false };

    
    // Composants de notification
    std::unique_ptr<SimpleToastComponent> toast;
    
    // Composant principal qui gère le contenu et le layout
    std::unique_ptr<MainContentComponent> mainContent;
    
    // Animators pour gérer les animations à différents niveaux
    std::unique_ptr<RootAnimator> rootAnimator;      // Animations niveau root (flex)
    std::unique_ptr<FooterAnimator> footerAnimator;  // Animations niveau footer (grid + fade)

    // Constrainer pour la taille
    std::unique_ptr<juce::ComponentBoundsConstrainer> constrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
}; 