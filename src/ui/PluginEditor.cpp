#include "PluginEditor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"
#include "utils/FontManager.h"
#include "ui/animation/AnimationManager.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      headerPanel(),
      sectionPanel(),
      footerPanel()
{
    // Les fonts sont maintenant initialisées automatiquement via SharedResourcePointer
    // Plus besoin d'appeler initializeFonts() manuellement
    
    constrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
    constrainer->setSizeLimits(1125, 562, 1694, 847);      // tailles min/max
    constrainer->setFixedAspectRatio(1500.0 / 750.0);     // ratio constant
    setConstrainer(constrainer.get());                    // lie le constrainer à l'éditeur
    
    setSize(1500, 750);

    // Configuration du callback pour l'animation moderne
    footerPanel.onRequestResize = [this] { startFlexAnimation(); };
    
    DBG("PluginEditor: Callback onRequestResize assigné au FooterPanel"); // Debug

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

void AudioPluginAudioProcessorEditor::startFlexAnimation()
{
    DBG("PluginEditor: startFlexAnimation() appelée !"); // Debug
    
    // Valeur de départ et cible (toggle)
    float targetFooterFlex = (footerFlex < 30.0f) ? 40.0f : 25.0f;
    
    DBG("PluginEditor: footerFlex actuel = " << footerFlex << ", target = " << targetFooterFlex);
    
    // Utiliser l'AnimationManager modernisé avec l'API juce_animation
    AnimationManager::getInstance()->animateValueSimple(
        footerFlex,
        targetFooterFlex,
        300.0, // durée en ms
        [this]() { 
            // Callback appelé à chaque frame pour redessiner le layout
            resized(); 
        }
    );
}