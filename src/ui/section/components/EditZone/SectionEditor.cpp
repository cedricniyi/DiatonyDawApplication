#include "SectionEditor.h"

SectionEditor::SectionEditor()
    : ColoredPanel(juce::Colours::lightblue.withAlpha(0.25f)) // Bleu clair plus visible
{
    setupSectionNameLabel();
    updateContent();
}

SectionEditor::~SectionEditor()
{
}

void SectionEditor::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
    
    // Dessiner la bordure sophistiquée
    drawBorder(g);
    
    // Dessiner le trait de séparation
    drawSeparatorLine(g);
}

void SectionEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // Zone du header : titre + espacement pour le trait
    headerArea = bounds.removeFromTop(60); // Hauteur augmentée pour inclure le trait
    
    // Zone du contenu : le reste de l'espace
    contentArea = bounds; // Garde tout le reste pour le contenu futur
    
    // Positionner le titre dans la zone header (avec un peu de marge)
    auto titleBounds = headerArea.reduced(0, 5).removeFromTop(40);
    sectionNameLabel.setBounds(titleBounds);
}

void SectionEditor::setSectionToEdit(const juce::String& sectionId)
{
    if (currentSectionId != sectionId)
    {
        currentSectionId = sectionId;
        updateContent();
        repaint();
    }
}

void SectionEditor::setupSectionNameLabel()
{
    // Configuration du label de nom de section
    sectionNameLabel.setJustificationType(juce::Justification::centredLeft);
    sectionNameLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
    
    // Application de la police via FontManager - taille légèrement plus grande
    auto sectionNameFontOptions = fontManager->getSFProDisplay(28.0f, FontManager::FontWeight::Bold);
    sectionNameLabel.setFont(juce::Font(sectionNameFontOptions));
    
    addAndMakeVisible(sectionNameLabel);
}

void SectionEditor::updateContent()
{
    if (currentSectionId.isEmpty())
    {
        sectionNameLabel.setText("No Section", juce::dontSendNotification);
    }
    else
    {
        // Extraire l'index de la section depuis l'ID (ex: "Section_0" -> "Section 1")
        juce::String displayName = "Section";
        if (currentSectionId.startsWith("Section_"))
        {
            int index = currentSectionId.getTrailingIntValue();
            displayName = "Section " + juce::String(index + 1);
        }
        
        sectionNameLabel.setText(displayName, juce::dontSendNotification);
    }
} 

void SectionEditor::drawBorder(juce::Graphics& g)
{
    // Logique inspirée de OutlineTextButton::drawButtonBackground
    auto bounds = getLocalBounds().toFloat().reduced(borderThickness * 0.5f);
    
    juce::Colour currentBorderColour = borderColour;
    
    // Gestion des états comme dans OutlineTextButton
    // Pour un éditeur, on peut simuler un état "actif" quand une section est sélectionnée
    if (isEditingSection())
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

void SectionEditor::drawSeparatorLine(juce::Graphics& g)
{
    // Dessiner un trait de séparation entre le header et le contenu
    auto separatorY = headerArea.getBottom(); // 10px de décalage depuis le header
    auto separatorStart = headerArea.getX();
    auto separatorEnd = headerArea.getRight();
    
    g.setColour(juce::Colours::darkblue.withAlpha(0.15f));
    g.drawHorizontalLine(separatorY, static_cast<float>(separatorStart), static_cast<float>(separatorEnd));
} 