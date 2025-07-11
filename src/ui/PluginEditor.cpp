#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Appliquer le thème personnalisé
    setLookAndFeel(&diatonyLookAndFeel);
    
    // Configurer la fenêtre
    setSize(800, 600);
    
    // Créer le composant de toast
    toast = std::make_unique<SimpleToastComponent>();
    addAndMakeVisible(*toast);
    
    // Configurer le titre
    titleLabel.setText("Diatony DAW Application - Nouvelle Interface", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);
    
    // Bouton de test
    testButton.setButtonText("Test Toast & Alert");
    testButton.onClick = [this]() {
        showToast("Toast de test !");
        
        // Montrer l'alerte après un petit délai
        juce::Timer::callAfterDelay(1000, [this]() {
            showAlert("Test Alert", "Interface redémarrée avec succès ! Les composants réutilisables sont disponibles.");
        });
    };
    addAndMakeVisible(testButton);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fond de l'application
    g.fillAll(diatonyLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
    
    // Cadre décoratif
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.drawRoundedRectangle(getLocalBounds().reduced(20).toFloat(), 10.0f, 2.0f);
    
    // Message d'accueil
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(16.0f);
    g.drawText("Interface redémarrée avec succès !", 
               getLocalBounds().reduced(40).removeFromBottom(100).removeFromTop(40), 
               juce::Justification::centred, true);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(40);
    
    // Toast en plein écran pour les notifications
    if (toast) toast->setBounds(getLocalBounds());
    
    // Titre en haut
    titleLabel.setBounds(bounds.removeFromTop(60));
    
    // Espace
    bounds.removeFromTop(20);
    
    // Bouton de test au centre
    auto buttonBounds = bounds.reduced(200, 150);
    testButton.setBounds(buttonBounds.removeFromTop(40));
}

//==============================================================================
void AudioPluginAudioProcessorEditor::showToast(const juce::String& message)
{
    if (toast) toast->showMessage(message);
}

void AudioPluginAudioProcessorEditor::showAlert(const juce::String& title, const juce::String& message)
{
    DiatonyAlertWindow::show(title, message);
} 