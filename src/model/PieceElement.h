#pragma once

#include <juce_core/juce_core.h>
#include <functional>

/**
 * Classe de base abstraite pour tous les éléments composant une Piece
 * Permet de stocker Section et Modulation dans une même collection
 */
class PieceElement 
{
public:
    virtual ~PieceElement() = default;

    // Énumération pour identifier le type d'élément
    enum class Type { Section, Modulation };
    
    // Méthodes virtuelles pures à implémenter par les classes dérivées
    virtual Type getType() const = 0;
    virtual const juce::String& getName() const = 0;
    
    // Callback optionnel pour notifier les changements
    std::function<void()> onElementChanged;

protected:
    void notifyChange() 
    {
        if (onElementChanged)
            onElementChanged();
    }
}; 