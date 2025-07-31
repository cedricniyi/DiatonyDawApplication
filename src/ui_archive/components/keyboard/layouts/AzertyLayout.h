#pragma once

#include "../KeyboardLayout.h"

//==============================================================================
/**
 * Layout AZERTY français
 */
class AzertyLayout : public KeyboardLayout
{
public:
    AzertyLayout()
        : KeyboardLayout("AZERTY")
    {
        initializeLayout();
    }
    
    void initializeLayout() override
    {
        // Couleur de fond standard pour toutes les touches du layout AZERTY (fond noir, texte rouge)
        juce::Colour keybackgroundColour = juce::Colours::black;
        juce::Colour textColour = juce::Colours::whitesmoke;
        
        // Première rangée (1-0)
        keys.push_back(KeyInfo(49, "1", "1", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(50, "2", "2", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(51, "3", "3", keybackgroundColour, textColour)); 
        keys.push_back(KeyInfo(52, "4", "4", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(53, "5", "5", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(54, "6", "6", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(55, "7", "7", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(56, "8", "8", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(57, "9", "9", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(48, "0", "0", keybackgroundColour, textColour));
        
        // Deuxième rangée (AZERTYUIOP)
        keys.push_back(KeyInfo(81, "A", "Q", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(87, "Z", "W", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(69, "E", "E", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(82, "R", "R", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(84, "T", "T", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(89, "Y", "Y", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(85, "U", "U", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(73, "I", "I", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(79, "O", "O", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(80, "P", "P", keybackgroundColour, textColour));
        
        // Troisième rangée (QSDFGHJKLM)
        keys.push_back(KeyInfo(65, "Q", "A", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(83, "S", "S", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(68, "D", "D", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(70, "F", "F", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(71, "G", "G", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(72, "H", "H", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(74, "J", "J", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(75, "K", "K", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(76, "L", "L", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(77, "M", "M", keybackgroundColour, textColour));
        
        // Quatrième rangée (WXCVBN)
        keys.push_back(KeyInfo(90, "W", "Z", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(88, "X", "X", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(67, "C", "C", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(86, "V", "V", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(66, "B", "B", keybackgroundColour, textColour));
        keys.push_back(KeyInfo(78, "N", "N", keybackgroundColour, textColour));
    }
}; 