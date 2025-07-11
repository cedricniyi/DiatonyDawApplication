#include "PluginEditor.h"
#include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      topPanel(),
      middlePanel(),
      bottomPanel()
{
    // Appliquer le thème personnalisé
    setLookAndFeel(&diatonyLookAndFeel);
    
    // Configurer la fenêtre
    
    setResizeLimits(750, 375, 1500, 750);
    setResizable(true,  false);
    setSize(1500, 750);

    addAndMakeVisible (topPanel);
    addAndMakeVisible (middlePanel);
    addAndMakeVisible (bottomPanel);

}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{   
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto content = getLocalBounds();

    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;  // empile verticalement

    fb.items = {
        juce::FlexItem (topPanel   ).withFlex (7.5f),
        juce::FlexItem (middlePanel).withFlex (47.5f),
        juce::FlexItem (bottomPanel).withFlex (25.0f)
    };

    fb.performLayout (content);
}