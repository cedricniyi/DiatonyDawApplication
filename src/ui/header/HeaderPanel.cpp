#include "HeaderPanel.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"
#include "ui/UIStateIdentifiers.h"
#include "ui/PluginEditor.h"
#include "controller/AppController.h"
#include "utils/FontManager.h"
#include "IconBinaryData.h"

HeaderPanel::HeaderPanel()
    : ColoredPanel(juce::Colour::fromString("#FF333333")),
      generateButton(juce::String::fromUTF8("Generate"),
                     juce::Colour::fromString("#ff22c55e"),  // Vert
                     juce::Colour::fromString("#ff16a34a"),  // Vert foncé hover
                     14.0f, FontManager::FontWeight::Medium)
{
    loadLogo();
    
    mainLabel.setText(juce::String::fromUTF8("DiatonyDAW"), juce::dontSendNotification);
    mainLabel.setJustificationType(juce::Justification::centredLeft);
    mainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    
    auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
    mainLabel.setFont(juce::Font(fontOptions));
    addAndMakeVisible(mainLabel);

    generateButton.setTooltip(juce::String::fromUTF8("Générer une solution musicale"));
    addAndMakeVisible(generateButton);

    addAndMakeVisible(midiDragZone);
    
    hamburgerButton = std::make_unique<IconStyledButton>(
        "HistoryButton",
        IconData::historysvgrepo_svg,
        IconData::historysvgrepo_svgSize,
        juce::Colour::fromString("ff808080"),
        juce::Colour::fromString("ff606060"),
        juce::Colours::white
    );
    hamburgerButton->setTooltip("Ouvrir/fermer le panneau History");
    addAndMakeVisible(*hamburgerButton);
}

HeaderPanel::~HeaderPanel()
{
    if (appState.isValid())
        appState.removeListener(this);
}

void HeaderPanel::setAppState(juce::ValueTree& state)
{
    if (appState.isValid())
        appState.removeListener(this);
        
    appState = state;
    appState.addListener(this);
    
    hamburgerButton->onClick = [this]() {
        if (appState.isValid())
        {
            bool currentVisible = static_cast<bool>(
                appState.getProperty(UIStateIdentifiers::historyPanelVisible, false)
            );
            appState.setProperty(UIStateIdentifiers::historyPanelVisible, !currentVisible, nullptr);
        }
    };
    
    updateDockState();
}

void HeaderPanel::resized()
{
    auto bounds = getLocalBounds();
    
    juce::GlyphArrangement ga;
    ga.addLineOfText(mainLabel.getFont(), mainLabel.getText(), 0, 0);
    auto labelWidth = static_cast<int>(std::ceil(ga.getBoundingBox(0, -1, false).getWidth()));
    
    titleZoneWidth = LOGO_SIZE + 8 + labelWidth + 40;
    
    auto titleArea = bounds.removeFromLeft(titleZoneWidth).reduced(16, 10);
    titleArea.removeFromLeft(LOGO_SIZE + 8);
    mainLabel.setBounds(titleArea);
    
    auto buttonArea = bounds.reduced(20, 10);
    auto buttonSize = buttonArea.getHeight();
    
    juce::FlexBox buttonFlex;
    buttonFlex.flexDirection = juce::FlexBox::Direction::row;
    buttonFlex.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    buttonFlex.alignItems = juce::FlexBox::AlignItems::center;
    
    buttonFlex.items.add(juce::FlexItem(generateButton)
        .withMinWidth(80.0f)
        .withMinHeight(static_cast<float>(buttonSize))
        .withMargin(juce::FlexItem::Margin(0, 12, 0, 0)));
    
    buttonFlex.items.add(juce::FlexItem(midiDragZone)
        .withMinWidth(60.0f)
        .withMinHeight(static_cast<float>(buttonSize))
        .withMargin(juce::FlexItem::Margin(0, 12, 0, 0)));
    
    buttonFlex.items.add(juce::FlexItem(*hamburgerButton)
        .withMinWidth(static_cast<float>(buttonSize))
        .withMinHeight(static_cast<float>(buttonSize)));
    
    buttonFlex.performLayout(buttonArea);
}

void HeaderPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Zone titre (gauche) - plus foncée
    auto titleZone = bounds.removeFromLeft(titleZoneWidth);
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRect(titleZone);
    
    // Zone boutons (droite) - plus claire
    g.setColour(juce::Colour(0xFF2A2A2A));
    g.fillRect(bounds);
    
    // Bordure bas
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
    
    // Séparation verticale
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(titleZoneWidth, 0, 1, getHeight());
    
    // Logo Diatony
    if (logoDrawable != nullptr)
    {
        int logoX = 16;
        int logoY = (getHeight() - LOGO_SIZE) / 2;
        auto logoBounds = juce::Rectangle<float>(
            static_cast<float>(logoX), 
            static_cast<float>(logoY), 
            static_cast<float>(LOGO_SIZE), 
            static_cast<float>(LOGO_SIZE)
        );
        logoDrawable->drawWithin(g, logoBounds, juce::RectanglePlacement::centred, 1.0f);
    }
}

void HeaderPanel::parentHierarchyChanged()
{
    ColoredPanel::parentHierarchyChanged();
    findAppController();
}

void HeaderPanel::findAppController()
{
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
        connectGenerateButton();
        midiDragZone.setSelectionState(appController->getSelectionState());
    }
    else
    {
        appController = nullptr;
    }
}

void HeaderPanel::connectGenerateButton()
{
    if (!appController)
        return;
    
    generateButton.onClick = [this]() {
        if (appController)
            appController->startGeneration();
    };
}

void HeaderPanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                          const juce::Identifier& property)
{
    if (property == UIStateIdentifiers::dockVisible)
        updateDockState();
}

void HeaderPanel::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void HeaderPanel::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void HeaderPanel::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void HeaderPanel::valueTreeParentChanged(juce::ValueTree&) {}

void HeaderPanel::updateDockState() {}

void HeaderPanel::loadLogo()
{
    auto svgString = juce::String::fromUTF8(IconData::diatony_logo_svg, 
                                            IconData::diatony_logo_svgSize);
    auto svgXml = juce::XmlDocument::parse(svgString);
    
    if (svgXml == nullptr)
        return;
    
    logoDrawable = juce::Drawable::createFromSVG(*svgXml);
    
    if (logoDrawable != nullptr)
        logoDrawable->replaceColour(juce::Colours::black, juce::Colours::white);
}
