#include "HistoryPanel.h"
#include "ui/UIStateIdentifiers.h"
#include "utils/FontManager.h"
#include "utils/FileUtils.h"

//==============================================================================
// ContentContainer implementation
HistoryPanel::ContentContainer::ContentContainer()
{
    setOpaque(false);
}

void HistoryPanel::ContentContainer::resized()
{
    auto bounds = getLocalBounds();
    
    // Zone du bouton "Open Folder" - première row
    if (openFolderButton)
    {
        auto buttonArea = bounds.removeFromTop(BUTTON_ZONE_HEIGHT);
        openFolderButton->setBounds(buttonArea.reduced(12, 8));
    }
    
    // ListBox prend le reste de l'espace
    historyList.setBounds(bounds);
}

//==============================================================================
// HistoryPanel implementation
HistoryPanel::HistoryPanel() 
    : isPanelVisible(false),
      widthTransitionFraction(0.0f)
{
    // Configuration du header - titre centré
    headerLabel.setText("History", juce::dontSendNotification);
    headerLabel.setJustificationType(juce::Justification::centred);
    headerLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    
    // Même style de police que le header principal
    auto fontOptions = fontManager->getSFProDisplay(18.0f, FontManager::FontWeight::Semibold);
    headerLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(headerLabel);
    
    // Configuration du bouton "Open Folder" dans le conteneur
    contentContainer.openFolderButton = std::make_unique<StyledButton>(
        juce::String::fromUTF8("Open Folder"),
        juce::Colour::fromString("ff808080"),  // Gris
        juce::Colour::fromString("ff606060"),  // Gris plus foncé au survol
        13.0f, FontManager::FontWeight::Medium
    );
    contentContainer.openFolderButton->setTooltip("Ouvrir le dossier des solutions MIDI");
    contentContainer.openFolderButton->onClick = []() {
        FileUtils::openMidiSolutionsFolder();
    };
    contentContainer.addAndMakeVisible(*contentContainer.openFolderButton);
    
    // Configuration de la ListBox dans le conteneur
    contentContainer.historyList.setModel(this);
    contentContainer.historyList.setRowHeight(ROW_HEIGHT);
    contentContainer.historyList.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xFF1A1A1A));
    contentContainer.historyList.setColour(juce::ListBox::outlineColourId, juce::Colours::transparentBlack);
    contentContainer.historyList.setMultipleSelectionEnabled(false);
    contentContainer.addAndMakeVisible(contentContainer.historyList);
    
    // Le conteneur fade in/out
    contentContainer.setAlpha(0.0f);
    addAndMakeVisible(contentContainer);
    
    // Générer les données de test
    generateMockData();
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
    
    // Synchronise l'état initial
    updateVisibilityState();
}

void HistoryPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Fond principal du panneau
    g.fillAll(juce::Colour(0xFF1E1E1E));
    
    // Header zone - fond identique au HeaderPanel principal
    auto headerZone = bounds.removeFromTop(HEADER_HEIGHT);
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRect(headerZone);
    
    // Bordure en bas du header (même style que HeaderPanel)
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, HEADER_HEIGHT - 1, getWidth(), 1);
    
    // Bordure à gauche du panneau (séparation avec le contenu principal)
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, 0, 1, getHeight());
}

void HistoryPanel::resized()
{
    auto bounds = getLocalBounds();
    
    // Header - titre centré sur toute la largeur
    auto headerArea = bounds.removeFromTop(HEADER_HEIGHT);
    headerLabel.setBounds(headerArea);
    
    // Bordure à gauche
    bounds.removeFromLeft(1);
    
    // Le conteneur (bouton + liste) prend le reste
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
// ListBoxModel implementation

int HistoryPanel::getNumRows()
{
    return static_cast<int>(items.size());
}

void HistoryPanel::paintListBoxItem(int rowNumber, juce::Graphics& g, 
                                    int width, int height, bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= static_cast<int>(items.size()))
        return;
    
    const auto& item = items[static_cast<size_t>(rowNumber)];
    
    // === FOND ===
    if (rowIsSelected)
        g.fillAll(juce::Colour(0xFF3A5A6A));  // Bleu-gris sélectionné
    else if (rowNumber % 2 == 0)
        g.fillAll(juce::Colour(0xFF1A1A1A));  // Fond pair
    else
        g.fillAll(juce::Colour(0xFF222222));  // Fond impair (légèrement différent)
    
    // Marges internes
    auto bounds = juce::Rectangle<int>(0, 0, width, height).reduced(10, 6);
    
    // === LIGNE 1 : NOM + DATE ===
    auto topRow = bounds.removeFromTop(20);
    
    // Date à droite (en premier pour réserver l'espace)
    auto dateFont = juce::Font(fontManager->getSFProDisplay(11.0f, FontManager::FontWeight::Regular));
    g.setFont(dateFont);
    g.setColour(rowIsSelected ? juce::Colours::white.withAlpha(0.7f) : juce::Colour(0xFF888888));
    
    // Calculer la largeur de la date
    int dateWidth = dateFont.getStringWidth(item.timestamp) + 4;
    auto dateArea = topRow.removeFromRight(dateWidth);
    g.drawText(item.timestamp, dateArea, juce::Justification::centredRight, false);
    
    // Nom à gauche (prend le reste, avec ellipsis si trop long)
    topRow.removeFromRight(8);  // Petit espace avant la date
    auto nameFont = juce::Font(fontManager->getSFProDisplay(14.0f, FontManager::FontWeight::Semibold));
    g.setFont(nameFont);
    g.setColour(rowIsSelected ? juce::Colours::white : juce::Colour(0xFFE0E0E0));
    g.drawText(item.name, topRow, juce::Justification::centredLeft, true);  // true = ellipsis
    
    // === LIGNE 2 : DASHBOARD STATS ===
    bounds.removeFromTop(4);  // Petit espace entre les lignes
    auto botRow = bounds;
    
    auto statsFont = juce::Font(fontManager->getSFProDisplay(12.0f, FontManager::FontWeight::Regular));
    g.setFont(statsFont);
    g.setColour(rowIsSelected ? juce::Colours::white.withAlpha(0.85f) : juce::Colour(0xFFAAAAAA));
    
    // Format compact : Key | S:n | M:n | A:n
    juce::String stats;
    stats << item.startKey;
    stats << "  |  S:" << item.numSections;
    stats << "  |  M:" << item.numModulations;
    stats << "  |  A:" << item.numChords;
    
    g.drawText(stats, botRow, juce::Justification::centredLeft, false);
    
    // === BORDURE INFÉRIEURE COMPLÈTE ===
    g.setColour(juce::Colour(0xFF333333));
    g.fillRect(0, height - 1, width, 1);
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
        // TODO: Charger cette solution
    }
}

//==============================================================================
// ValueTree::Listener implementation

void HistoryPanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                           const juce::Identifier& property)
{
    if (property == UIStateIdentifiers::historyPanelVisible)
    {
        updateVisibilityState();
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
    
    // Notifie l'Animator pour déclencher l'animation de visibilité
    if (onVisibilityChange)
    {
        onVisibilityChange(visible);
    }
    else
    {
        // Fallback : mise à jour directe de l'état si aucun animateur n'est attaché
        setExpanded(visible);
        widthTransitionFraction = visible ? 1.0f : 0.0f;
    }
}

//==============================================================================
// Mock data generation

void HistoryPanel::generateMockData()
{
    items.clear();
    
    // === AUJOURD'HUI ===
    items.push_back({ "Idee Refrain V2", "16:45", "Cm", 4, 3, 32 });
    items.push_back({ "Bridge Alternative", "14:30", "Am", 2, 1, 12 });
    items.push_back({ "Intro Piano Solo", "11:20", "Dm", 1, 0, 8 });
    
    // === HIER ===
    items.push_back({ "Symphonie Inachevee", "Hier", "Eb", 12, 8, 145 });
    items.push_back({ "Variation Theme Principal", "Hier", "Bb", 3, 2, 24 });
    items.push_back({ "Test Harmonique Rapide", "Hier", "C", 1, 0, 4 });
    
    // === CETTE SEMAINE ===
    items.push_back({ "Progression Jazz chromatique", "07/12", "F#m", 2, 5, 16 });
    items.push_back({ "Ballade Triste", "06/12", "Gm", 3, 1, 24 });
    items.push_back({ "Outro Cinematique", "05/12", "Em", 2, 2, 20 });
    items.push_back({ "Hook Electro", "04/12", "Am", 1, 0, 8 });
    
    // === SEMAINE DERNIERE ===
    items.push_back({ "Couplet Final V3", "01/12", "Fm", 2, 1, 16 });
    items.push_back({ "Intro Epique", "28/11", "Dm", 2, 2, 18 });
    items.push_back({ "Transition Orchestrale", "27/11", "Ab", 1, 3, 12 });
    items.push_back({ "Pre-Chorus Energique", "26/11", "E", 2, 1, 14 });
    items.push_back({ "Coda Finale", "25/11", "G", 1, 1, 8 });
    
    // === PLUS ANCIEN ===
    items.push_back({ "Esquisse Nocturne", "20/11", "Bbm", 3, 2, 28 });
    items.push_back({ "Meditation Modale", "18/11", "D", 4, 4, 36 });
    items.push_back({ "Riff Principal V1", "15/11", "A", 1, 0, 6 });
    items.push_back({ "Harmonie Complexe Test", "12/11", "C#m", 5, 6, 48 });
    items.push_back({ "Prototype Ambient", "10/11", "F", 2, 1, 16 });
    items.push_back({ "Structure Verse-Chorus", "08/11", "G", 4, 2, 32 });
    items.push_back({ "Modulation Enharmonique", "05/11", "Gb", 2, 4, 18 });
    items.push_back({ "Premier Brouillon", "01/11", "Am", 1, 0, 4 });
    
    // Mettre à jour la ListBox
    contentContainer.historyList.updateContent();
}
