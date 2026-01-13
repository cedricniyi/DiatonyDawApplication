#include "MainContentComponent.h"
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "history/HistoryPanel.h"
#include "UIStateIdentifiers.h"
#include "extra/Component/DiatonyAlertWindow.h"
#include "PluginEditor.h"
#include "controller/AppController.h"

MainContentComponent::MainContentComponent() 
    : appState(),
      headerPanel(),
      sectionPanel(),
      historyPanel(),
      headerFlex(7.5f),
      sectionFlex(10.5f)
{        
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(sectionPanel);
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
    
    closePopup();
}

void MainContentComponent::setAppState(juce::ValueTree& state)
{
    if (appState.isValid())
        appState.removeListener(this);
        
    appState = state;
    appState.addListener(this);
    
    headerPanel.setAppState(appState);
    historyPanel.setAppState(appState);
}

void MainContentComponent::setSelectionState(juce::ValueTree& state)
{
    if (selectionState.isValid())
        selectionState.removeListener(this);
        
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
    
    constexpr int HISTORY_PANEL_MAX_WIDTH = 250;
    int historyWidth = static_cast<int>(HISTORY_PANEL_MAX_WIDTH * historyPanel.getWidthFraction());
    
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
    
    auto mainAreaBounds = bounds;
    
    int padding = 8;
    
    constexpr int HEADER_HEIGHT = 60;
    headerPanel.setBounds(bounds.removeFromTop(HEADER_HEIGHT));
    
    auto content = bounds.reduced(padding, 0);
    content.removeFromTop(4);

    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.items = { juce::FlexItem(sectionPanel).withFlex(1.0f) };
    fb.performLayout(content);
    
    if (activePopup != nullptr)
        activePopup->setBounds(getLocalBounds());
    
    // Positionner l'overlay de drag & drop UNIQUEMENT sur la zone principale (pas sur HistoryPanel)
    dragOverlay.setBounds(mainAreaBounds);
}

float& MainContentComponent::getHeaderFlexRef() { return headerFlex; }
float& MainContentComponent::getSectionFlexRef() { return sectionFlex; }

HeaderPanel& MainContentComponent::getHeaderPanel() { return headerPanel; }
SectionPanel& MainContentComponent::getSectionPanel() { return sectionPanel; }
HistoryPanel& MainContentComponent::getHistoryPanel() { return historyPanel; }

void MainContentComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                   const juce::Identifier& property)
{
    if (treeWhosePropertyHasChanged == selectionState && property == juce::Identifier("generationStatus"))
    {
        auto status = treeWhosePropertyHasChanged.getProperty("generationStatus").toString();
        
        if (status == "generating")
        {
            juce::MessageManager::callAsync([this]() {
                showPopup(
                    DiatonyAlertWindow::AlertType::Info,
                    juce::String::fromUTF8("Generating..."),
                    juce::String::fromUTF8("Diatony is searching for a musical solution...\n\nPlease wait, this may take a few seconds."),
                    ""  // Pas de bouton → non fermable par l'utilisateur
                );
            });
        }
        else if (status == "completed")
        {
            juce::MessageManager::callAsync([this]() {
                showPopup(
                    DiatonyAlertWindow::AlertType::Success,
                    juce::String::fromUTF8("Generation Complete"),
                    juce::String::fromUTF8("The MIDI file was generated successfully!\n\nA solution was found by the Diatony solver."),
                    "OK"
                );
            });
        }
        else if (status == "warning")
        {
            juce::String warningMessage = treeWhosePropertyHasChanged
                                            .getProperty("generationError")
                                            .toString();
            
            juce::MessageManager::callAsync([this, warningMessage]() {
                showPopup(
                    DiatonyAlertWindow::AlertType::Warning,
                    juce::String::fromUTF8("Warning"),
                    warningMessage,
                    "OK"
                );
            });
        }
        else if (status == "error")
        {
            juce::String errorMessage = treeWhosePropertyHasChanged
                                            .getProperty("generationError")
                                            .toString();
            
            juce::MessageManager::callAsync([this, errorMessage]() {
                showPopup(
                    DiatonyAlertWindow::AlertType::Error,
                    juce::String::fromUTF8("Generation Failed"),
                    juce::String::fromUTF8("The solver could not find a solution") + errorMessage,
                    "OK"
                );
            });
        }
    }
}

void MainContentComponent::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void MainContentComponent::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void MainContentComponent::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void MainContentComponent::valueTreeParentChanged(juce::ValueTree&) {}

void MainContentComponent::showPopup(DiatonyAlertWindow::AlertType type,
                                     const juce::String& title,
                                     const juce::String& message,
                                     const juce::String& buttonText)
{
    closePopup();
    
    auto alertWindow = std::make_unique<DiatonyAlertWindow>(
        type, title, message, buttonText,
        [this]() { closePopup(); }
    );
    
    // Créer l'overlay avec le popup
    activePopup = std::make_unique<DiatonyAlertWindowWithOverlay>(std::move(alertWindow));
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
    g.drawText(juce::String::fromUTF8("📂 Drop your .diatony file here"),
               centerRect, juce::Justification::centred, true);
}

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
    dragOverlay.setVisible(true);
    dragOverlay.toFront(false);
}

void MainContentComponent::fileDragExit(const juce::StringArray& files)
{
    juce::ignoreUnused(files);
    dragOverlay.setVisible(false);
}

void MainContentComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(x, y);
    dragOverlay.setVisible(false);
    
    // Valider le fichier
    if (files.isEmpty())
        return;
    
    if (files.size() > 1)
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Warning,
            "Warning",
            juce::String::fromUTF8("Please drop only one file at a time."),
            "OK"
        );
        return;
    }
    
    // Trouver le fichier .diatony  
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
            "Invalid Format",
            juce::String::fromUTF8("The dropped file is not a valid .diatony file."),
            "OK"
        );
        return;
    }
    
    //Récupéré l'appController
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    if (pluginEditor == nullptr)
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Error,
            "Internal Error",
            juce::String::fromUTF8("Unable to access the application controller."),
            "OK"
        );
        return;
    }
    
    auto& appController = pluginEditor->getAppController();
    
    // Charger le fichier
    juce::File file(diatonyFilePath);
    bool success = appController.loadProjectFromFile(file);
    
    // Feedback utilisateur
    if (success)
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Success,
            juce::String::fromUTF8("Project Loaded"),
            juce::String::fromUTF8("The file \"") + file.getFileNameWithoutExtension() + juce::String::fromUTF8("\" was loaded successfully."),
            "OK"
        );
    }
    else
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Error,
            "Loading Error",
            juce::String::fromUTF8("Unable to load the file.\n\nMake sure it is a valid .diatony file."),
            "OK"
        );
    }
}

bool MainContentComponent::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
    juce::String description = dragSourceDetails.description.toString();
    return description.startsWith("HISTORY_ITEM:");
}

void MainContentComponent::itemDragEnter(const SourceDetails& dragSourceDetails)
{
    juce::ignoreUnused(dragSourceDetails);
    dragOverlay.setVisible(true);
    dragOverlay.toFront(false);
}

void MainContentComponent::itemDragExit(const SourceDetails& dragSourceDetails)
{
    juce::ignoreUnused(dragSourceDetails);
    dragOverlay.setVisible(false);
}

void MainContentComponent::itemDropped(const SourceDetails& dragSourceDetails)
{
    dragOverlay.setVisible(false);
    
    // Extraire le chemin du fichier .diatony
    juce::String description = dragSourceDetails.description.toString();
    
    if (!description.startsWith("HISTORY_ITEM:"))
        return;
    
    juce::String diatonyFilePath = description.fromFirstOccurrenceOf("HISTORY_ITEM:", false, false);
    
    // Récupéré l'appController
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    if (pluginEditor == nullptr)
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Error,
            "Internal Error",
            juce::String::fromUTF8("Unable to access the application controller."),
            "OK"
        );
        return;
    }
    
    auto& appController = pluginEditor->getAppController();
    
    // Charger le fichier
    juce::File file(diatonyFilePath);
    bool success = appController.loadProjectFromFile(file);
    
    // Feedback utilisateur
    if (success)
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Success,
            juce::String::fromUTF8("Session Restored"),
            juce::String::fromUTF8("The session \"") + file.getFileNameWithoutExtension() + juce::String::fromUTF8("\" was restored successfully."),
            "OK"
        );
    }
    else
    {
        showPopup(
            DiatonyAlertWindow::AlertType::Error,
            "Loading Error",
            juce::String::fromUTF8("Unable to load the session.\n\nThe file may be corrupted."),
            "OK"
        );
    }
}
