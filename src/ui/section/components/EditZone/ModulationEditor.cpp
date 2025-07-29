#include "ModulationEditor.h"
#include "utils/FontManager.h"

ModulationEditor::ModulationEditor()
{
    setupLabels();
    updateContent();
}

ModulationEditor::~ModulationEditor()
{
}

void ModulationEditor::paint(juce::Graphics& g)
{
    // Arrière-plan avec une couleur légèrement différente pour indiquer l'état d'édition
    // Couleur verte claire pour différencier des sections
    g.fillAll(juce::Colour::fromString("#f0fff0ff"));
    
    // Bordure pour délimiter la zone d'édition
    g.setColour(juce::Colours::lightgreen);
    g.drawRect(getLocalBounds(), 1);
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