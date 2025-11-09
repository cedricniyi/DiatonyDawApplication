#include "DiatonyComboBoxLookAndFeel.h"

//==============================================================================
DiatonyComboBoxLookAndFeel::DiatonyComboBoxLookAndFeel()
{
    // Couleurs par défaut harmonieuses avec InfoColoredPanel
    backgroundColour = juce::Colour(240, 240, 240);  // Gris clair
    borderColour = juce::Colour(200, 200, 200);      // Gris moyen
    textColour = juce::Colour(60, 60, 60);           // Gris foncé
    arrowColour = juce::Colour(120, 120, 120);       // Gris moyen-foncé
    highlightColour = juce::Colour(220, 220, 220);   // Gris très clair pour highlight
}

void DiatonyComboBoxLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                             bool isButtonDown, int buttonX, int buttonY,
                                             int buttonW, int buttonH,
                                             juce::ComboBox& comboBox)
{
    auto bounds = juce::Rectangle<float>(0, 0, width, height);
    
    // Fond arrondi simple
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Contour subtil
    g.setColour(borderColour);
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, 1.0f);
    
    // Zone de la flèche
    auto arrowArea = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH);
    
    // Séparateur vertical léger entre texte et flèche
    g.setColour(borderColour.withAlpha(0.5f));
    g.drawVerticalLine(buttonX, 2.0f, height - 2.0f);
    
    // Flèche
    drawArrow(g, arrowArea, arrowColour);
}

int DiatonyComboBoxLookAndFeel::getComboBoxTextHeight(juce::ComboBox& comboBox)
{
    // Utiliser une police standard pour calculer la hauteur
    juce::Font font(juce::FontOptions(14.0f));
    return font.getHeight() + (textPadding * 2);
}

juce::Label* DiatonyComboBoxLookAndFeel::createComboBoxTextBox(juce::ComboBox& comboBox)
{
    auto* label = new juce::Label();
    
    // Ne PAS définir les couleurs ici - elles seront définies via setColour sur le ComboBox
    // Le label utilisera automatiquement ComboBox::textColourId du parent
    
    // Configuration du style
    label->setFont(juce::Font(juce::FontOptions(14.0f)));
    label->setJustificationType(juce::Justification::centredLeft);
    label->setBorderSize(juce::BorderSize<int>(textPadding, textPadding, textPadding, textPadding));
    
    return label;
}

void DiatonyComboBoxLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                                  bool isSeparator, bool isActive, bool isHighlighted,
                                                  bool isTicked, bool hasSubMenu, const juce::String& text,
                                                  const juce::String& shortcutKeyText,
                                                  const juce::Drawable* icon, const juce::Colour* textColourToUse)
{
    if (isHighlighted)
    {
        // Couleur de hover : couleur du composant légèrement plus foncée
        g.setColour(backgroundColour.darker(0.1f));
        g.fillRect(area);
    }
    
    g.setColour(textColour);
    g.setFont(juce::Font(juce::FontOptions(14.0f)));
    
    auto textArea = area.reduced(textPadding, 0);
    g.drawFittedText(text, textArea, juce::Justification::centredLeft, 1);
}

void DiatonyComboBoxLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    // Fond simple du menu déroulant - pas d'arrondi
    g.setColour(backgroundColour);
    g.fillRect(0, 0, width, height);
}

void DiatonyComboBoxLookAndFeel::setBackgroundColour(juce::Colour colour)
{
    backgroundColour = colour;
}

void DiatonyComboBoxLookAndFeel::setBorderColour(juce::Colour colour)
{
    borderColour = colour;
}

void DiatonyComboBoxLookAndFeel::setTextColour(juce::Colour colour)
{
    textColour = colour;
}

void DiatonyComboBoxLookAndFeel::setArrowColour(juce::Colour colour)
{
    arrowColour = colour;
}

void DiatonyComboBoxLookAndFeel::drawArrow(juce::Graphics& g, juce::Rectangle<int> arrowArea, juce::Colour colour)
{
    // Flèche simple vers le bas
    juce::Path arrow;
    
    auto centerX = arrowArea.getCentreX();
    auto centerY = arrowArea.getCentreY();
    auto arrowSize = 4.0f;
    
    arrow.addTriangle(centerX - arrowSize, centerY - arrowSize * 0.5f,
                     centerX + arrowSize, centerY - arrowSize * 0.5f,
                     centerX, centerY + arrowSize * 0.5f);
    
    g.setColour(colour);
    g.fillPath(arrow);
}