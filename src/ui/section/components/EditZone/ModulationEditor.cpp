#include "ModulationEditor.h"
#include "utils/FontManager.h"

ModulationEditor::ModulationEditor()
    : ColoredPanel(juce::Colours::lightgreen.withAlpha(0.25f)) // Vert clair plus visible
{
    setupLabels();
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
}

void ModulationEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // Disposition verticale simple
    titleLabel.setBounds(bounds.removeFromTop(40));
    bounds.removeFromTop(10); // Espacement
    contentLabel.setBounds(bounds.removeFromTop(60));
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

void ModulationEditor::setupLabels()
{
    // Configuration du label de titre
    titleLabel.setJustificationType(juce::Justification::centredTop);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::darkgreen);
    
    // Configuration du label de contenu
    contentLabel.setJustificationType(juce::Justification::centredTop);
    contentLabel.setColour(juce::Label::textColourId, juce::Colours::darkslategrey);
    
    // Application des polices via FontManager
    juce::SharedResourcePointer<FontManager> fontManager;
    
    auto titleFontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
    titleLabel.setFont(juce::Font(titleFontOptions));
    
    auto contentFontOptions = fontManager->getSFProText(16.0f, FontManager::FontWeight::Regular);
    contentLabel.setFont(juce::Font(contentFontOptions));
    
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(contentLabel);
}

void ModulationEditor::updateContent()
{
    if (currentModulationId.isEmpty())
    {
        titleLabel.setText("Modulation Editor", juce::dontSendNotification);
        contentLabel.setText("No modulation selected", juce::dontSendNotification);
    }
    else
    {
        titleLabel.setText("Modulation Editor", juce::dontSendNotification);
        contentLabel.setText("Édition de la Modulation : " + currentModulationId + "\n\n" +
                           "Fonctionnalités d'édition à venir :\n" +
                           "• Gestion des accords de destination\n" +
                           "• Configuration des paramètres temporels\n" +
                           "• Types de modulation (majeur/mineur)\n" +
                           "• Réglages avancés", 
                           juce::dontSendNotification);
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