#pragma once

#include <JuceHeader.h>
#include "../FooterPanel.h"
#include "ui/animation/AnimationManager.h"

//==============================================================================
class FooterAnimator
{
public:
    FooterAnimator(FooterPanel& panel, 
                   AnimationManager& animationManager)
        : footerPanel(panel), 
          animManager(animationManager)
    {        
        // S'abonner au callback du FooterPanel pour les changements de visibilité du piano
        footerPanel.onInteractivePianoVisibilityChange = [this](bool visible) {
            setInteractivePianoVisible(visible);
        };
        
        DBG("FooterAnimator: Initialisé et abonné aux callbacks du FooterPanel");
    }
    
    /** Destructeur */
    ~FooterAnimator()
    {
        // Désabonner le callback
        if (footerPanel.onInteractivePianoVisibilityChange)
            footerPanel.onInteractivePianoVisibilityChange = nullptr;
    }
    
    /** Déclenche la séquence d'animation du contenu (grid + fade uniquement) */
    void toggleSequence()
    {
        DBG("FooterAnimator: Déclenchement du toggle contenu, état actuel : " << (footerPanel.getExpanded() ? "élargi" : "compact"));
        
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
    
    /** Appelée automatiquement via callback quand FooterPanel détecte changement ValueTree */
    void setInteractivePianoVisible(bool visible)
    {
        DBG("FooterAnimator: Callback reçu - Animation InteractivePiano -> " << (visible ? "visible" : "caché"));
        
        // Synchroniser l'état du FooterPanel avec le ValueTree
        footerPanel.setExpanded(visible);
        
        if (visible)
        {
            // RÉVÉLER : élargir la grille progressivement → puis fade in
            showSequence();
        }
        else
        {
            // MASQUER : fade out → puis rétrécir la grille progressivement
            hideSequence();
        }
    }
    
private:
    FooterPanel& footerPanel;
    AnimationManager& animManager;
    
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