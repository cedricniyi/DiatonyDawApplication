#pragma once

#include <JuceHeader.h>

/** @brief Identifiants pour les propriétés ValueTree de l'état UI. */
namespace UIStateIdentifiers
{
    const juce::Identifier APP_STATE { "APP_STATE" };
    
    // Header/Dock
    const juce::Identifier dockVisible { "dockVisible" };
    
    // History Panel (panneau latéral droit)
    const juce::Identifier historyPanelVisible { "historyPanelVisible" };
    
    // Note: footerExpanded et interactivePianoVisible retirés (footer non utilisé)
}
