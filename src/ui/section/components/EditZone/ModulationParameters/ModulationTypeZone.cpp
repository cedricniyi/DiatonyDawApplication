#include "ModulationTypeZone.h"

ModulationTypeZone::ModulationTypeZone()
    : BaseZone("Type")
{
    setupButtons();
}

void ModulationTypeZone::setupButtons()
{
    for (size_t i = 0; i < 4; ++i)
    {
        typeButtons[i] = std::make_unique<SelectableStyledButton>(
            typeLabels[i],
            normalColour,
            selectedColour,
            15.0f,  // Taille de police
            FontManager::FontWeight::Medium
        );
        
        typeButtons[i]->onClick = [this, i]() {
            selectType(static_cast<int>(i));
        };
        
        addAndMakeVisible(*typeButtons[i]);
    }
    
    // Sélectionner le premier par défaut
    selectType(0);
}

void ModulationTypeZone::resizeContent(const juce::Rectangle<int>& contentBounds)
{
    if (contentBounds.isEmpty())
        return;
    
    constexpr int BUTTON_SPACING = 4;
    constexpr int BUTTON_HEIGHT = 24;
    
    // Calculer la largeur de chaque bouton (4 boutons sur une ligne)
    int availableWidth = contentBounds.getWidth() - (3 * BUTTON_SPACING);
    int btnWidth = availableWidth / 4;
    
    // Centrer verticalement
    int startY = contentBounds.getY() + (contentBounds.getHeight() - BUTTON_HEIGHT) / 2;
    
    int x = contentBounds.getX();
    for (size_t i = 0; i < 4; ++i)
    {
        typeButtons[i]->setBounds(x, startY, btnWidth, BUTTON_HEIGHT);
        x += btnWidth + BUTTON_SPACING;
    }
}

void ModulationTypeZone::selectType(int index)
{
    if (index < 0 || index >= 4)
        return;
    
    selectedType = modulationTypes[static_cast<size_t>(index)];
    
    // Mettre à jour l'état visuel de tous les boutons
    for (size_t i = 0; i < 4; ++i)
    {
        typeButtons[i]->setSelected(static_cast<int>(i) == index);
    }
    
    // Notifier le callback
    if (onTypeChanged)
    {
        onTypeChanged(selectedType);
    }
}

void ModulationTypeZone::setSelectedType(Diatony::ModulationType type)
{
    // Trouver l'index correspondant
    for (size_t i = 0; i < 4; ++i)
    {
        if (modulationTypes[i] == type)
        {
            if (selectedType == type)
                return;  // Déjà sélectionné
            
            selectedType = type;
            
            // Mettre à jour l'état visuel sans déclencher le callback
            for (size_t j = 0; j < 4; ++j)
            {
                typeButtons[j]->setSelected(j == i);
            }
            return;
        }
    }
}

