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
    // ✅ S'abonner au callback du MainContentComponent (même pattern que FooterAnimator)
    mainContent.onLayoutAnimationNeeded = [this]() {
        handleLayoutAnimationNeeded();
    };
    
    DBG("RootAnimator: Initialisé et abonné aux callbacks du MainContentComponent");
}

RootAnimator::~RootAnimator()
{
    // Désabonner le callback
    if (mainContent.onLayoutAnimationNeeded)
        mainContent.onLayoutAnimationNeeded = nullptr;
}

void RootAnimator::handleLayoutAnimationNeeded()
{
    DBG("RootAnimator: Callback reçu - Déclenchement des animations de layout");
    
    // Déclencher les animations appropriées selon l'état
    animateFooterFlex();
    // Je peux ajouter d'autre animations à ce niveau si besoin
}

void RootAnimator::animateFooterFlex()
{
    // Détecter l'état actuel via la valeur flex courante
    // Si footerFlex < 25, alors on passe à l'état élargi, sinon on réduit
    float currentFlex = mainContent.getFooterFlexRef();
    float targetFooterFlex = (currentFlex < 25.0f) ? 30.0f : 15.0f;
    
    DBG("RootAnimator: Animation footer flex " << currentFlex << " -> " << targetFooterFlex);
    
    // Animer la valeur flex du footer
    animManager.animateValueSimple(
        mainContent.getFooterFlexRef(),
        targetFooterFlex,
        300.0, // durée
        [this]() { 
            // Callback pour redessiner le layout global à chaque frame
            mainContent.resized();
        }
    );
} 