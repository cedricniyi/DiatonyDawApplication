#pragma once

#include <JuceHeader.h>
#include "controller/core/PluginProcessor.h"
#include "extra/Component/SimpleToastComponent.h"
#include "extra/Component/DiatonyAlertWindow.h"
#include "MainContentComponent.h"
#include "UIStateIdentifiers.h"
#include "melatonin_inspector/melatonin_inspector.h"

#if DEBUG
    #include "debug/ValueTreeLogger.h"
#endif

class SlidingPanelAnimator;
class AppController;

/** @brief Éditeur principal du plugin audio, gère l'UI et les animations. */
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    /** @brief Permet aux composants enfants de découvrir AppController. */
    AppController& getAppController();

private:
    AudioPluginAudioProcessor& audioProcessor;
    
    // État global de l'application (source de vérité unique)
    juce::ValueTree appState;
    
    #if DEBUG
        // Loggers pour le débogage (uniquement en mode DEBUG)
        ValueTreeLogger appStateLogger { "UI State" };
        ValueTreeLogger pieceStateLogger { "Piece Model State" };
        ValueTreeLogger selectionStateLogger { "Selection Context State" };
    #endif
    
    std::unique_ptr<AppController> appController;
    
    // Melatonin Inspector pour déboguer l'interface
    melatonin::Inspector inspector { *this, false };
    
    std::unique_ptr<SimpleToastComponent> toast;
    std::unique_ptr<MainContentComponent> mainContent;
    
    std::unique_ptr<SlidingPanelAnimator> historyAnimator;

    // Constrainer pour la taille
    std::unique_ptr<juce::ComponentBoundsConstrainer> constrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
