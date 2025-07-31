#include "RootAnimator.h"
#include "MainContentComponent.h"
#include "animation/AnimationManager.h"
#include "UIStateIdentifiers.h"

//==============================================================================
RootAnimator::RootAnimator(MainContentComponent& mainContentComponent,
                           AnimationManager& animationManager)
    : mainContent(mainContentComponent),
      animManager(animationManager)
{
    // S'abonne au callback du MainContentComponent pour déclencher les animations de layout.
    mainContent.onLayoutAnimationNeeded = [this]() {
        handleLayoutAnimationNeeded();
    };
}

RootAnimator::~RootAnimator()
{
    // Se désabonne du callback pour éviter les appels invalides.
    if (mainContent.onLayoutAnimationNeeded)
        mainContent.onLayoutAnimationNeeded = nullptr;
}

void RootAnimator::handleLayoutAnimationNeeded()
{
    // Le callback a été reçu, on déclenche les animations de layout nécessaires.
    animateFooterFlex();
    // D'autres animations globales pourraient être ajoutées ici.
}

void RootAnimator::animateFooterFlex()
{
    // Détecte l'état actuel (compact ou étendu) via la valeur flex du footer.
    // Si la valeur est inférieure à 25, on étend le footer, sinon on le réduit.
    float currentFlex = mainContent.getFooterFlexRef();
    float targetFooterFlex = (currentFlex < 25.0f) ? 30.0f : 15.0f;
    
    // Lance l'animation de la propriété flex du footer.
    animManager.animateValueSimple(
        mainContent.getFooterFlexRef(),
        targetFooterFlex,
        300.0, // durée en ms
        [this]() { 
            // À chaque frame de l'animation, redessine le layout global.
            mainContent.resized();
        }
    );
} 