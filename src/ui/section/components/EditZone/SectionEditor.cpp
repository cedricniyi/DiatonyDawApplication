#include "SectionEditor.h"
#include "utils/FontManager.h"

SectionEditor::SectionEditor()
{
    setupLabels();
    updateContent();
}

SectionEditor::~SectionEditor()
{
}

void SectionEditor::paint(juce::Graphics& g)
{
    // Arrière-plan avec une couleur légèrement différente pour indiquer l'état d'édition
    g.fillAll(juce::Colour::fromString("#f8f8f8ff"));
    
    // Bordure pour délimiter la zone d'édition
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds(), 1);
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