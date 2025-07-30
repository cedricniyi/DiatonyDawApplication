#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "WelcomeView.h"
#include "SectionEditor.h"
#include "ModulationEditor.h"
#include "controller/ContextIdentifiers.h"

// Forward declarations
class AppController;
class AudioPluginAudioProcessorEditor;

//==============================================================================
class ProgressionArea : public ColoredPanel, public juce::ValueTree::Listener
{
public:
    ProgressionArea();
    ~ProgressionArea() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === DÉCOUVERTE DE SERVICE ===
    void parentHierarchyChanged() override;
    
    // === GESTION DE LA SÉLECTION RÉACTIVE ===
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override {}
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, 
                               int indexFromWhichChildWasRemoved) override {}
    void valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, 
                                    int oldIndex, int newIndex) override {}
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override {}

private:
    // Composants UI
    WelcomeView welcomeView;
    std::unique_ptr<SectionEditor> sectionEditor;
    std::unique_ptr<ModulationEditor> modulationEditor;
    
    // === DÉCOUVERTE DE SERVICE ===
    AppController* appController = nullptr;  // Trouvé via parentHierarchyChanged()
    juce::ValueTree selectionState; // État de sélection centralisé
    
    // Méthodes privées
    void findAppController();
    void updateContentBasedOnSelection();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionArea)
}; 