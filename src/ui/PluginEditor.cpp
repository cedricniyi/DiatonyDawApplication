#include "PluginEditor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"
#include "utils/FontManager.h"
#include "ui/animation/AnimationManager.h"
#include "ui/RootAnimator.h"
#include "ui/footer/animator/FooterAnimator.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      appState(UIStateIdentifiers::APP_STATE)  // Initialiser le ValueTree
{   
    // =================================================================================
    // 1. Initialiser l'état global de l'application
    appState.setProperty(UIStateIdentifiers::footerExpanded, false, nullptr);
    appState.setProperty(UIStateIdentifiers::dockVisible, false, nullptr);
    appState.setProperty(UIStateIdentifiers::interactivePianoVisible, false, nullptr);
    
    DBG("PluginEditor: État global initialisé - footer:" << 
        (static_cast<bool>(appState.getProperty(UIStateIdentifiers::footerExpanded, false)) ? "true" : "false") <<
        ", dock:" << (static_cast<bool>(appState.getProperty(UIStateIdentifiers::dockVisible, false)) ? "true" : "false") <<
        ", piano:" << (static_cast<bool>(appState.getProperty(UIStateIdentifiers::interactivePianoVisible, false)) ? "true" : "false"));

    // =================================================================================
    // 2. Créer et configurer le composant principal
    mainContent = std::make_unique<MainContentComponent>();
    mainContent->setAppState(appState);
    addAndMakeVisible(*mainContent);
    
    // =================================================================================
    // 3. Contrainte de taille
    constrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
    constrainer->setSizeLimits(1300, 569, 1694, 847);      // tailles min/max
    constrainer->setFixedAspectRatio(1500.0 / 750.0);     // ratio constant
    setConstrainer(constrainer.get());                    // lie le constrainer à l'éditeur
    
    setSize(1500, 750);

    // =================================================================================
    // 4. Créer les Animators pour gérer les animations à différents niveaux
    
    // RootAnimator : gère les animations de niveau élevé (flex du layout global)
    rootAnimator = std::make_unique<RootAnimator>(
        *mainContent,                 // Référence vers MainContentComponent
        *AnimationManager::getInstance()
    );
    
    // FooterAnimator : gère les animations internes au FooterPanel (grid + fade)
    footerAnimator = std::make_unique<FooterAnimator>(
        mainContent->getFooterPanel(), 
        *AnimationManager::getInstance()
    );
    
    DBG("PluginEditor: Architecture hiérarchique activée - RootAnimator (flex) + FooterAnimator (contenu)");

    // =================================================================================
    // 5. Le MainContentComponent est déjà rendu visible plus haut
    DBG("PluginEditor: Configuration terminée avec MainContentComponent");

}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{   
    // La peinture est maintenant déléguée au MainContentComponent
    // Pas besoin de peindre ici car MainContentComponent remplit toute la zone
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Redimensionner le MainContentComponent pour qu'il occupe toute la zone
    if (mainContent)
        mainContent->setBounds(getLocalBounds());
}