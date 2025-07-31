#pragma once
#include <juce_core/juce_core.h>

namespace ModelIdentifiers
{
    // Noms des types de noeuds (les "tables" de notre base de données)
    const juce::Identifier PIECE      { "Piece" };
    const juce::Identifier SECTION    { "Section" };
    const juce::Identifier MODULATION { "Modulation" };
    const juce::Identifier PROGRESSION{ "Progression" };
    const juce::Identifier CHORD      { "Chord" };

    // Noms des propriétés (les "colonnes" de nos tables)
    const juce::Identifier id         { "id" }; // ID unique pour chaque élément
    const juce::Identifier name       { "name" };

    // Propriétés de Section
    const juce::Identifier tonalityNote       { "tonalityNote" };
    const juce::Identifier tonalityAlteration { "tonalityAlteration" };
    const juce::Identifier isMajor            { "isMajor" };

    // Propriétés de Chord
    const juce::Identifier degree { "degree" };
    const juce::Identifier quality{ "quality" };
    const juce::Identifier state  { "state" };
    
    // Propriétés de Modulation
    const juce::Identifier modulationType { "modulationType" };
} 