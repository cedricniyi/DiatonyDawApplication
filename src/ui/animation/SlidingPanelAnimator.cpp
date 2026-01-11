#include "SlidingPanelAnimator.h"
#include "AnimationManager.h"

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
    visible ? show() : hide();
}

void SlidingPanelAnimator::show()
{
    animationManager.animateValueSimple(fraction, 1.0f, expandDuration, updateCallback);
    
    juce::Timer::callAfterDelay(static_cast<int>(expandDuration * 0.5), [this]() {
        animationManager.fadeComponent(fadingComp, true, fadeDuration, nullptr);
    });
}

void SlidingPanelAnimator::hide()
{
    animationManager.fadeComponent(fadingComp, false /* fade out */, fadeDuration, [this]() {
        // rétrécir le panneau (1.0f → 0.0f) après le fade out
        animationManager.animateValueSimple(fraction, 0.0f, expandDuration, updateCallback);
    });
}
