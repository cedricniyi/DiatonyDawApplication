#pragma once
#include <juce_core/juce_core.h>

/**
 * @brief Identifiants JUCE pour les types et propriétés du ValueTree.
 */
namespace ModelIdentifiers
{
    // Types de noeuds
    const juce::Identifier PIECE      { "Piece" };
    const juce::Identifier SECTION    { "Section" };
    const juce::Identifier MODULATION { "Modulation" };
    const juce::Identifier PROGRESSION{ "Progression" };
    const juce::Identifier CHORD      { "Chord" };

    // Propriétés communes
    const juce::Identifier id   { "id" };
    const juce::Identifier name { "name" };

    // Propriétés Section
    const juce::Identifier tonalityNote       { "tonalityNote" };
    const juce::Identifier tonalityAlteration { "tonalityAlteration" };
    const juce::Identifier isMajor            { "isMajor" };

    // Propriétés Chord
    const juce::Identifier degree  { "degree" };
    const juce::Identifier quality { "quality" };
    const juce::Identifier state   { "state" };
    
    // Propriétés Modulation
    const juce::Identifier modulationType { "modulationType" };
    const juce::Identifier fromSectionId  { "fromSectionId" };
    const juce::Identifier toSectionId    { "toSectionId" };
    const juce::Identifier fromChordIndex { "fromChordIndex" };
    const juce::Identifier toChordIndex   { "toChordIndex" };
} 