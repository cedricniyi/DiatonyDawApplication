#include "MainContentComponent.h"
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "footer/FooterPanel.h"
#include "history/HistoryPanel.h"
#include "UIStateIdentifiers.h"
#include "extra/Component/DiatonyAlertWindow.h"
#include "PluginEditor.h"
#include "controller/AppController.h"

//==============================================================================
MainContentComponent::MainContentComponent() 
    : appState(),
      headerPanel(),
      sectionPanel(),
      footerPanel(),
      historyPanel(),
      headerFlex(7.5f),
      sectionFlex(10.5f),
      footerFlex(15.0f)
{        
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(sectionPanel);
    // Footer masqué temporairement
    addChildComponent(footerPanel);
    
    // History Panel (visible mais largeur animée)
    addAndMakeVisible(historyPanel);
    
    // Drag & Drop overlay (invisible par défaut)
    addChildComponent(dragOverlay);
    dragOverlay.setAlwaysOnTop(true);
}

MainContentComponent::~MainContentComponent()
{
    if (appState.isValid())
        appState.removeListener(this);
    
    if (selectionState.isValid())
        selectionState.removeListener(this);
    
    // Fermer le popup actif si encore ouvert
    closePopup();
}

void MainContentComponent::setAppState(juce::ValueTree& state)
{
    // Déconnexion de l'ancien listener
    if (appState.isValid())
        appState.removeListener(this);
        
    // Connexion au nouveau ValueTree
    appState = state;
    appState.addListener(this);
    
    // Propagation de l'état aux panels enfants
    headerPanel.setAppState(appState);
    footerPanel.setAppState(appState);
    historyPanel.setAppState(appState);
    // sectionPanel.setAppState(appState);  // Si nécessaire plus tard
}

void MainContentComponent::setSelectionState(juce::ValueTree& state)
{
    // Déconnexion de l'ancien listener
    if (selectionState.isValid())
        selectionState.removeListener(this);
        
    // Connexion au selectionState pour écouter les changements de génération
    selectionState = state;
    selectionState.addListener(this);
}

void MainContentComponent::paint(juce::Graphics& g)
{   
    // Fond uni gris #5b5b5b
    g.fillAll(juce::Colour::fromString("#FF2A2A2A"));
}

void MainContentComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // === LAYOUT HORIZONTAL : Zone principale | History Panel ===
    // Calculer la largeur du panneau History (animée de 0 à 250px)
    constexpr int HISTORY_PANEL_MAX_WIDTH = 250;
    int historyWidth = static_cast<int>(HISTORY_PANEL_MAX_WIDTH * historyPanel.getWidthFraction());
    
    // History Panel à droite (largeur animée)
    if (historyWidth > 0)
    {
        auto historyBounds = bounds.removeFromRight(historyWidth);
        historyPanel.setBounds(historyBounds);
        historyPanel.setVisible(true);
    }
    else
    {
        historyPanel.setBounds(0, 0, 0, 0);
        historyPanel.setVisible(false);
    }
    
    // Sauvegarder la zone principale pour l'overlay (sans le HistoryPanel)
    auto mainAreaBounds = bounds;
    
    // === ZONE PRINCIPALE (header + section + footer) ===
    int padding = 8;
    
    // Header avec hauteur FIXE de 60px (ne se compresse pas)
    constexpr int HEADER_HEIGHT = 60;
    headerPanel.setBounds(bounds.removeFromTop(HEADER_HEIGHT));
    
    // Le reste avec padding
    auto content = bounds.reduced(padding, 0);  // Padding horizontal seulement
    content.removeFromTop(4);  // Petit espace après le header

    // Layout vertical avec FlexBox pour le reste
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;

    fb.items = {
        juce::FlexItem(sectionPanel).withFlex(1.0f)
        // Footer masqué temporairement
    };

    fb.performLayout(content);
    
    // Redimensionner l'overlay popup s'il est actif
    if (activePopup != nullptr)
    {
        activePopup->setBounds(getLocalBounds());
    }
    
    // Positionner l'overlay de drag & drop UNIQUEMENT sur la zone principale (pas sur HistoryPanel)
    dragOverlay.setBounds(mainAreaBounds);
}

float& MainContentComponent::getHeaderFlexRef()
{
    return headerFlex;
}

float& MainContentComponent::getSectionFlexRef()
{
    return sectionFlex;
}

float& MainContentComponent::getFooterFlexRef()
{
    return footerFlex;
}

FooterPanel& MainContentComponent::getFooterPanel()
{
    return footerPanel;
}

HeaderPanel& MainContentComponent::getHeaderPanel()
{
    return headerPanel;
}

SectionPanel& MainContentComponent::getSectionPanel()
{
    return sectionPanel;
}

HistoryPanel& MainContentComponent::getHistoryPanel()
{
    return historyPanel;
}

void MainContentComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                   const juce::Identifier& property)
{
    // === GÉNÉRATION : Écoute du statut pour afficher les pop-ups (depuis selectionState) ===
    if (treeWhosePropertyHasChanged == selectionState && property == juce::Identifier("generationStatus"))
    {
        auto status = treeWhosePropertyHasChanged.getProperty("generationStatus").toString();
        
        if (status == "generating")
        {
            // Afficher un popup de chargement (sans bouton, non fermable)
            DBG("🔄 MainContentComponent: Génération en cours...");
            
            juce::MessageManager::callAsync([this]() {
                showPopup(
                    DiatonyAlertWindow::AlertType::Info,
                    juce::String::fromUTF8("Génération en cours"),
                    juce::String::fromUTF8("Diatony recherche une solution musicale...\n\nVeuillez patienter, cela peut prendre quelques secondes."),
                    ""  // Pas de bouton → non fermable par l'utilisateur
                );
            });
        }
        else if (status == "completed")
        {
            // ✅ Succès : Fermer le popup de chargement et afficher le succès
            DBG("✅ MainContentComponent: Génération réussie !");
            
            juce::MessageManager::callAsync([this]() {
                showPopup(
                    DiatonyAlertWindow::AlertType::Success,
                    juce::String::fromUTF8("Génération Réussie"),
                    juce::String::fromUTF8("Le fichier MIDI a été généré avec succès !\n\nLa solution a été trouvée par le solveur Diatony."),
                    "OK"
                );
            });
        }
        else if (status == "error")
        {
            // ❌ Erreur : Fermer le popup de chargement et afficher l'erreur
            juce::String errorMessage = treeWhosePropertyHasChanged
                                            .getProperty("generationError")
                                            .toString();
            
            DBG("❌ MainContentComponent: Erreur de génération - " << errorMessage);
            
            juce::MessageManager::callAsync([this, errorMessage]() {
                showPopup(
                    DiatonyAlertWindow::AlertType::Error,
                    juce::String::fromUTF8("Échec de la Génération"),
                    juce::String::fromUTF8("Le solveur n'a pas pu trouver de solution :\n\n") + errorMessage,
                    "OK"
                );
            });
        }
    }
    
    // === LAYOUT : Notification du RootAnimator (depuis appState) ===
    if (treeWhosePropertyHasChanged == appState && property == UIStateIdentifiers::interactivePianoVisible)
    {
        if (onLayoutAnimationNeeded)
        {
            onLayoutAnimationNeeded();
        }
    }
}

void MainContentComponent::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void MainContentComponent::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void MainContentComponent::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void MainContentComponent::valueTreeParentChanged(juce::ValueTree&) {}

//==============================================================================
// Méthodes helper pour gérer les popups en overlay
void MainContentComponent::showPopup(DiatonyAlertWindow::AlertType type,
                                     const juce::String& title,
                                     const juce::String& message,
                                     const juce::String& buttonText)
{
    // Fermer l'ancien popup s'il existe
    closePopup();
    
    // Créer le nouveau popup avec callback de fermeture
    auto alertWindow = std::make_unique<DiatonyAlertWindow>(
        type, title, message, buttonText,
        [this]() { closePopup(); }  // Callback pour fermer le popup
    );
    
    // Créer l'overlay avec le popup
    activePopup = std::make_unique<DiatonyAlertWindowWithOverlay>(std::move(alertWindow));
    
    // Ajouter l'overlay par-dessus tout (z-order max)
    addAndMakeVisible(activePopup.get());
    activePopup->setBounds(getLocalBounds());
    activePopup->toFront(false);  // Mettre au premier plan sans voler le focus
}

void MainContentComponent::closePopup()
{
    if (activePopup != nullptr)
    {
        removeChildComponent(activePopup.get());
        activePopup.reset();
    }
}

//==============================================================================
// Drag & Drop Overlay
MainContentComponent::DragOverlay::DragOverlay()
{
    setOpaque(false);
}

void MainContentComponent::DragOverlay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Fond semi-transparent
    g.fillAll(juce::Colours::black.withAlpha(0.6f));
    
    // Rectangle central arrondi
    auto centerRect = bounds.reduced(50.0f);
    
    // Fond blanc du rectangle central
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(centerRect, 16.0f);
    
    // Bordure bleue
    g.setColour(juce::Colour(0xFF2196F3));  // Bleu Material
    g.drawRoundedRectangle(centerRect, 16.0f, 3.0f);
    
    // Bordure en pointillés (effet drop zone)
    g.setColour(juce::Colour(0xFF2196F3).withAlpha(0.5f));
    float dashLengths[] = { 10.0f, 5.0f };
    g.drawDashedLine(juce::Line<float>(centerRect.getTopLeft().translated(20, 20), 
                                        centerRect.getTopRight().translated(-20, 20)),
                     dashLengths, 2, 2.0f);
    
    // Texte centré
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.setFont(juce::Font(fontManager->getSFProDisplay(20.0f, FontManager::FontWeight::Semibold)));
    g.drawText(juce::String::fromUTF8("📂 Déposer votre fichier .diatony ici"),
               centerRect, juce::Justification::centred, true);
}

//==============================================================================
// FileDragAndDropTarget callbacks
bool MainContentComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    // Vérifie si au moins un fichier a l'extension .diatony
    for (const auto& file : files)
    {
        if (file.endsWithIgnoreCase(".diatony"))
            return true;
    }
    return false;
}

void MainContentComponent::fileDragEnter(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(files, x, y);
    
    DBG("📂 Drag Enter : fichier(s) .diatony détecté(s)");
    dragOverlay.setVisible(true);
    dragOverlay.toFront(false);
}

void MainContentComponent::fileDragExit(const juce::StringArray& files)
{
    juce::ignoreUnused(files);
    
    DBG("📂 Drag Exit");
    dragOverlay.setVisible(false);
}

void MainContentComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(x, y);
    
    // Cacher l'overlay immédiatement
    dragOverlay.setVisible(false);
    
    // === 1. VALIDATION DES FICHIERS ===
    if (files.isEmpty())
        return;
    
    if (files.size() > 1)
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Warning,
            "Attention",
            juce::String::fromUTF8("Veuillez déposer un seul fichier à la fois."),
            "OK"
        );
        return;
    }
    
    // === 2. TROUVER LE FICHIER .diatony ===
    juce::String diatonyFilePath;
    for (const auto& filePath : files)
    {
        if (filePath.endsWithIgnoreCase(".diatony"))
        {
            diatonyFilePath = filePath;
            break;
        }
    }
    
    if (diatonyFilePath.isEmpty())
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Warning,
            "Format invalide",
            juce::String::fromUTF8("Le fichier déposé n'est pas un fichier .diatony valide."),
            "OK"
        );
        return;
    }
    
    // === 3. OBTENIR L'APPCONTROLLER ===
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    if (pluginEditor == nullptr)
    {
        DBG("❌ Impossible de trouver AudioPluginAudioProcessorEditor");
        showPopup(
            DiatonyAlertWindow::AlertType::Error,
            "Erreur interne",
            juce::String::fromUTF8("Impossible d'accéder au contrôleur de l'application."),
            "OK"
        );
        return;
    }
    
    auto& appController = pluginEditor->getAppController();
    
    // === 4. CHARGER LE FICHIER ===
    juce::File file(diatonyFilePath);
    bool success = appController.loadProjectFromFile(file);
    
    // === 5. FEEDBACK UTILISATEUR ===
    if (success)
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Success,
            juce::String::fromUTF8("Projet Chargé"),
            juce::String::fromUTF8("Le fichier « ") + file.getFileNameWithoutExtension() + juce::String::fromUTF8(" » a été chargé avec succès."),
            "OK"
        );
    }
    else
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Error,
            "Erreur de chargement",
            juce::String::fromUTF8("Impossible de charger le fichier.\n\nVérifiez qu'il s'agit d'un fichier .diatony valide."),
            "OK"
        );
    }
}

//==============================================================================
// DragAndDropTarget interface (drag interne depuis HistoryPanel)
//==============================================================================

bool MainContentComponent::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
    // Accepter uniquement les drags depuis HistoryPanel
    // La description commence par "HISTORY_ITEM:" suivie du chemin du fichier
    juce::String description = dragSourceDetails.description.toString();
    return description.startsWith("HISTORY_ITEM:");
}

void MainContentComponent::itemDragEnter(const SourceDetails& dragSourceDetails)
{
    juce::ignoreUnused(dragSourceDetails);
    
    DBG("📂 Drag Enter (interne) : item d'historique");
    dragOverlay.setVisible(true);
    dragOverlay.toFront(false);
}

void MainContentComponent::itemDragExit(const SourceDetails& dragSourceDetails)
{
    juce::ignoreUnused(dragSourceDetails);
    
    DBG("📂 Drag Exit (interne)");
    dragOverlay.setVisible(false);
}

void MainContentComponent::itemDropped(const SourceDetails& dragSourceDetails)
{
    // Cacher l'overlay
    dragOverlay.setVisible(false);
    
    // === 1. EXTRAIRE LE CHEMIN DU FICHIER ===
    juce::String description = dragSourceDetails.description.toString();
    
    if (!description.startsWith("HISTORY_ITEM:"))
    {
        DBG("❌ Description invalide : " << description);
        return;
    }
    
    // Extraire le chemin après le préfixe
    juce::String diatonyFilePath = description.fromFirstOccurrenceOf("HISTORY_ITEM:", false, false);
    
    DBG("📂 Item dropped depuis historique : " << diatonyFilePath);
    
    // === 2. OBTENIR L'APPCONTROLLER ===
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    if (pluginEditor == nullptr)
    {
        DBG("❌ Impossible de trouver AudioPluginAudioProcessorEditor");
        showPopup(
            DiatonyAlertWindow::AlertType::Error,
            "Erreur interne",
            juce::String::fromUTF8("Impossible d'accéder au contrôleur de l'application."),
            "OK"
        );
        return;
    }
    
    auto& appController = pluginEditor->getAppController();
    
    // === 3. CHARGER LE FICHIER ===
    juce::File file(diatonyFilePath);
    bool success = appController.loadProjectFromFile(file);
    
    // === 4. FEEDBACK UTILISATEUR ===
    if (success)
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Success,
            juce::String::fromUTF8("Session Restaurée"),
            juce::String::fromUTF8("La session « ") + file.getFileNameWithoutExtension() + juce::String::fromUTF8(" » a été restaurée."),
            "OK"
        );
    }
    else
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Error,
            "Erreur de chargement",
            juce::String::fromUTF8("Impossible de charger la session.\n\nLe fichier est peut-être corrompu."),
            "OK"
        );
    }
} 