#include "HistoryPanel.h"
#include "ui/UIStateIdentifiers.h"
#include "utils/FontManager.h"
#include "utils/FileUtils.h"
#include "model/ModelIdentifiers.h"

//==============================================================================
// HistoryRowComponent implementation
//==============================================================================

HistoryRowComponent::HistoryRowComponent(HistoryPanel& parentPanel)
    : owner(parentPanel)
{
}

void HistoryRowComponent::setRowData(const HistoryItem& item, int rowNum, bool isSelected)
{
    currentItem = item;
    rowNumber = rowNum;
    selected = isSelected;
    repaint();
}

void HistoryRowComponent::paint(juce::Graphics& g)
{
    auto width = getWidth();
    auto height = getHeight();
    
    // === FOND ===
    if (selected)
        g.fillAll(juce::Colour(0xFF3A5A6A));  // Bleu-gris sélectionné
    else if (rowNumber % 2 == 0)
        g.fillAll(juce::Colour(0xFF1A1A1A));  // Fond pair
    else
        g.fillAll(juce::Colour(0xFF222222));  // Fond impair
    
    // Marges internes
    auto bounds = juce::Rectangle<int>(0, 0, width, height).reduced(10, 6);
    
    // === LIGNE 1 : NOM + DATE ===
    auto topRow = bounds.removeFromTop(20);
    
    // Formater le timestamp pour l'affichage
    juce::String timestampStr = HistoryPanel::formatTimestamp(currentItem.timestamp);
    
    // Date à droite
    auto dateFont = juce::Font(fontManager->getSFProDisplay(11.0f, FontManager::FontWeight::Regular));
    g.setFont(dateFont);
    g.setColour(selected ? juce::Colours::white.withAlpha(0.7f) : juce::Colour(0xFF888888));
    
    int dateWidth = static_cast<int>(dateFont.getStringWidthFloat(timestampStr)) + 4;
    auto dateArea = topRow.removeFromRight(dateWidth);
    g.drawText(timestampStr, dateArea, juce::Justification::centredRight, false);
    
    // Nom à gauche
    topRow.removeFromRight(8);
    auto nameFont = juce::Font(fontManager->getSFProDisplay(14.0f, FontManager::FontWeight::Semibold));
    g.setFont(nameFont);
    g.setColour(selected ? juce::Colours::white : juce::Colour(0xFFE0E0E0));
    g.drawText(currentItem.name, topRow, juce::Justification::centredLeft, true);
    
    // === LIGNE 2 : STATS ===
    bounds.removeFromTop(4);
    auto botRow = bounds;
    
    auto statsFont = juce::Font(fontManager->getSFProDisplay(12.0f, FontManager::FontWeight::Regular));
    g.setFont(statsFont);
    g.setColour(selected ? juce::Colours::white.withAlpha(0.85f) : juce::Colour(0xFFAAAAAA));
    
    juce::String stats;
    stats << currentItem.startKey;
    stats << "  |  S:" << currentItem.numSections;
    stats << "  |  M:" << currentItem.numModulations;
    stats << "  |  A:" << currentItem.numChords;
    
    g.drawText(stats, botRow, juce::Justification::centredLeft, false);
    
    // === BORDURE INFÉRIEURE ===
    g.setColour(juce::Colour(0xFF333333));
    g.fillRect(0, height - 1, width, 1);
}

void HistoryRowComponent::mouseDrag(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    
    // Vérifier que le fichier .diatony existe
    if (!currentItem.diatonyFile.existsAsFile())
    {
        DBG("⚠️ Fichier .diatony introuvable : " << currentItem.diatonyFile.getFullPathName());
        return;
    }
    
    // Trouver le DragAndDropContainer parent (HistoryPanel)
    auto* dragContainer = juce::DragAndDropContainer::findParentDragContainerFor(this);
    
    if (dragContainer == nullptr)
    {
        DBG("❌ Pas de DragAndDropContainer parent !");
        return;
    }
    
    // Créer la description du drag : préfixe + chemin du fichier .diatony
    juce::String dragDescription = "HISTORY_ITEM:" + currentItem.diatonyFile.getFullPathName();
    
    DBG("📋 Drag interne : " << currentItem.name);
    
    // Créer une image de prévisualisation pour le drag
    juce::Image dragImage(juce::Image::ARGB, 180, 40, true);
    {
        juce::Graphics g(dragImage);
        g.fillAll(juce::Colour(0xFF3A5A6A).withAlpha(0.9f));
        g.setColour(juce::Colours::white);
        auto font = juce::Font(fontManager->getSFProDisplay(13.0f, FontManager::FontWeight::Semibold));
        g.setFont(font);
        g.drawText(currentItem.name, dragImage.getBounds().reduced(8, 0), 
                   juce::Justification::centredLeft, true);
    }
    
    // Lancer le drag interne JUCE
    dragContainer->startDragging(
        dragDescription,
        this,
        juce::ScaledImage(dragImage),
        true  // Permettre le drag vers d'autres fenêtres JUCE
    );
}

//==============================================================================
// ContentContainer implementation
//==============================================================================

HistoryPanel::ContentContainer::ContentContainer()
{
    setOpaque(false);
}

void HistoryPanel::ContentContainer::resized()
{
    auto bounds = getLocalBounds();
    
    // Zone du bouton "Open Folder"
    if (openFolderButton)
    {
        auto buttonArea = bounds.removeFromTop(BUTTON_ZONE_HEIGHT);
        openFolderButton->setBounds(buttonArea.reduced(12, 8));
    }
    
    // ListBox prend le reste
    historyList.setBounds(bounds);
}

//==============================================================================
// HistoryPanel implementation
//==============================================================================

HistoryPanel::HistoryPanel() 
    : isPanelVisible(false),
      widthTransitionFraction(0.0f)
{
    // Configuration du header
    headerLabel.setText("History", juce::dontSendNotification);
    headerLabel.setJustificationType(juce::Justification::centred);
    headerLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    
    auto fontOptions = fontManager->getSFProDisplay(18.0f, FontManager::FontWeight::Semibold);
    headerLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(headerLabel);
    
    // Configuration du bouton "Open Folder"
    contentContainer.openFolderButton = std::make_unique<StyledButton>(
        juce::String::fromUTF8("Open Folder"),
        juce::Colour::fromString("ff808080"),
        juce::Colour::fromString("ff606060"),
        13.0f, FontManager::FontWeight::Medium
    );
    contentContainer.openFolderButton->setTooltip("Ouvrir le dossier des solutions MIDI");
    contentContainer.openFolderButton->onClick = []() {
        FileUtils::openMidiSolutionsFolder();
    };
    contentContainer.addAndMakeVisible(*contentContainer.openFolderButton);
    
    // Configuration de la ListBox
    contentContainer.historyList.setModel(this);
    contentContainer.historyList.setRowHeight(ROW_HEIGHT);
    contentContainer.historyList.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xFF1A1A1A));
    contentContainer.historyList.setColour(juce::ListBox::outlineColourId, juce::Colours::transparentBlack);
    contentContainer.historyList.setMultipleSelectionEnabled(false);
    contentContainer.addAndMakeVisible(contentContainer.historyList);
    
    // Le conteneur fade in/out
    contentContainer.setAlpha(0.0f);
    addAndMakeVisible(contentContainer);
}

HistoryPanel::~HistoryPanel()
{
    if (appState.isValid())
        appState.removeListener(this);
}

void HistoryPanel::setAppState(juce::ValueTree& state)
{
    if (appState.isValid())
        appState.removeListener(this);
        
    appState = state;
    appState.addListener(this);
    
    updateVisibilityState();
}

void HistoryPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.fillAll(juce::Colour(0xFF1E1E1E));
    
    auto headerZone = bounds.removeFromTop(HEADER_HEIGHT);
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRect(headerZone);
    
    // Bordure en bas du header
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, HEADER_HEIGHT - 1, getWidth(), 1);
    
    // Bordure à gauche
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, 0, 1, getHeight());
}

void HistoryPanel::resized()
{
    auto bounds = getLocalBounds();
    
    auto headerArea = bounds.removeFromTop(HEADER_HEIGHT);
    headerLabel.setBounds(headerArea);
    
    bounds.removeFromLeft(1);
    contentContainer.setBounds(bounds);
}

void HistoryPanel::setExpanded(bool expanded)
{
    isPanelVisible = expanded;
}

bool HistoryPanel::getExpanded() const
{
    return isPanelVisible;
}

juce::Component& HistoryPanel::getFadingComponent()
{
    return contentContainer;
}

float& HistoryPanel::getWidthFractionRef()
{
    return widthTransitionFraction;
}

float HistoryPanel::getWidthFraction() const
{
    return widthTransitionFraction;
}

//==============================================================================
// Chargement dynamique depuis le disque
//==============================================================================

void HistoryPanel::refreshFromDisk()
{
    DBG("📂 HistoryPanel::refreshFromDisk() - Scan du dossier...");
    
    items.clear();
    
    // Localiser le dossier des solutions
    juce::File solutionsDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
        .getChildFile(APPLICATION_SUPPORT_PATH)
        .getChildFile("DiatonyDawApplication")
        .getChildFile("Solutions")
        .getChildFile("MidiFiles");
    
    if (!solutionsDir.exists())
    {
        DBG("  ⚠️ Dossier inexistant : " << solutionsDir.getFullPathName());
        contentContainer.historyList.updateContent();
        return;
    }
    
    // Scanner les fichiers .diatony
    juce::Array<juce::File> diatonyFiles;
    solutionsDir.findChildFiles(diatonyFiles, juce::File::findFiles, false, "*.diatony");
    
    DBG("  📄 " << diatonyFiles.size() << " fichier(s) .diatony trouvé(s)");
    
    // Parser chaque fichier
    for (const auto& file : diatonyFiles)
    {
        HistoryItem item;
        if (parseHistoryFile(file, item))
        {
            items.push_back(item);
        }
    }
    
    // Trier par date de modification (plus récent en premier)
    std::sort(items.begin(), items.end(), 
        [](const HistoryItem& a, const HistoryItem& b) {
            return a.timestamp > b.timestamp;
        });
    
    DBG("  ✅ " << items.size() << " item(s) chargé(s)");
    
    // Rafraîchir la ListBox
    contentContainer.historyList.updateContent();
}

bool HistoryPanel::parseHistoryFile(const juce::File& file, HistoryItem& outItem)
{
    // Parser le XML
    auto xml = juce::XmlDocument::parse(file);
    
    if (xml == nullptr || !xml->hasTagName("Piece"))
    {
        DBG("    ⚠️ XML invalide : " << file.getFileName());
        return false;
    }
    
    // Remplir les infos de base
    outItem.diatonyFile = file;
    outItem.midiFile = file.withFileExtension("mid");
    outItem.name = file.getFileNameWithoutExtension();
    outItem.timestamp = file.getLastModificationTime();
    
    // Compter les éléments
    outItem.numSections = 0;
    outItem.numModulations = 0;
    outItem.numChords = 0;
    outItem.startKey = "?";
    
    // Parcourir les enfants du XML
    for (auto* child : xml->getChildIterator())
    {
        if (child->hasTagName("Section"))
        {
            outItem.numSections++;
            
            // Extraire la tonalité de la première section
            if (outItem.numSections == 1)
            {
                int noteIndex = child->getIntAttribute("tonalityNote", 0);
                bool isMajor = child->getBoolAttribute("isMajor", true);
                outItem.startKey = noteToKeyLabel(noteIndex, isMajor);
            }
            
            // Compter les accords dans la Progression
            if (auto* progression = child->getChildByName("Progression"))
            {
                outItem.numChords += progression->getNumChildElements();
            }
        }
        else if (child->hasTagName("Modulation"))
        {
            outItem.numModulations++;
        }
    }
    
    return true;
}

juce::String HistoryPanel::noteToKeyLabel(int noteIndex, bool isMajor)
{
    // Table des noms de notes (avec préférence pour bémols sur certaines)
    static const char* noteNames[] = {
        "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"
    };
    
    if (noteIndex < 0 || noteIndex > 11)
        return "?";
    
    juce::String label = noteNames[noteIndex];
    label += isMajor ? "" : "m";  // Ajouter "m" pour mineur
    
    return label;
}

juce::String HistoryPanel::formatTimestamp(const juce::Time& time)
{
    auto now = juce::Time::getCurrentTime();
    auto today = juce::Time(now.getYear(), now.getMonth(), now.getDayOfMonth(), 0, 0);
    auto yesterday = today - juce::RelativeTime::days(1);
    
    if (time >= today)
    {
        // Aujourd'hui : afficher l'heure
        return time.formatted("%H:%M");
    }
    else if (time >= yesterday)
    {
        // Hier
        return "Hier";
    }
    else
    {
        // Plus ancien : afficher la date
        return time.formatted("%d/%m");
    }
}

//==============================================================================
// ListBoxModel implementation
//==============================================================================

int HistoryPanel::getNumRows()
{
    return static_cast<int>(items.size());
}

void HistoryPanel::paintListBoxItem(int /*rowNumber*/, juce::Graphics& /*g*/, 
                                    int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{
    // On utilise refreshComponentForRow() à la place, donc cette méthode est vide
}

juce::Component* HistoryPanel::refreshComponentForRow(int rowNumber, bool isRowSelected,
                                                       juce::Component* existingComponentToUpdate)
{
    if (rowNumber < 0 || rowNumber >= static_cast<int>(items.size()))
    {
        // Supprimer le composant s'il existe mais qu'il n'y a plus de données
        if (existingComponentToUpdate != nullptr)
            delete existingComponentToUpdate;
        return nullptr;
    }
    
    // Réutiliser ou créer un composant
    auto* rowComponent = dynamic_cast<HistoryRowComponent*>(existingComponentToUpdate);
    
    if (rowComponent == nullptr)
    {
        rowComponent = new HistoryRowComponent(*this);
    }
    
    // Mettre à jour les données
    rowComponent->setRowData(items[static_cast<size_t>(rowNumber)], rowNumber, isRowSelected);
    
    return rowComponent;
}

void HistoryPanel::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < static_cast<int>(items.size()))
    {
        DBG("📋 Item cliqué : " << items[static_cast<size_t>(row)].name);
    }
}

void HistoryPanel::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < static_cast<int>(items.size()))
    {
        DBG("📋 Double-clic sur : " << items[static_cast<size_t>(row)].name);
        // TODO: Charger cette solution dans l'éditeur
    }
}

//==============================================================================
// ValueTree::Listener implementation
//==============================================================================

void HistoryPanel::valueTreePropertyChanged(juce::ValueTree& /*treeWhosePropertyHasChanged*/,
                                           const juce::Identifier& property)
{
    if (property == UIStateIdentifiers::historyPanelVisible)
    {
        updateVisibilityState();
    }
    else if (property.toString() == "generationStatus")
    {
        // Rafraîchir après une génération
        juce::String status = appState.getProperty("generationStatus", "").toString();
        if (status == "completed")
        {
            DBG("📂 Génération terminée, rafraîchissement de l'historique...");
            refreshFromDisk();
        }
    }
}

void HistoryPanel::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void HistoryPanel::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void HistoryPanel::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void HistoryPanel::valueTreeParentChanged(juce::ValueTree&) {}

void HistoryPanel::updateVisibilityState()
{
    if (!appState.isValid()) return;
    
    bool visible = static_cast<bool>(appState.getProperty(UIStateIdentifiers::historyPanelVisible, false));
    
    // Rafraîchir quand le panneau devient visible
    if (visible)
    {
        refreshFromDisk();
    }
    
    if (onVisibilityChange)
    {
        onVisibilityChange(visible);
    }
    else
    {
        setExpanded(visible);
        widthTransitionFraction = visible ? 1.0f : 0.0f;
    }
}
