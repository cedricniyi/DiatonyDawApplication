#include "PluginEditor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      topPanel(),
      middlePanel(),
      bottomPanel()
{

    
    // Configurer la fenêtre
        
    constrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
    constrainer->setSizeLimits(1125, 562, 1694, 847);      // tailles min/max
    constrainer->setFixedAspectRatio(1500.0 / 750.0);     // ratio constant
    setConstrainer(constrainer.get());                    // lie le constrainer à l’éditeur
    
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