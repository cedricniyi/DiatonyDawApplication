#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Configuration du bouton
    addAndMakeVisible(generateButton);
    generateButton.setButtonText(juce::String::fromUTF8("Générer"));
    generateButton.addListener(this);
    
    // Style personnalisé
    generateButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    generateButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    generateButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

    // juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (300, 150); // Taille adaptée au bouton
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // It's good practice to remove listeners in the destructor
    generateButton.removeListener(this);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    generateButton.setBounds(getLocalBounds().reduced(20)); // Bouton centré avec marge
}

//==============================================================================
void AudioPluginAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &generateButton) {
        processorRef.generateMidiSolution(); // Appel de la nouvelle méthode
        juce::Logger::writeToLog(juce::String::fromUTF8("MIDI généré avec succès !"));
        
        // Optionnel : notification visuelle
        generateButton.setButtonText(juce::String::fromUTF8("Générer!!"));
        startTimer(2000); // Réinitialise après 2 secondes  
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::timerCallback(){
    generateButton.setButtonText(juce::String::fromUTF8("Générer"));
    stopTimer();
}