#include "DiatonyComboBoxLookAndFeel.h"

DiatonyComboBoxLookAndFeel::DiatonyComboBoxLookAndFeel()
{
    // Couleurs par défaut harmonieuses avec InfoColoredPanel
    backgroundColour = juce::Colour(240, 240, 240);  // Gris clair
    borderColour = juce::Colour(200, 200, 200);      // Gris moyen
    textColour = juce::Colour(60, 60, 60);           // Gris foncé
    arrowColour = juce::Colour(120, 120, 120);       // Gris moyen-foncé
    highlightColour = juce::Colour(220, 220, 220);   // Gris très clair pour highlight
}

juce::String DiatonyComboBoxLookAndFeel::getDisplayText(const juce::ComboBox& comboBox) const
{
    if (shortDisplayMode)
    {
        int selectedId = comboBox.getSelectedId();
        auto it = shortTexts.find(selectedId);
        if (it != shortTexts.end())
            return it->second;
    }
    return comboBox.getText();
}

void DiatonyComboBoxLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                             bool isButtonDown, int buttonX, int buttonY,
                                             int buttonW, int buttonH,
                                             juce::ComboBox& comboBox)
{
    auto bounds = juce::Rectangle<float>(0, 0, width, height);
    
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    g.setColour(borderColour);
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, 1.0f);
    
    if (showArrow)
    {
        auto arrowArea = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH);
        
        g.setColour(borderColour.withAlpha(0.5f));
        g.drawVerticalLine(buttonX, 2.0f, height - 2.0f);
        
        drawArrow(g, arrowArea, arrowColour);
    }
}

int DiatonyComboBoxLookAndFeel::getComboBoxTextHeight(juce::ComboBox& comboBox)
{
    juce::Font font(juce::FontOptions(14.0f));
    return font.getHeight() + (textPadding * 2);
}

juce::Label* DiatonyComboBoxLookAndFeel::createComboBoxTextBox(juce::ComboBox& comboBox)
{
    auto* label = new juce::Label();
    label->setFont(juce::Font(juce::FontOptions(14.0f)));
    label->setJustificationType(juce::Justification::centredLeft);
    label->setBorderSize(juce::BorderSize<int>(textPadding, textPadding, textPadding, textPadding));
    return label;
}

void DiatonyComboBoxLookAndFeel::positionComboBoxText(juce::ComboBox& comboBox, juce::Label& labelToPosition)
{
    auto buttonWidth = showArrow ? (comboBox.getHeight() + 2) : 0;
    labelToPosition.setBounds(1, 1, comboBox.getWidth() - buttonWidth - 2, comboBox.getHeight() - 2);
    
    // Mettre à jour le texte court sans toucher à la sélection
    if (shortDisplayMode)
    {
        int selectedId = comboBox.getSelectedId();
        if (selectedId > 0)
        {
            auto it = shortTexts.find(selectedId);
            if (it != shortTexts.end())
                labelToPosition.setText(it->second, juce::dontSendNotification);
        }
    }
}

void DiatonyComboBoxLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                                  bool isSeparator, bool isActive, bool isHighlighted,
                                                  bool isTicked, bool hasSubMenu, const juce::String& text,
                                                  const juce::String& shortcutKeyText,
                                                  const juce::Drawable* icon, const juce::Colour* textColourToUse)
{
    if (isHighlighted)
    {
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
    g.setColour(backgroundColour);
    g.fillRect(0, 0, width, height);
}

void DiatonyComboBoxLookAndFeel::setBackgroundColour(juce::Colour colour) { backgroundColour = colour; }
void DiatonyComboBoxLookAndFeel::setBorderColour(juce::Colour colour) { borderColour = colour; }
void DiatonyComboBoxLookAndFeel::setTextColour(juce::Colour colour) { textColour = colour; }
void DiatonyComboBoxLookAndFeel::setArrowColour(juce::Colour colour) { arrowColour = colour; }

void DiatonyComboBoxLookAndFeel::drawArrow(juce::Graphics& g, juce::Rectangle<int> arrowArea, juce::Colour colour)
{
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
