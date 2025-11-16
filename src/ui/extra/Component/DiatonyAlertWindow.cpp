#include "DiatonyAlertWindow.h"
#include "utils/DiatonyConstants.h"

//==============================================================================
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
    
    // Créer l'icône
    iconComponent = std::make_unique<IconComponent>(type, accentColour);
    addAndMakeVisible(iconComponent.get());
    
    // Créer le titre
    titleComponent = std::make_unique<TitleComponent>(titleText);
    addAndMakeVisible(titleComponent.get());
    
    // Créer le message
    messageComponent = std::make_unique<MessageComponent>(messageText);
    addAndMakeVisible(messageComponent.get());
    
    // Créer le bouton avec StyledButton
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
    
    // Cacher le bouton si le texte est vide (popup de chargement non-fermable)
    if (buttonText.isEmpty())
    {
        okButton->setVisible(false);
    }
    else
    {
        addAndMakeVisible(okButton.get());
    }

    // Taille fixe pour un pop-up harmonieux
    setSize(450, 340);
    
    // Rendre le composant non-opaque pour les coins arrondis
    setOpaque(false);
}

void DiatonyAlertWindow::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto accentColour = getAccentColour();
    
    // ===== FOND ET BORDURE (MODE CLAIR) =====
    // Fond principal clair avec gradient subtil (blanc → gris très clair)
    g.setGradientFill(juce::ColourGradient(
        juce::Colours::white, bounds.getTopLeft(),
        juce::Colour(0xFFF8F9FA), bounds.getBottomLeft(),
        false
    ));
    g.fillRoundedRectangle(bounds, 12.0f);
    
    // Bordure colorée selon le type d'alerte (plus subtile)
    g.setColour(accentColour.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 2.5f);
}

void DiatonyAlertWindow::resized()
{
    auto bounds = getLocalBounds().reduced(30);  // Padding global
    
    // ===== LAYOUT AVEC FLEXBOX VERTICAL =====
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.alignItems = juce::FlexBox::AlignItems::center;
    
    // Icône en haut (80x80px)
    fb.items.add(juce::FlexItem(*iconComponent)
        .withWidth(80.0f)
        .withHeight(80.0f)
        .withMargin({20, 0, 0, 0}));
    
    // Titre sous l'icône (hauteur fixe)
    fb.items.add(juce::FlexItem(*titleComponent)
        .withWidth(bounds.getWidth())
        .withHeight(40.0f)
        .withMargin({15, 0, 0, 0}));
    
    // Message au centre (prend l'espace restant avec flex)
    fb.items.add(juce::FlexItem(*messageComponent)
        .withWidth(bounds.getWidth())
        .withFlex(1.0f)  // Prend l'espace restant
        .withMargin({15, 0, 0, 0}));
    
    // Bouton en bas (hauteur fixe, largeur fixe 200px)
    fb.items.add(juce::FlexItem(*okButton)
        .withWidth(200.0f)
        .withHeight(40.0f)
        .withMargin({15, 0, 20, 0}));
    
    fb.performLayout(bounds);
}

juce::DialogWindow* DiatonyAlertWindow::showWithHandle(AlertType type,
                                                      const juce::String& title,
                                                      const juce::String& message,
                                                      const juce::String& buttonText,
                                                      std::function<void()> onCloseCallback,
                                                      juce::Component* parentComponent)
{
    // Créer le pop-up
    auto alertWindow = std::make_unique<DiatonyAlertWindow>(type, title, message, buttonText, onCloseCallback);
    
    // Créer le wrapper avec overlay
    auto* wrapper = new DiatonyAlertWindowWithOverlay(std::move(alertWindow));
    
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(wrapper);
    options.dialogTitle = "";  // Titre vide car nous avons notre propre barre de titre
    options.dialogBackgroundColour = juce::Colours::transparentBlack;  // ✅ TRANSPARENT pour voir les coins arrondis
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;
    
    // Créer et configurer la fenêtre
    auto* dialogWindow = options.create();
    dialogWindow->setUsingNativeTitleBar(false);
    dialogWindow->setTitleBarHeight(0);
    
    // ✅ Rendre le fond de la DialogWindow complètement transparent
    dialogWindow->setOpaque(false);
    
    // === PARENTAGE CORRECT POUR PLUGINS ===
    // Trouver le vrai composant top-level (pas juste la fenêtre active du système)
    juce::Component* topLevelComp = nullptr;
    
    // 1. Priorité : utiliser le parentComponent fourni (recommandé pour plugins)
    if (parentComponent != nullptr)
    {
        topLevelComp = parentComponent->getTopLevelComponent();
        DBG("DiatonyAlertWindow: Parent fourni, utilisation de getTopLevelComponent()");
    }
    
    // 2. Fallback : chercher la fenêtre active (pour standalone sans parent)
    if (topLevelComp == nullptr)
    {
        if (auto* activeWindow = juce::TopLevelWindow::getActiveTopLevelWindow())
        {
            topLevelComp = activeWindow;
            DBG("DiatonyAlertWindow: Fallback sur getActiveTopLevelWindow()");
        }
    }
    
    // 3. Positionner et centrer la fenêtre dans le parent
    if (topLevelComp != nullptr)
    {
        // Centrer le popup dans le parent (et non pas prendre toute sa taille !)
        dialogWindow->centreAroundComponent(topLevelComp, dialogWindow->getWidth(), dialogWindow->getHeight());
        DBG("DiatonyAlertWindow: Popup centré dans le parent avec succès");
    }
    else
    {
        // Fallback final : centrer à l'écran
        dialogWindow->centreWithSize(dialogWindow->getWidth(), dialogWindow->getHeight());
        DBG("DiatonyAlertWindow: Fallback sur centrage écran");
    }
    
    dialogWindow->setDropShadowEnabled(false);  // Pas besoin, on a déjà une ombre dans le composant
    
    // ⚠️ CRITIQUE : Rendre visible et forcer le repaint AVANT enterModalState
    dialogWindow->setVisible(true);
    dialogWindow->toFront(true);
    dialogWindow->setAlwaysOnTop(true);
    
    // Forcer un repaint complet pour éviter l'affichage partiel
    dialogWindow->repaint();
    if (auto* content = dialogWindow->getContentComponent())
        content->repaint();
    
    // Différer légèrement l'entrée en mode modal pour laisser le temps au rendu
    juce::MessageManager::callAsync([dialogWindow, onCloseCallback]() {
        // Lancer la fenêtre de manière asynchrone (non-bloquant)
        // ⚠️ IMPORTANT : Appeler le callback AVANT de delete pour nettoyer les références
        dialogWindow->enterModalState(true,
            juce::ModalCallbackFunction::create([dialogWindow, onCloseCallback](int) {
                // Appeler le callback de fermeture (pour nettoyer loadingPopup)
                if (onCloseCallback)
                    onCloseCallback();
                
                // Puis supprimer la fenêtre
                delete dialogWindow;
            }), true);
    });
    
    // Retourner le pointeur pour que l'appelant puisse fermer la fenêtre
    return dialogWindow;
}

void DiatonyAlertWindow::show(AlertType type,
                            const juce::String& title,
                            const juce::String& message,
                            const juce::String& buttonText,
                            std::function<void()> onCloseCallback,
                            juce::Component* parentComponent)
{
    // Utilise showWithHandle mais ignore le retour
    showWithHandle(type, title, message, buttonText, onCloseCallback, parentComponent);
}

//==============================================================================
// IMPLÉMENTATION DES SOUS-COMPOSANTS
//==============================================================================

void DiatonyAlertWindow::IconComponent::paint(juce::Graphics& g)
{
    auto iconArea = getLocalBounds().toFloat();
    
    // Cercle de fond avec transparence
    g.setColour(accentColour.withAlpha(0.15f));
    g.fillEllipse(iconArea);
    
    // Bordure du cercle
    g.setColour(accentColour.withAlpha(0.5f));
    g.drawEllipse(iconArea.reduced(1.0f), 2.0f);
    
    // Symbole au centre
    auto center = iconArea.getCentre();
    g.setColour(accentColour);
    
    switch (alertType)
    {
        case AlertType::Success:
        {
            // Checkmark ✓
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
            // X (réduit pour être plus discret)
            auto size = iconArea.getWidth() * 0.25f;  // Réduit de 0.35f à 0.25f
            juce::Line<float> line1(center.x - size, center.y - size, center.x + size, center.y + size);
            juce::Line<float> line2(center.x + size, center.y - size, center.x - size, center.y + size);
            g.drawLine(line1, 3.0f);  // Trait légèrement plus fin aussi (3.0f au lieu de 3.5f)
            g.drawLine(line2, 3.0f);
            break;
        }
        
        case AlertType::Warning:
        {
            // Triangle d'avertissement !
            g.setFont(juce::Font(fontManager->getSFProDisplay(30.0f, FontManager::FontWeight::Bold)));
            g.drawText("!", iconArea.toNearestInt(), juce::Justification::centred, false);
            break;
        }
        
        case AlertType::Info:
        {
            // i (information)
            g.setFont(juce::Font(fontManager->getSFProDisplay(28.0f, FontManager::FontWeight::Bold)));
            g.drawText("i", iconArea.toNearestInt(), juce::Justification::centred, false);
            break;
        }
    }
}

void DiatonyAlertWindow::TitleComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xFF1A1A1A));  // Texte sombre
    g.setFont(juce::Font(fontManager->getSFProDisplay(22.0f, FontManager::FontWeight::Bold)));
    g.drawText(titleText, getLocalBounds(), juce::Justification::centred, true);
}

void DiatonyAlertWindow::MessageComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xFF4A5568));  // Gris foncé
    g.setFont(juce::Font(fontManager->getSFProText(15.0f, FontManager::FontWeight::Regular)));
    g.drawText(messageText, getLocalBounds(), juce::Justification::centred, true);
}

//==============================================================================
// MÉTHODES PRIVÉES
//==============================================================================

void DiatonyAlertWindow::drawIcon(juce::Graphics& g, juce::Rectangle<float> iconArea)
{
    // Cette fonction n'est plus utilisée, gardée pour compatibilité
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