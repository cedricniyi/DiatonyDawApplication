#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Configuration des boutons
    addAndMakeVisible(incrementButton);
    incrementButton.setButtonText("+");
    incrementButton.addListener(this);
    
    addAndMakeVisible(decrementButton);
    decrementButton.setButtonText("-");
    decrementButton.addListener(this);
    
    // Configuration du label
    addAndMakeVisible(counterLabel);
    counterLabel.setText("0", juce::dontSendNotification);
    counterLabel.setFont(juce::Font(juce::FontOptions().withHeight(24.0f)));
    counterLabel.setJustificationType(juce::Justification::centred);

    // juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // It's good practice to remove listeners in the destructor
    incrementButton.removeListener(this);
    decrementButton.removeListener(this);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Positionnement des éléments
    auto area = getLocalBounds().reduced(20);
    
    auto buttonRow = area.removeFromTop(50);
    decrementButton.setBounds(buttonRow.removeFromLeft(100));
    incrementButton.setBounds(buttonRow.removeFromRight(100));
    
    counterLabel.setBounds(area);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &incrementButton) {
        counter++;
    }
    else if (button == &decrementButton) {
        counter--;
    }
    
    counterLabel.setText(juce::String(counter), juce::dontSendNotification);
}