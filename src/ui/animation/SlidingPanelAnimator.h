#pragma once

#include <JuceHeader.h>
#include <functional>

// Forward declaration
class AnimationManager;

//==============================================================================
/**
 * @brief Animator générique pour les panneaux coulissants avec fade
 * 
 * Remplace les animators spécifiques (FooterAnimator, HistoryPanelAnimator)
 * par une implémentation réutilisable.
 * 
 * Séquence show : expand (0→1) puis fade in
 * Séquence hide : fade out puis collapse (1→0)
 */
class SlidingPanelAnimator
{
public:
    /**
     * @param fractionRef       Référence à la valeur d'animation (0.0 = fermé, 1.0 = ouvert)
     * @param fadingComponent   Composant qui effectue le fade in/out
     * @param onUpdate          Callback appelé à chaque frame pour redessiner le layout
     * @param animManager       Référence vers l'AnimationManager
     * @param expandDurationMs  Durée de l'animation d'expansion (défaut: 300ms)
     * @param fadeDurationMs    Durée de l'animation de fade (défaut: 200ms)
     */
    SlidingPanelAnimator(float& fractionRef,
                         juce::Component& fadingComponent,
                         std::function<void()> onUpdate,
                         AnimationManager& animManager,
                         double expandDurationMs = 300.0,
                         double fadeDurationMs = 200.0);
    
    ~SlidingPanelAnimator() = default;
    
    /** Point d'entrée principal - anime vers visible ou caché */
    void setVisible(bool visible);
    
    /** Anime vers l'état ouvert (expand + fade in) */
    void show();
    
    /** Anime vers l'état fermé (fade out + collapse) */
    void hide();
    
private:
    float& fraction;
    juce::Component& fadingComp;
    std::function<void()> updateCallback;
    AnimationManager& animationManager;
    
    double expandDuration;
    double fadeDuration;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SlidingPanelAnimator)
};


