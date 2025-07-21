#include "AnimationManager.h"

//==============================================================================
JUCE_IMPLEMENT_SINGLETON(AnimationManager)

AnimationManager::AnimationManager()
{
    // Créer l'AnimatorUpdater pour gérer les mises à jour d'animation
    animatorUpdater = std::make_unique<juce::AnimatorUpdater>();
}

AnimationManager::~AnimationManager()
{
    stopAllAnimations();
    stopTimer();
    
    // ESSENTIEL : Nettoyer le singleton pour éviter l'assertion dans juce_Singleton.h:62
    clearSingletonInstance();
}

//==============================================================================
void AnimationManager::timerCallback()
{
    // Mettre à jour l'AnimatorUpdater (60 FPS)
    animatorUpdater->update();
    
    // Arrêter le timer s'il n'y a plus d'animations actives
    if (activeAnimators.empty())
    {
        // DBG("AnimationManager: Arrêt du timer, plus d'animations actives");
        stopTimer();
    }
}

//==============================================================================
int AnimationManager::animateValue(float& valueToAnimate,
                                   float targetValue,
                                   double durationMs,
                                   std::function<double(double)> easingFn,
                                   std::function<void()> onUpdate,
                                   std::function<void()> onComplete)
{
    int animationId = nextAnimationId++;
    
    // Stocker la valeur initiale
    float initialValue = valueToAnimate;
    
    // DBG("AnimationManager: Création animation ID=" << animationId << ", initial=" << initialValue << ", target=" << targetValue);
    
    // Créer l'animation avec ValueAnimatorBuilder
    auto builder = juce::ValueAnimatorBuilder()
        .withDurationMs(durationMs)
        .withValueChangedCallback([&valueToAnimate, initialValue, targetValue, onUpdate](float progress) {
            // Interpoler entre la valeur initiale et la valeur cible
            float newValue = initialValue + (targetValue - initialValue) * progress;
            valueToAnimate = newValue;
            
            // DBG("AnimationManager: Animation progress=" << progress << ", newValue=" << newValue);
            
            if (onUpdate)
                onUpdate();
        })
        .withOnCompleteCallback([this, animationId, onComplete]() {
            // DBG("AnimationManager: Animation ID=" << animationId << " terminée");
            // Nettoyer l'animation terminée
            activeAnimators.erase(animationId);
            if (onComplete)
                onComplete();
        });
    
    // Appliquer l'easing si fourni
    if (easingFn)
    {
        builder = std::move(builder).withEasing([easingFn](float t) -> float {
            return static_cast<float>(easingFn(static_cast<double>(t)));
        });
    }
    
    // Construire l'animateur
    auto animator = std::move(builder).build();
    
    // Stocker l'animation et la démarrer
    activeAnimators[animationId] = std::make_unique<juce::Animator>(std::move(animator));
    animatorUpdater->addAnimator(*activeAnimators[animationId]);
    activeAnimators[animationId]->start();
    
    // Démarrer le timer pour les mises à jour (60 FPS)
    if (!isTimerRunning())
    {
        // DBG("AnimationManager: Démarrage du timer pour les mises à jour");
        startTimer(16); // 60 FPS
    }
    
    // DBG("AnimationManager: Animation ID=" << animationId << " démarrée");
    
    return animationId;
}

int AnimationManager::animateValueSimple(float& valueToAnimate, 
                                        float targetValue, 
                                        double durationMs,
                                        std::function<void()> onUpdate)
{
    return animateValue(valueToAnimate, targetValue, durationMs, nullptr, onUpdate, nullptr);
}

//==============================================================================
int AnimationManager::animateComponentAlpha(juce::Component& component,
                                           float targetAlpha,
                                           double durationMs,
                                           std::function<double(double)> easingFn,
                                           std::function<void()> onUpdate,
                                           std::function<void()> onComplete)
{
    int animationId = nextAnimationId++;
    
    // Stocker l'opacité initiale en utilisant la méthode native de JUCE
    float initialAlpha = component.getAlpha();
    
    // Assurer que le composant est visible si on fait un fade-in
    if (targetAlpha > 0.0f && !component.isVisible())
    {
        component.setVisible(true);
    }
    
    // DBG("AnimationManager: Animation alpha ID=" << animationId << ", initial=" << initialAlpha << ", target=" << targetAlpha);
    
    // Créer l'animation avec ValueAnimatorBuilder
    auto builder = juce::ValueAnimatorBuilder()
        .withDurationMs(durationMs)
        .withValueChangedCallback([&component, initialAlpha, targetAlpha, onUpdate](float progress) {
            // Interpoler entre l'opacité initiale et la cible
            float newAlpha = initialAlpha + (targetAlpha - initialAlpha) * progress;
            
            // Utiliser la méthode native de JUCE Component::setAlpha()
            // Cette méthode sera appelée sur le Component directement, pas sur ColoredPanel
            component.juce::Component::setAlpha(newAlpha);
            
            // DBG("AnimationManager: Alpha animation progress=" << progress << ", newAlpha=" << newAlpha);
            
            if (onUpdate)
                onUpdate();
        })
        .withOnCompleteCallback([this, animationId, &component, targetAlpha, onComplete]() {
            // DBG("AnimationManager: Animation alpha ID=" << animationId << " terminée");
            
            // S'assurer que l'opacité finale est bien définie avec la méthode native
            component.juce::Component::setAlpha(targetAlpha);
            
            // Masquer le composant si fade-out complet
            if (targetAlpha <= 0.0f)
            {
                component.setVisible(false);
            }
            
            // Nettoyer l'animation terminée
            activeAnimators.erase(animationId);
            
            if (onComplete)
                onComplete();
        });
    
    // Appliquer l'easing si fourni
    if (easingFn)
    {
        builder = std::move(builder).withEasing([easingFn](float t) -> float {
            return static_cast<float>(easingFn(static_cast<double>(t)));
        });
    }
    
    // Construire l'animateur
    auto animator = std::move(builder).build();
    
    // Stocker l'animation et la démarrer
    activeAnimators[animationId] = std::make_unique<juce::Animator>(std::move(animator));
    animatorUpdater->addAnimator(*activeAnimators[animationId]);
    activeAnimators[animationId]->start();
    
    // Démarrer le timer pour les mises à jour (60 FPS)
    if (!isTimerRunning())
    {
        // DBG("AnimationManager: Démarrage du timer pour les mises à jour");
        startTimer(16); // 60 FPS
    }
    
    // DBG("AnimationManager: Animation alpha ID=" << animationId << " démarrée");
    
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

//==============================================================================
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
    {
        animatorUpdater->removeAnimator(*animator);
    }
    activeAnimators.clear();
    
    // Arrêter le timer
    if (isTimerRunning())
    {
        // DBG("AnimationManager: Arrêt du timer (stopAllAnimations)");
        stopTimer();
    }
}

bool AnimationManager::isAnimating(int animationId) const
{
    return activeAnimators.find(animationId) != activeAnimators.end();
}

int AnimationManager::getActiveAnimationCount() const
{
    return static_cast<int>(activeAnimators.size());
}

//==============================================================================
std::function<double(double)> AnimationManager::getDefaultEasing()
{
    // Ease-in-out quadratic
    return [](double t) -> double {
        if (t < 0.5)
            return 2.0 * t * t;
        else
            return 1.0 - 2.0 * (1.0 - t) * (1.0 - t);
    };
} 