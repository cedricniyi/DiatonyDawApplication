#include "DiatonyAlertWindow.h"
#include "utils/DiatonyConstants.h"

DiatonyAlertWindow::DiatonyAlertWindow(AlertType type,
                                      const juce::String& titleText,
                                      const juce::String& messageText,
                                      const juce::String& buttonText,
                                      std::function<void()> onCloseCallback)
    : alertType(type),
      title(titleText),
      message(messageText),
      onClose(std::move(onCloseCallback))
{
    auto accentColour = getAccentColour();
    
    iconComponent = std::make_unique<IconComponent>(type, accentColour);
    addAndMakeVisible(iconComponent.get());
    
    titleComponent = std::make_unique<TitleComponent>(titleText);
    addAndMakeVisible(titleComponent.get());
    
    messageComponent = std::make_unique<MessageComponent>(messageText);
    addAndMakeVisible(messageComponent.get());
    
    auto highlightColour = accentColour.brighter(0.2f);
    okButton = std::make_unique<StyledButton>(
        buttonText,
        accentColour,
        highlightColour,
        16.0f,
        FontManager::FontWeight::Semibold
    );
    
    okButton->onClick = [this]() {
        if (onClose) onClose();
        if (auto* parent = findParentComponentOfClass<juce::DialogWindow>())
            parent->exitModalState(0);
    };
    
    if (buttonText.isEmpty())
        okButton->setVisible(false);
    else
        addAndMakeVisible(okButton.get());

    setSize(450, 340);
    setOpaque(false);
}

void DiatonyAlertWindow::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto accentColour = getAccentColour();
    
    // Fond avec gradient subtil (blanc → gris très clair)
    g.setGradientFill(juce::ColourGradient(
        juce::Colours::white, bounds.getTopLeft(),
        juce::Colour(0xFFF8F9FA), bounds.getBottomLeft(),
        false
    ));
    g.fillRoundedRectangle(bounds, 12.0f);
    
    // Bordure colorée selon le type d'alerte
    g.setColour(accentColour.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 2.5f);
}

void DiatonyAlertWindow::resized()
{
    auto bounds = getLocalBounds().reduced(30);
    
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.alignItems = juce::FlexBox::AlignItems::center;
    
    fb.items.add(juce::FlexItem(*iconComponent)
        .withWidth(80.0f).withHeight(80.0f).withMargin({20, 0, 0, 0}));
    
    fb.items.add(juce::FlexItem(*titleComponent)
        .withWidth(bounds.getWidth()).withHeight(40.0f).withMargin({15, 0, 0, 0}));
    
    fb.items.add(juce::FlexItem(*messageComponent)
        .withWidth(bounds.getWidth()).withFlex(1.0f).withMargin({15, 0, 0, 0}));
    
    fb.items.add(juce::FlexItem(*okButton)
        .withWidth(200.0f).withHeight(40.0f).withMargin({15, 0, 20, 0}));
    
    fb.performLayout(bounds);
}

juce::DialogWindow* DiatonyAlertWindow::showWithHandle(AlertType type,
                                                      const juce::String& title,
                                                      const juce::String& message,
                                                      const juce::String& buttonText,
                                                      std::function<void()> onCloseCallback,
                                                      juce::Component* parentComponent)
{
    auto alertWindow = std::make_unique<DiatonyAlertWindow>(type, title, message, buttonText, onCloseCallback);
    auto* wrapper = new DiatonyAlertWindowWithOverlay(std::move(alertWindow));
    
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(wrapper);
    options.dialogTitle = "";
    options.dialogBackgroundColour = juce::Colours::transparentBlack;
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;
    
    auto* dialogWindow = options.create();
    dialogWindow->setUsingNativeTitleBar(false);
    dialogWindow->setTitleBarHeight(0);
    dialogWindow->setOpaque(false);
    
    // Trouver le composant top-level pour centrage correct
    juce::Component* topLevelComp = nullptr;
    
    if (parentComponent != nullptr)
        topLevelComp = parentComponent->getTopLevelComponent();
    
    if (topLevelComp == nullptr)
        if (auto* activeWindow = juce::TopLevelWindow::getActiveTopLevelWindow())
            topLevelComp = activeWindow;
    
    if (topLevelComp != nullptr)
        dialogWindow->centreAroundComponent(topLevelComp, dialogWindow->getWidth(), dialogWindow->getHeight());
    else
        dialogWindow->centreWithSize(dialogWindow->getWidth(), dialogWindow->getHeight());
    
    dialogWindow->setDropShadowEnabled(false);
    dialogWindow->setVisible(true);
    dialogWindow->toFront(true);
    dialogWindow->setAlwaysOnTop(true);
    
    dialogWindow->repaint();
    if (auto* content = dialogWindow->getContentComponent())
        content->repaint();
    
    // Entrée modale différée pour laisser le temps au rendu
    juce::MessageManager::callAsync([dialogWindow, onCloseCallback]() {
        dialogWindow->enterModalState(true,
            juce::ModalCallbackFunction::create([dialogWindow, onCloseCallback](int) {
                if (onCloseCallback)
                    onCloseCallback();
                delete dialogWindow;
            }), true);
    });
    
    return dialogWindow;
}

void DiatonyAlertWindow::show(AlertType type,
                            const juce::String& title,
                            const juce::String& message,
                            const juce::String& buttonText,
                            std::function<void()> onCloseCallback,
                            juce::Component* parentComponent)
{
    showWithHandle(type, title, message, buttonText, onCloseCallback, parentComponent);
}

void DiatonyAlertWindow::IconComponent::paint(juce::Graphics& g)
{
    auto iconArea = getLocalBounds().toFloat();
    
    g.setColour(accentColour.withAlpha(0.15f));
    g.fillEllipse(iconArea);
    
    g.setColour(accentColour.withAlpha(0.5f));
    g.drawEllipse(iconArea.reduced(1.0f), 2.0f);
    
    auto center = iconArea.getCentre();
    g.setColour(accentColour);
    
    switch (alertType)
    {
        case AlertType::Success:
        {
            juce::Path checkmark;
            auto size = iconArea.getWidth() * 0.5f;
            checkmark.startNewSubPath(center.x - size * 0.3f, center.y);
            checkmark.lineTo(center.x - size * 0.1f, center.y + size * 0.25f);
            checkmark.lineTo(center.x + size * 0.3f, center.y - size * 0.25f);
            g.strokePath(checkmark, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            break;
        }
        
        case AlertType::Error:
        {
            auto size = iconArea.getWidth() * 0.25f;
            juce::Line<float> line1(center.x - size, center.y - size, center.x + size, center.y + size);
            juce::Line<float> line2(center.x + size, center.y - size, center.x - size, center.y + size);
            g.drawLine(line1, 3.0f);
            g.drawLine(line2, 3.0f);
            break;
        }
        
        case AlertType::Warning:
        {
            g.setFont(juce::Font(fontManager->getSFProDisplay(30.0f, FontManager::FontWeight::Bold)));
            g.drawText("!", iconArea.toNearestInt(), juce::Justification::centred, false);
            break;
        }
        
        case AlertType::Info:
        {
            g.setFont(juce::Font(fontManager->getSFProDisplay(28.0f, FontManager::FontWeight::Bold)));
            g.drawText("i", iconArea.toNearestInt(), juce::Justification::centred, false);
            break;
        }
    }
}

void DiatonyAlertWindow::TitleComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.setFont(juce::Font(fontManager->getSFProDisplay(22.0f, FontManager::FontWeight::Bold)));
    g.drawText(titleText, getLocalBounds(), juce::Justification::centred, true);
}

void DiatonyAlertWindow::MessageComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xFF4A5568));
    g.setFont(juce::Font(fontManager->getSFProText(15.0f, FontManager::FontWeight::Regular)));
    g.drawText(messageText, getLocalBounds(), juce::Justification::centred, true);
}

void DiatonyAlertWindow::drawIcon(juce::Graphics& g, juce::Rectangle<float> iconArea)
{
    // Non utilisée, gardée pour compatibilité
}

juce::Colour DiatonyAlertWindow::getAccentColour() const
{
    switch (alertType)
    {
        case AlertType::Success:  return juce::Colour(0xFF4CAF50);  // Vert
        case AlertType::Error:    return juce::Colour(0xFFF44336);  // Rouge
        case AlertType::Warning:  return juce::Colour(0xFFFF9800);  // Orange
        case AlertType::Info:     return juce::Colour(0xFF2196F3);  // Bleu
        default:                  return juce::Colour(0xFF2196F3);
    }
}
