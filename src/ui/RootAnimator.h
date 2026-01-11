#pragma once

#include <JuceHeader.h>

class MainContentComponent;
class AnimationManager;

/**
 * @brief Gestionnaire d'animations pour le niveau root (PluginEditor).
 *
 * Prévu pour les animations qui affectent le layout global via MainContentComponent.
 * Actuellement vide (footer retiré), conservé pour usage futur.
 */
class RootAnimator
{
public:
    RootAnimator(MainContentComponent& mainContentComponent,
                 AnimationManager& animationManager);
    
    ~RootAnimator();

private:
    MainContentComponent& mainContent;
    AnimationManager& animManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RootAnimator)
};
