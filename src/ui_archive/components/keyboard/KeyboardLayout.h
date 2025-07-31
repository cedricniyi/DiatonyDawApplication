#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>

//==============================================================================
/**
 * Définition d'un layout de clavier
 * Permet de définir différentes configurations de touches avec leurs étiquettes et couleurs
 */
class KeyboardLayout
{
public:
    // Structure pour définir une touche individuelle
    struct KeyInfo {
        int keyCode;                     // Code de la touche JUCE
        juce::String defaultLabel;       // Étiquette affichée sur la touche
        juce::String qwertyPosition;     // Position équivalente sur un clavier QWERTY (pour référence)
        juce::Colour keyColour;          // Couleur de fond de la touche
        juce::Colour textColour;         // Couleur du texte de la touche
        juce::Colour activeKeyColour;    // Couleur quand la touche est active/sélectionnée
        juce::Colour activeTextColour;   // Couleur du texte quand la touche est active
        juce::Colour pressedKeyColour;   // Couleur quand la touche est pressée
        
        // Constructeur pour une initialisation facile
        KeyInfo(int code, const juce::String& label, const juce::String& qwertyPos,
                const juce::Colour& keyCol = juce::Colours::grey.withAlpha(0.3f),
                const juce::Colour& textCol = juce::Colours::white.withAlpha(0.7f),
                const juce::Colour& activeKeyCol = juce::Colours::orange.withAlpha(0.5f),
                const juce::Colour& activeTextCol = juce::Colours::white,
                const juce::Colour& pressedKeyCol = juce::Colours::blue.withAlpha(0.7f))
            : keyCode(code), defaultLabel(label), qwertyPosition(qwertyPos),
              keyColour(keyCol), textColour(textCol),
              activeKeyColour(activeKeyCol), activeTextColour(activeTextCol),
              pressedKeyColour(pressedKeyCol) {}
    };
    
    KeyboardLayout(const juce::String& name)
        : layoutName(name) {}
    
    virtual ~KeyboardLayout() = default;
    
    // Méthode virtuelle que chaque layout doit implémenter pour définir ses touches
    virtual void initializeLayout() = 0;
    
    // Accesseurs
    const juce::String& getName() const { return layoutName; }
    
    const std::vector<KeyInfo>& getKeys() const { return keys; }
    
protected:
    std::vector<KeyInfo> keys;
    juce::String layoutName;
}; 