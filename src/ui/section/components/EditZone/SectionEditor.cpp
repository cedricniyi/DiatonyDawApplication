#include "SectionEditor.h"
#include "utils/FontManager.h"

SectionEditor::SectionEditor()
    : ColoredPanel(juce::Colours::lightblue.withAlpha(0.25f)) // Bleu clair plus visible
{
    setupLabels();
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
}

void SectionEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // Disposition verticale simple
    titleLabel.setBounds(bounds.removeFromTop(40));
    bounds.removeFromTop(10); // Espacement
    contentLabel.setBounds(bounds.removeFromTop(60));
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

void SectionEditor::setupLabels()
{
    // Configuration du label de titre
    titleLabel.setJustificationType(juce::Justification::centredTop);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::darkslategrey);
    
    // Configuration du label de contenu
    contentLabel.setJustificationType(juce::Justification::centredTop);
    contentLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    
    // Application des polices via FontManager
    juce::SharedResourcePointer<FontManager> fontManager;
    
    auto titleFontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
    titleLabel.setFont(juce::Font(titleFontOptions));
    
    auto contentFontOptions = fontManager->getSFProText(16.0f, FontManager::FontWeight::Regular);
    contentLabel.setFont(juce::Font(contentFontOptions));
    
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(contentLabel);
}

void SectionEditor::updateContent()
{
    if (currentSectionId.isEmpty())
    {
        titleLabel.setText("Section Editor", juce::dontSendNotification);
        contentLabel.setText("No section selected", juce::dontSendNotification);
    }
    else
    {
        titleLabel.setText("Section Editor", juce::dontSendNotification);
        contentLabel.setText("Editing section: " + currentSectionId + "\n\n" +
                           "Upcoming editing features:\n" +
                           "• Key signature modification\n" +
                           "• Mode editing (major/minor)\n" +
                           "• Progression management\n" +
                           "• Advanced settings", 
                           juce::dontSendNotification);
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