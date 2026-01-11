#include "ModeZone.h"
#include "utils/FontManager.h"

ModeZone::ModeZone()
    : BaseZone("Mode")
{
    setupButtons();
}

void ModeZone::setSelectedMode(Diatony::Mode mode)
{
    selectedMode = mode;
    for (size_t j = 0; j < modeButtons.size(); ++j)
        modeButtons[j]->setSelected(modes[j] == selectedMode);
    repaint();
}

Diatony::Mode ModeZone::getSelectedMode() const { return selectedMode; }

void ModeZone::resizeContent(const juce::Rectangle<int>& contentBounds)
{
    if (contentBounds.isEmpty())
        return;
    
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::column;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    flexBox.alignItems = juce::FlexBox::AlignItems::stretch;
    
    for (auto& button : modeButtons)
    {
        flexBox.items.add(juce::FlexItem(*button)
            .withFlex(1.0f)
            .withMargin(juce::FlexItem::Margin(2.0f)));
    }
    
    flexBox.performLayout(contentBounds.toFloat());
}

void ModeZone::setupButtons()
{
    for (int i = 0; i < 2; ++i)
    {
        auto modeName = DiatonyText::getModeName(modes[i]);
        
        modeButtons[i] = std::make_unique<SelectableStyledButton>(
            modeName,
            juce::Colours::lightgrey,
            juce::Colours::blue,
            16.0f,
            FontManager::FontWeight::Medium
        );
        
        modeButtons[i]->onClick = [i, this]() {
            selectedMode = modes[i];
            
            for (int j = 0; j < 2; ++j)
                modeButtons[j]->setSelected(j == i);
            
            if (onModeChanged)
                onModeChanged(selectedMode);
        };
        
        addAndMakeVisible(*modeButtons[i]);
    }
}
