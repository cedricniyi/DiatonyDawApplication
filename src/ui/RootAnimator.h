#pragma once

#include <JuceHeader.h>

// Forward declarations pour éviter les dépendances circulaires
class MainContentComponent;
class AnimationManager;

//==============================================================================
/**
 * @brief Gestionnaire d'animations pour le niveau root (PluginEditor)
 * 
 * Le RootAnimator s'occupe des animations qui affectent le layout global
 * via MainContentComponent, notamment les valeurs flex des panels principaux.
 * Il utilise le même mécanisme de callback que FooterAnimator.
 */
class RootAnimator
{
public:
    RootAnimator(MainContentComponent& mainContentComponent,
                 AnimationManager& animationManager);
    
    ~RootAnimator();

    /** Appelée automatiquement via callback quand MainContentComponent détecte changement ValueTree */
    void handleLayoutAnimationNeeded();

private:
    void animateFooterFlex();
    
    MainContentComponent& mainContent;
    AnimationManager& animManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RootAnimator)
}; 