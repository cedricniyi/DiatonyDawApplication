#include "RootAnimator.h"
#include "MainContentComponent.h"
#include "animation/AnimationManager.h"

RootAnimator::RootAnimator(MainContentComponent& mainContentComponent,
                           AnimationManager& animationManager)
    : mainContent(mainContentComponent),
      animManager(animationManager)
{
    // Prêt pour de futures animations root
}

RootAnimator::~RootAnimator()
{
}
