#include "PluginEditor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"
#include "utils/FontManager.h"

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

    // Configuration du callback pour l'animation
    footerPanel.onRequestResize = [this] { startFlexAnimation(); };
    
    // Initialiser le timer d'animation
    animationTimer = std::make_unique<AnimationTimer>(*this);

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
    // Valeur de départ et cible (toggle)
    targetFooterFlex = (footerFlex < 30.0f) ? 40.0f : 25.0f;
    
    // Enregistrer le temps de début
    animationStart = juce::Time::getMillisecondCounterHiRes();
    
    // Démarrer le timer d'animation (60 FPS)
    animationTimer->startTimer(16);
}

void AudioPluginAudioProcessorEditor::updateFlexAnimation()
{
    double currentTime = juce::Time::getMillisecondCounterHiRes();
    double elapsed = currentTime - animationStart;
    
    if (elapsed >= animationDuration)
    {
        // Animation terminée
        footerFlex = targetFooterFlex;
        animationTimer->stopTimer();
        resized();
        return;
    }
    
    // Calcul du progrès (0.0 à 1.0)
    double progress = elapsed / animationDuration;
    
    // Easing ease-in-out quadratic
    if (progress < 0.5)
        progress = 2.0 * progress * progress;
    else
        progress = -1.0 + (4.0 - 2.0 * progress) * progress;
    
    // Interpolarion entre la valeur actuelle et la cible
    float startValue = (targetFooterFlex == 40.0f) ? 25.0f : 40.0f;
    footerFlex = startValue + (targetFooterFlex - startValue) * static_cast<float>(progress);
    
    // Mettre à jour le layout
    resized();
}