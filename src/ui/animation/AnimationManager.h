#pragma once

#include <JuceHeader.h>
#include <functional>
#include <memory>

/**
 * @brief Gestionnaire centralisé des animations utilisant le module juce_animation de JUCE 8
 * 
 * Cette classe utilise ValueAnimatorBuilder et AnimatorUpdater pour créer des animations fluides
 * sans avoir besoin de gérer manuellement des Timer ou de la logique d'interpolation.
 */
class AnimationManager : private juce::Timer
{
public:
    //==============================================================================
    /** Obtient l'instance singleton de l'AnimationManager */
    static AnimationManager& getInstance();
    
    //==============================================================================
    /** 
     * Anime une valeur float avec des paramètres personnalisables
     * 
     * @param valueToAnimate    Référence vers la valeur à animer
     * @param targetValue       Valeur cible à atteindre
     * @param durationMs        Durée de l'animation en millisecondes
     * @param easingFn          Fonction d'easing (optionnel, par défaut ease-in-out)
     * @param onUpdate          Callback appelé à chaque mise à jour (optionnel)
     * @param onComplete        Callback appelé à la fin de l'animation (optionnel)
     * @return                  ID de l'animation pour pouvoir l'arrêter si nécessaire
     */
    int animateValue(float& valueToAnimate,
                     float targetValue,
                     double durationMs,
                     std::function<double(double)> easingFn = nullptr,
                     std::function<void()> onUpdate = nullptr,
                     std::function<void()> onComplete = nullptr);
    
    /** 
     * Version simplifiée pour animer une valeur avec les paramètres par défaut
     */
    int animateValueSimple(float& valueToAnimate, 
                          float targetValue, 
                          double durationMs = 300.0,
                          std::function<void()> onUpdate = nullptr);
    
    //==============================================================================
    /** Arrête une animation spécifique par son ID */
    void stopAnimation(int animationId);
    
    /** Arrête toutes les animations en cours */
    void stopAllAnimations();
    
    /** Vérifie si une animation est en cours */
    bool isAnimating(int animationId) const;
    
    /** Retourne le nombre d'animations actives */
    int getActiveAnimationCount() const;

private:
    //==============================================================================
    AnimationManager();
    ~AnimationManager();
    
    // Timer callback pour mettre à jour l'AnimatorUpdater
    void timerCallback() override;
    
    //==============================================================================
    std::unique_ptr<juce::AnimatorUpdater> animatorUpdater;
    std::unordered_map<int, std::unique_ptr<juce::Animator>> activeAnimators;
    int nextAnimationId = 1;
    
    // Fonction d'easing par défaut (ease-in-out quadratic)
    std::function<double(double)> getDefaultEasing();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnimationManager)
}; 