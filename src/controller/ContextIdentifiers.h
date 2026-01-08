#pragma once
#include <juce_core/juce_core.h>

/** @brief Identifiants pour le ValueTree de sélection UI. */
namespace ContextIdentifiers
{
    const juce::Identifier SELECTION_STATE { "SelectionState" };
    const juce::Identifier selectionType     { "selectionType" };
    const juce::Identifier selectedElementId { "selectedElementId" };
} 