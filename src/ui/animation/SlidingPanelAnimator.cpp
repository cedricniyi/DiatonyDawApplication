#include "SlidingPanelAnimator.h"
#include "AnimationManager.h"

//==============================================================================
SlidingPanelAnimator::SlidingPanelAnimator(float& fractionRef,
                                           juce::Component& fadingComponent,
                                           std::function<void()> onUpdate,
                                           AnimationManager& animManager,
                                           double expandDurationMs,
                                           double fadeDurationMs)
    : fraction(fractionRef),
      fadingComp(fadingComponent),
      updateCallback(std::move(onUpdate)),
      animationManager(animManager),
      expandDuration(expandDurationMs),
      fadeDuration(fadeDurationMs)
{
}

void SlidingPanelAnimator::setVisible(bool visible)
{
    if (visible)
        show();
    else
        hide();
}

void SlidingPanelAnimator::show()
{
    // Étape 1 : Animation pour élargir le panneau (0.0f → 1.0f)
    animationManager.animateValueSimple(
        fraction,
        1.0f,
        expandDuration,
        updateCallback
    );
    
    // Étape 2 : Petite pause puis fade in du contenu
    juce::Timer::callAfterDelay(static_cast<int>(expandDuration * 0.5), [this]() {
        animationManager.fadeComponent(
            fadingComp,
            true,  // fade in
            fadeDuration,
            nullptr
        );
    });
}

void SlidingPanelAnimator::hide()
{
    // Étape 1 : Fade out du contenu
    animationManager.fadeComponent(
        fadingComp,
        false,  // fade out
        fadeDuration,
        [this]() {
            // Étape 2 : Une fois le fade terminé, rétrécir le panneau (1.0f → 0.0f)
            animationManager.animateValueSimple(
                fraction,
                0.0f,
                expandDuration,
                updateCallback
            );
        }
    );
}


