#include "PluginEditor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"
#include "utils/FontManager.h"
#include "ui/animation/AnimationManager.h"
#include "ui/footer/animator/FooterAnimator.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      appState(UIStateIdentifiers::APP_STATE),  // NOUVEAU: Initialiser le ValueTree ici
      headerPanel(),
      sectionPanel(),
      footerPanel()
{   
    // =================================================================================
    // 1. Initialiser l'état global de l'application (maintenant dans PluginEditor)
    appState.setProperty(UIStateIdentifiers::footerExpanded, false, nullptr);
    
    DBG("PluginEditor: État global initialisé - footer:" << 
        (static_cast<bool>(appState.getProperty(UIStateIdentifiers::footerExpanded, false)) ? "true" : "false"));

    // =================================================================================
    // 2. Initialiser le contrôleur UI avec référence au ValueTree
    uiController = std::make_unique<RootUIController>(audioProcessor, appState);
    
    // =================================================================================
    // 3. Contrainte de taille
    constrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
    constrainer->setSizeLimits(1300, 569, 1694, 847);      // tailles min/max
    constrainer->setFixedAspectRatio(1500.0 / 750.0);     // ratio constant
    setConstrainer(constrainer.get());                    // lie le constrainer à l'éditeur
    
    setSize(1500, 750);

    // =================================================================================
    // 4. Lier les panels au contrôleur (injection de dépendances)
    uiController->initializePanels(headerPanel, sectionPanel, footerPanel);

    // =================================================================================
    // 5. Créer le FooterAnimator avec les références nécessaires pour l'animation flex
    footerAnimator = std::make_unique<FooterAnimator>(
        footerPanel, 
        *AnimationManager::getInstance(),
        footerFlex,                    // Référence vers la variable flex
        [this]() { resized(); }        // Callback pour redessiner le layout
    );
    
    DBG("PluginEditor: Architecture mise à jour - PluginEditor possède l'état, Controller l'orchestre");

    // =================================================================================
    // 6. Rendre les panels visibles
    addAndMakeVisible (headerPanel);
    addAndMakeVisible (sectionPanel);
    addAndMakeVisible (footerPanel);

}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{   
    // Dégradé linéaire à 135 degrés
    auto bounds = getLocalBounds().toFloat();
    
    // Calculer les points de départ et d'arrivée pour un angle de 135°
    // 135° = direction vers le bas-droite
    auto center = bounds.getCentre();
    auto diagonal = std::sqrt(bounds.getWidth() * bounds.getWidth() + bounds.getHeight() * bounds.getHeight()) * 0.5f;
    
    // Convertir 135° en radians et calculer les points
    auto angleRad = juce::MathConstants<float>::pi * 135.0f / 180.0f;
    auto startPoint = center - juce::Point<float>(std::cos(angleRad), std::sin(angleRad)) * diagonal;
    auto endPoint = center + juce::Point<float>(std::cos(angleRad), std::sin(angleRad)) * diagonal;
    
    // Créer le dégradé
    juce::ColourGradient gradient(
        juce::Colour::fromString("fff5f7fa"), startPoint,  // #f5f7fa à 0%
        juce::Colour::fromString("ffc3cfe2"), endPoint,    // #c3cfe2 à 100%
        false // pas radial
    );
    
    g.setGradientFill(gradient);
    g.fillRect(bounds);
}

void AudioPluginAudioProcessorEditor::resized()
{
    int padding = 8;
    auto content = getLocalBounds().reduced(padding);

    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;  // empile verticalement

    fb.items = {
        juce::FlexItem (headerPanel).withFlex (headerFlex).withMargin ({ 0, 0, 4, 0 }),
        juce::FlexItem (sectionPanel).withFlex (sectionFlex).withMargin ({ 4, 0, 4, 0 }),
        juce::FlexItem (footerPanel).withFlex (footerFlex).withMargin ({ 4, 0, 0, 0 })
    };

    fb.performLayout (content);
}