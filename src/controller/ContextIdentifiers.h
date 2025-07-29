#pragma once
#include <juce_core/juce_core.h>

namespace ContextIdentifiers
{
    // Identifiant pour le ValueTree de l'état de sélection
    const juce::Identifier SELECTION_STATE { "SelectionState" };
    
    // Propriétés de l'état de sélection
    const juce::Identifier selectionType     { "selectionType" };
    const juce::Identifier selectedElementId { "selectedElementId" };
} 