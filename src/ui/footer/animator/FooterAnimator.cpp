#include "FooterAnimator.h"
#include "../FooterPanel.h"
#include "ui/animation/AnimationManager.h"

//==============================================================================
FooterAnimator::FooterAnimator(FooterPanel& panel,
                               AnimationManager& animationManager)
    : footerPanel(panel),
      animManager(animationManager)
{
    // S'abonne au callback du FooterPanel pour les changements de visibilité du piano.
    footerPanel.onInteractivePianoVisibilityChange = [this](bool visible) {
        setInteractivePianoVisible(visible);
    };
}

FooterAnimator::~FooterAnimator()
{
    // Se désabonne du callback pour éviter les appels invalides.
    if (footerPanel.onInteractivePianoVisibilityChange)
        footerPanel.onInteractivePianoVisibilityChange = nullptr;
}

void FooterAnimator::toggleSequence()
{
    // Déclenche la séquence d'animation appropriée (masquer ou révéler)
    // en fonction de l'état actuel du panneau.
    if (footerPanel.getExpanded())
    {
        // MASQUER : fondu sortant, puis rétrécissement de la grille.
        hideSequence();
    }
    else
    {
        // RÉVÉLER : élargissement de la grille, puis fondu entrant.
        showSequence();
    }
}

void FooterAnimator::setInteractivePianoVisible(bool visible)
{
    // Callback reçu pour animer la visibilité du piano interactif.

    // Met à jour l'état du panneau.
    footerPanel.setExpanded(visible);

    if (visible)
    {
        showSequence();
    }
    else
    {
        hideSequence();
    }
}

void FooterAnimator::hideSequence()
{
    // Étape 1 : Fondu sortant du composant.
    animManager.fadeComponent(
        footerPanel.getFadingComponent(),
        false, // fade out
        300.0, // durée
        [this]() { // Callback : une fois le fondu terminé.
            // Étape 2 : Animation pour rétrécir la grille (1.0f -> 0.0f).
            animManager.animateValueSimple(
                footerPanel.getGridFractionRef(), // référence à la valeur à animer
                0.0f,                             // target : grille compacte
                350.0,                            // durée
                [this]() {                        // onUpdate : redessiner à chaque frame.
                    footerPanel.resized();
                });

            // Finalise l'état comme compact.
            footerPanel.setExpanded(false);
        });
}

void FooterAnimator::showSequence()
{
    // Étape 1 : Animation pour élargir la grille (0.0f -> 1.0f).
    animManager.animateValueSimple(
        footerPanel.getGridFractionRef(), // référence à la valeur à animer
        1.0f,                             // target : grille élargie
        350.0,                            // durée
        [this]() {                        // onUpdate : redessiner à chaque frame.
            footerPanel.resized();
        });

    // Met à jour l'état comme élargi.
    footerPanel.setExpanded(true);

    // Petite pause pour laisser la grille se déployer avant le fondu.
    juce::Timer::callAfterDelay(150, [this]() {
        // Étape 2 : Fondu entrant du composant.
        animManager.fadeComponent(
            footerPanel.getFadingComponent(),
            true,  // fade in
            300.0, // durée
            []() {
                // Séquence de révélation terminée.
            });
    });
}