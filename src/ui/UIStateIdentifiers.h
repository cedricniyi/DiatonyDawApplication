#pragma once

#include <JuceHeader.h>

/**
 * @brief Identifiants pour les propriétés ValueTree de l'état UI
 * 
 * Ce fichier centralise toutes les constantes utilisées pour gérer l'état
 * de l'interface utilisateur via ValueTree. Ces identifiants peuvent être
 * utilisés par tous les composants qui ont besoin d'accéder à l'état global.
 */
namespace UIStateIdentifiers
{
    // =================================================================================
    // Nom de l'arbre principal
    const juce::Identifier APP_STATE { "APP_STATE" };
    
    // =================================================================================
    // Propriétés d'état UI - Footer
    const juce::Identifier footerExpanded { "footerExpanded" };
    const juce::Identifier interactivePianoVisible { "interactivePianoVisible" };
    
    // =================================================================================
    // Propriétés d'état UI - Header/Dock
    const juce::Identifier dockVisible { "dockVisible" };
    
    // =================================================================================
    // Propriétés d'état UI - History Panel (panneau latéral droit)
    const juce::Identifier historyPanelVisible { "historyPanelVisible" };
} 