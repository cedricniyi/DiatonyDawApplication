#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "../PlayBack/PlaybackActionArea.h"
#include "../GenerateAndMore/GenerationButtons.h"
#include "OverviewArea.h"

// Forward declarations  
class FontManager;
class AppController;
class AudioPluginAudioProcessorEditor;

//==============================================================================
class OverviewActionArea : public ColoredPanel
{
public:
    OverviewActionArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === DÉCOUVERTE DE SERVICE ===
    void parentHierarchyChanged() override;
    
    // Accès aux composants pour les callbacks
    GenerationButtons& getGenerationButtons();
    PlaybackActionArea& getPlaybackActionArea();
    OverviewArea& getOverviewArea();
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    PlaybackActionArea playbackActionArea;
    OverviewArea overviewArea;
    GenerationButtons generationButtons;
    juce::FlexBox flexBox;
    
    // === DÉCOUVERTE DE SERVICE ===
    AppController* appController = nullptr;  // Trouvé via parentHierarchyChanged()
    void findAppController();  // Méthode pour découvrir AppController
    void connectGenerationButton();  // Méthode pour connecter le bouton au contrôleur

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
}; 