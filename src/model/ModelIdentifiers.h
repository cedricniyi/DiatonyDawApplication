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
    const juce::Identifier fromSectionId  { "fromSectionId" };   // ID de la section source (conceptuel : connecte S1→S2)
    const juce::Identifier toSectionId    { "toSectionId" };     // ID de la section destination (conceptuel : connecte S1→S2)
    const juce::Identifier fromChordIndex { "fromChordIndex" };  // Index RELATIF de l'accord de début (relatif à sa section)
    const juce::Identifier toChordIndex   { "toChordIndex" };    // Index RELATIF de l'accord de fin (relatif à sa section)
} 