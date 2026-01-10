#include "AnimationManager.h"

JUCE_IMPLEMENT_SINGLETON(AnimationManager)

AnimationManager::AnimationManager()
{
    animatorUpdater = std::make_unique<juce::AnimatorUpdater>();
}

AnimationManager::~AnimationManager()
{
    stopAllAnimations();
    stopTimer();
    clearSingletonInstance(); // Évite assertion juce_Singleton.h
}

void AnimationManager::timerCallback()
{
    animatorUpdater->update();
    
    if (activeAnimators.empty())
        stopTimer();
}

int AnimationManager::animateValue(float& valueToAnimate,
                                   float targetValue,
                                   double durationMs,
                                   std::function<double(double)> easingFn,
                                   std::function<void()> onUpdate,
                                   std::function<void()> onComplete)
{
    int animationId = nextAnimationId++;
    float initialValue = valueToAnimate;
    
    auto builder = juce::ValueAnimatorBuilder()
        .withDurationMs(durationMs)
        .withValueChangedCallback([&valueToAnimate, initialValue, targetValue, onUpdate](float progress) {
            valueToAnimate = initialValue + (targetValue - initialValue) * progress;
            if (onUpdate) onUpdate();
        })
        .withOnCompleteCallback([this, animationId, onComplete]() {
            activeAnimators.erase(animationId);
            if (onComplete) onComplete();
        });
    
    if (easingFn)
    {
        builder = std::move(builder).withEasing([easingFn](float t) -> float {
            return static_cast<float>(easingFn(static_cast<double>(t)));
        });
    }
    
    auto animator = std::move(builder).build();
    activeAnimators[animationId] = std::make_unique<juce::Animator>(std::move(animator));
    animatorUpdater->addAnimator(*activeAnimators[animationId]);
    activeAnimators[animationId]->start();
    
    if (!isTimerRunning())
        startTimer(16); // 60 FPS
    
    return animationId;
}

int AnimationManager::animateValueSimple(float& valueToAnimate, 
                                        float targetValue, 
                                        double durationMs,
                                        std::function<void()> onUpdate)
{
    return animateValue(valueToAnimate, targetValue, durationMs, nullptr, onUpdate, nullptr);
}

int AnimationManager::animateComponentAlpha(juce::Component& component,
                                           float targetAlpha,
                                           double durationMs,
                                           std::function<double(double)> easingFn,
                                           std::function<void()> onUpdate,
                                           std::function<void()> onComplete)
{
    int animationId = nextAnimationId++;
    float initialAlpha = component.getAlpha();
    
    if (targetAlpha > 0.0f && !component.isVisible())
        component.setVisible(true);
    
    auto builder = juce::ValueAnimatorBuilder()
        .withDurationMs(durationMs)
        .withValueChangedCallback([&component, initialAlpha, targetAlpha, onUpdate](float progress) {
            float newAlpha = initialAlpha + (targetAlpha - initialAlpha) * progress;
            component.juce::Component::setAlpha(newAlpha);
            if (onUpdate) onUpdate();
        })
        .withOnCompleteCallback([this, animationId, &component, targetAlpha, onComplete]() {
            component.juce::Component::setAlpha(targetAlpha);
            if (targetAlpha <= 0.0f)
                component.setVisible(false);
            activeAnimators.erase(animationId);
            if (onComplete) onComplete();
        });
    
    if (easingFn)
    {
        builder = std::move(builder).withEasing([easingFn](float t) -> float {
            return static_cast<float>(easingFn(static_cast<double>(t)));
        });
    }
    
    auto animator = std::move(builder).build();
    activeAnimators[animationId] = std::make_unique<juce::Animator>(std::move(animator));
    animatorUpdater->addAnimator(*activeAnimators[animationId]);
    activeAnimators[animationId]->start();
    
    if (!isTimerRunning())
        startTimer(16);
    
    return animationId;
}

int AnimationManager::fadeComponent(juce::Component& component, 
                                   bool fadeIn, 
                                   double durationMs,
                                   std::function<void()> onComplete)
{
    float targetAlpha = fadeIn ? 1.0f : 0.0f;
    return animateComponentAlpha(component, targetAlpha, durationMs, nullptr, nullptr, onComplete);
}

void AnimationManager::stopAnimation(int animationId)
{
    auto it = activeAnimators.find(animationId);
    if (it != activeAnimators.end())
    {
        animatorUpdater->removeAnimator(*it->second);
        activeAnimators.erase(it);
    }
}

void AnimationManager::stopAllAnimations()
{
    for (auto& [id, animator] : activeAnimators)
        animatorUpdater->removeAnimator(*animator);
    activeAnimators.clear();
    
    if (isTimerRunning())
        stopTimer();
}

bool AnimationManager::isAnimating(int animationId) const
{
    return activeAnimators.find(animationId) != activeAnimators.end();
}

int AnimationManager::getActiveAnimationCount() const
{
    return static_cast<int>(activeAnimators.size());
}

std::function<double(double)> AnimationManager::getDefaultEasing()
{
    return [](double t) -> double {
        if (t < 0.5)
            return 2.0 * t * t;
        else
            return 1.0 - 2.0 * (1.0 - t) * (1.0 - t);
    };
} 