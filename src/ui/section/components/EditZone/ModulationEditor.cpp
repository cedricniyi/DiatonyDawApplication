#include "ModulationEditor.h"

ModulationEditor::ModulationEditor()
    : ColoredPanel(juce::Colours::lightgreen.withAlpha(0.25f)) // Vert clair plus visible
{
    setupModulationNameLabel();
    updateContent();
}

ModulationEditor::~ModulationEditor()
{
}

void ModulationEditor::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
    
    // Dessiner la bordure sophistiquée
    drawBorder(g);
    
    // Dessiner le trait de séparation
    drawSeparatorLine(g);
}

void ModulationEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // Zone du header : titre + espacement pour le trait
    headerArea = bounds.removeFromTop(60); // Hauteur augmentée pour inclure le trait
    
    // Zone du contenu : le reste de l'espace
    contentArea = bounds; // Garde tout le reste pour le contenu futur
    
    // Positionner le titre dans la zone header (avec un peu de marge)
    auto titleBounds = headerArea.reduced(0, 5).removeFromTop(40);
    modulationNameLabel.setBounds(titleBounds);
}

void ModulationEditor::setModulationToEdit(const juce::String& modulationId)
{
    if (currentModulationId != modulationId)
    {
        currentModulationId = modulationId;
        updateContent();
        repaint();
    }
}

void ModulationEditor::setupModulationNameLabel()
{
    // Configuration du label de nom de modulation
    modulationNameLabel.setJustificationType(juce::Justification::centredLeft);
    modulationNameLabel.setColour(juce::Label::textColourId, juce::Colours::darkgreen);
    
    // Application de la police via FontManager - taille légèrement plus grande
    auto modulationNameFontOptions = fontManager->getSFProDisplay(28.0f, FontManager::FontWeight::Bold);
    modulationNameLabel.setFont(juce::Font(modulationNameFontOptions));
    
    addAndMakeVisible(modulationNameLabel);
}

void ModulationEditor::updateContent()
{
    if (currentModulationId.isEmpty())
    {
        modulationNameLabel.setText("No Modulation", juce::dontSendNotification);
    }
    else
    {
        // Extraire l'index de la modulation depuis l'ID (ex: "Modulation_0" -> "Modulation 1 -> 2")
        juce::String displayName = "Modulation";
        if (currentModulationId.startsWith("Modulation_"))
        {
            int index = currentModulationId.getTrailingIntValue();
            // La modulation connecte la section (index+1) à la section (index+2)
            int fromSection = index + 1;
            int toSection = index + 2;
            displayName = "Modulation " + juce::String(fromSection) + " → " + juce::String(toSection);
        }
        
        modulationNameLabel.setText(displayName, juce::dontSendNotification);
    }
} 

void ModulationEditor::drawBorder(juce::Graphics& g)
{
    // Logique inspirée de OutlineTextButton::drawButtonBackground
    auto bounds = getLocalBounds().toFloat().reduced(borderThickness * 0.5f);
    
    juce::Colour currentBorderColour = borderColour;
    
    // Gestion des états comme dans OutlineTextButton
    // Pour un éditeur, on peut simuler un état "actif" quand une modulation est sélectionnée
    if (isEditingModulation())
    {
        // État actif : bordure plus vive
        currentBorderColour = currentBorderColour.brighter(0.3f);
    }
    else
    {
        // État inactif : bordure plus discrète
        currentBorderColour = currentBorderColour.withAlpha(0.6f);
    }
    
    g.setColour(currentBorderColour);
    g.drawRoundedRectangle(bounds, cornerRadius, borderThickness);
}

void ModulationEditor::drawSeparatorLine(juce::Graphics& g)
{
    // Dessiner un trait de séparation entre le header et le contenu
    auto separatorY = headerArea.getBottom(); // Aligné avec le bas du header
    auto separatorStart = headerArea.getX();
    auto separatorEnd = headerArea.getRight();
    
    g.setColour(juce::Colours::darkgreen.withAlpha(0.15f));
    g.drawHorizontalLine(separatorY, static_cast<float>(separatorStart), static_cast<float>(separatorEnd));
} 