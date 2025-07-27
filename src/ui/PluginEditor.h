#pragma once

#include <JuceHeader.h>
#include "controller/core/PluginProcessor.h"
#include "extra/Component/SimpleToastComponent.h"
#include "extra/Component/DiatonyAlertWindow.h"
#include "MainContentComponent.h"
#include "UIStateIdentifiers.h"
#include "melatonin_inspector/melatonin_inspector.h"

// Forward declarations pour éviter les dépendances circulaires
class RootAnimator;
class FooterAnimator;
class AppController;

// ==============================================================================
// Classe utilitaire pour logger les changements du ValueTree en temps réel
class ValueTreeLogger : public juce::ValueTree::Listener
{
public:
    ValueTreeLogger(const juce::String& name) : treeName(name) {}

    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override
    {
        DBG("Logger<" << treeName << ">: Propriété changée -> "
            << property.toString() << " = "
            << tree.getProperty(property).toString());
    }
private:
    juce::String treeName;
};
//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // === DÉCOUVERTE DE SERVICE ===
    /** 
     * Permet aux composants enfants de découvrir AppController via findParentComponentOfClass
     * @return Référence vers l'instance unique d'AppController
     */
    AppController& getAppController();

private:
    // Référence au processeur
    AudioPluginAudioProcessor& audioProcessor;
    
    // État global de l'application (source de vérité unique)
    juce::ValueTree appState;
    
    // Logger pour l'état de l'UI
    ValueTreeLogger appStateLogger { "UI State" };
    
    // Contrôleur principal de l'application
    std::unique_ptr<AppController> appController;
    
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