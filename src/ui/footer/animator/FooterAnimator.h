#pragma once

#include <JuceHeader.h>
#include "../FooterPanel.h"
#include "ui/animation/AnimationManager.h"

//==============================================================================
class FooterAnimator
{
public:
    FooterAnimator(FooterPanel& panel, 
                   AnimationManager& animationManager,
                   float& footerFlexRef,
                   std::function<void()> onLayoutUpdate)
        : footerPanel(panel), 
          animManager(animationManager),
          footerFlex(footerFlexRef),
          onLayoutChanged(onLayoutUpdate)
    {
        // S'abonner au callback de toggle du FooterPanel
        footerPanel.onToggleRequest = [this]() {
            toggleSequence();
        };
        
        DBG("FooterAnimator: Initialisé avec animation flex intégrée");
    }
    
    ~FooterAnimator() = default;
    
    /** Déclenche la séquence d'animation complète (flex + contenu) */
    void toggleSequence()
    {
        DBG("FooterAnimator: Déclenchement du toggle complet, état actuel : " << (footerPanel.getExpanded() ? "élargi" : "compact"));
        
        // Déclencher l'animation de flex du footer DIRECTEMENT
        startFlexAnimation();
        
        if (footerPanel.getExpanded())
        {
            // MASQUER : fade out → puis rétrécir la grille progressivement
            hideSequence();
        }
        else
        {
            // RÉVÉLER : élargir la grille progressivement → puis fade in
            showSequence();
        }
    }
    
private:
    FooterPanel& footerPanel;
    AnimationManager& animManager;
    float& footerFlex;                    // Référence vers la valeur flex du PluginEditor
    std::function<void()> onLayoutChanged; // Callback pour redessiner le layout
    
    /** Animation de la valeur flex du footer (maintenant intégrée) */
    void startFlexAnimation()
    {
        DBG("FooterAnimator: Animation flex déclenchée ! footerFlex actuel = " << footerFlex);
        
        // Valeur de départ et cible (toggle)
        float targetFooterFlex = (footerFlex < 30.0f) ? 30.0f : 15.0f;
        
        DBG("FooterAnimator: Target footerFlex = " << targetFooterFlex);
        
        // Animer la valeur flex
        animManager.animateValueSimple(
            footerFlex,
            targetFooterFlex,
            300.0, // durée
            [this]() { 
                // Callback pour redessiner le layout à chaque frame
                if (onLayoutChanged) {
                    onLayoutChanged();
                }
            }
        );
    }
    
    void hideSequence()
    {
        DBG("FooterAnimator: Séquence MASQUER - Étape 1: Fade out du composant");
        
        // Étape 1 : Faire disparaître le composant
        animManager.fadeComponent(
            footerPanel.getFadingComponent(), 
            false, // fade out
            300.0, // durée
            [this]() { // callback : quand le fade out est terminé
                DBG("FooterAnimator: Séquence MASQUER - Étape 2: Animer la grille (rétrécissement)");
                
                // Étape 2 : Animer la grille progressivement (1.0f → 0.0f)
                animManager.animateValueSimple(
                    footerPanel.getGridFractionRef(), // référence à la valeur à animer
                    0.0f, // target : grille compacte
                    350.0, // durée
                    [this]() { // onUpdate : redessiner à chaque frame
                        footerPanel.resized();
                    }
                );
                
                // Marquer l'état comme compact et finaliser
                footerPanel.setExpanded(false);
                DBG("FooterAnimator: Séquence MASQUER terminée");
            }
        );
    }
    
    void showSequence()
    {
        DBG("FooterAnimator: Séquence RÉVÉLER - Étape 1: Animer la grille (élargissement)");
        
        // Étape 1 : Animer la grille progressivement (0.0f → 1.0f)
        animManager.animateValueSimple(
            footerPanel.getGridFractionRef(), // référence à la valeur à animer
            1.0f, // target : grille élargie
            350.0, // durée
            [this]() { // onUpdate : redessiner à chaque frame
                footerPanel.resized();
            }
        );
        
        // Marquer l'état comme élargi
        footerPanel.setExpanded(true);
        
        // Petite pause pour laisser la grille se déployer un peu
        juce::Timer::callAfterDelay(150, [this]() {
            DBG("FooterAnimator: Séquence RÉVÉLER - Étape 2: Fade in du composant");
            
            // Étape 2 : Faire apparaître le composant
            animManager.fadeComponent(
                footerPanel.getFadingComponent(), 
                true, // fade in
                300.0, // durée
                [this]() { // callback : quand le fade in est terminé
                    DBG("FooterAnimator: Séquence RÉVÉLER terminée");
                }
            );
        });
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterAnimator)
}; 