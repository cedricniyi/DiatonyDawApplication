#pragma once

#include <JuceHeader.h>
#include "WelcomeView.h"
#include "SectionEditor.h"
#include "ModulationEditor.h"
#include "controller/ContextIdentifiers.h"

class AppController;
class AudioPluginAudioProcessorEditor;

/** @brief Zone d'édition affichant WelcomeView, SectionEditor ou ModulationEditor selon la sélection. */
class ProgressionArea : public juce::Component, public juce::ValueTree::Listener
{
public:
    ProgressionArea();
    ~ProgressionArea() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** @brief Découvre AppController via la hiérarchie des composants. */
    void parentHierarchyChanged() override;
    
    // ValueTree::Listener
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, 
                               int indexFromWhichChildWasRemoved) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}

private:
    WelcomeView welcomeView;
    std::unique_ptr<SectionEditor> sectionEditor;
    std::unique_ptr<ModulationEditor> modulationEditor;
    
    AppController* appController = nullptr;
    juce::ValueTree selectionState;
    juce::ValueTree modelState;
    
    static constexpr float cornerRadius = 8.0f;
    static constexpr int borderThickness = 1;
    static constexpr int contentPadding = 10;
    
    void findAppController();
    void updateContentBasedOnSelection();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionArea)
};
