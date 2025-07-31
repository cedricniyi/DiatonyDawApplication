#pragma once

#include <JuceHeader.h>

// Forward declarations
class FooterPanel;
class AnimationManager;

//==============================================================================
class FooterAnimator
{
public:
    FooterAnimator(FooterPanel& panel, 
                   AnimationManager& animationManager);
    
    /** Destructeur */
    ~FooterAnimator();
    
    /** Déclenche la séquence d'animation du contenu (grid + fade uniquement) */
    void toggleSequence();
    
    /** Appelée automatiquement via callback quand FooterPanel détecte changement ValueTree */
    void setInteractivePianoVisible(bool visible);
    
private:
    void hideSequence();
    void showSequence();
    
    FooterPanel& footerPanel;
    AnimationManager& animManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterAnimator)
}; 