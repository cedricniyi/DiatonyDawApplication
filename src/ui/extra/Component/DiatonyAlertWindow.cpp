#include "DiatonyAlertWindow.h"

//==============================================================================
DiatonyAlertWindow::DiatonyAlertWindow(const juce::String& titleText,
                                      const juce::String& messageText,
                                      const juce::String& buttonText,
                                      std::function<void()> onCloseCallback)
    : title(titleText),
      message(messageText),
      buttonLabel(buttonText),
      onClose(std::move(onCloseCallback))
{
    // Configuration du bouton OK
    okButton.setButtonText(buttonLabel);
    okButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF666666));  // Bleu assorti
    okButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    okButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    okButton.onClick = [this]() {
        if (onClose) onClose();
        if (auto* parent = findParentComponentOfClass<juce::DialogWindow>())
            parent->exitModalState(0);
    };
    addAndMakeVisible(okButton);

    // Définir une taille minimale
    setSize(400, 200);
}

void DiatonyAlertWindow::paint(juce::Graphics& g)
{
    // Fond principal avec coins arrondis
    g.setColour(juce::Colour(0xFF2D2D2D));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
    
    // Barre de titre
    auto titleBounds = getLocalBounds().removeFromTop(50).toFloat();
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRoundedRectangle(titleBounds, 10.0f);
    g.fillRect(titleBounds.removeFromBottom(10.0f)); // Pour éviter l'arrondi en bas
    
    // Bordure
    g.setColour(juce::Colour(0xFF444444));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f), 10.0f, 1.0f);
    
    // Texte du titre
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText(title, titleBounds.reduced(15, 0), juce::Justification::centredLeft);
    
    // Message
    g.setFont(16.0f);
    auto messageBounds = getLocalBounds().reduced(20);
    messageBounds.removeFromTop(60); // Espace pour le titre
    messageBounds.removeFromBottom(60); // Espace pour le bouton
    g.drawText(message, messageBounds, juce::Justification::centred, true);
}

void DiatonyAlertWindow::resized()
{
    // Positionner le bouton OK en bas
    auto bounds = getLocalBounds();
    auto buttonBounds = bounds.removeFromBottom(60);
    okButton.setBounds(buttonBounds.reduced(20, 15));
}

void DiatonyAlertWindow::show(const juce::String& title,
                            const juce::String& message,
                            const juce::String& buttonText,
                            std::function<void()> onCloseCallback)
{
    auto* window = new DiatonyAlertWindow(title, message, buttonText, onCloseCallback);
    
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(window);
    options.dialogTitle = "";  // Titre vide car nous avons notre propre barre de titre
    options.dialogBackgroundColour = juce::Colours::transparentBlack;  // Fond transparent
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;
    
    // Créer et configurer la fenêtre
    auto* dialogWindow = options.create();
    dialogWindow->setUsingNativeTitleBar(false);
    dialogWindow->setTitleBarHeight(0);
    dialogWindow->setBounds(dialogWindow->getBounds().withSizeKeepingCentre(400, 200));
    dialogWindow->setDropShadowEnabled(true);
    
    // Lancer la fenêtre de manière asynchrone
    dialogWindow->enterModalState(true,
        juce::ModalCallbackFunction::create([dialogWindow](int) {
            delete dialogWindow;
        }), true);
} 