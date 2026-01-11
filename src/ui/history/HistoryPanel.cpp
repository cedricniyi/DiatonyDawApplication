#include "HistoryPanel.h"
#include "ui/UIStateIdentifiers.h"
#include "utils/FontManager.h"
#include "utils/FileUtils.h"
#include "model/ModelIdentifiers.h"

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
    
    if (selected)
        g.fillAll(juce::Colour(0xFF3A5A6A));  // Bleu-gris sélectionné
    else if (rowNumber % 2 == 0)
        g.fillAll(juce::Colour(0xFF1A1A1A));  // Fond pair
    else
        g.fillAll(juce::Colour(0xFF222222));  // Fond impair
    
    auto bounds = juce::Rectangle<int>(0, 0, width, height).reduced(10, 6);
    auto topRow = bounds.removeFromTop(20);
    
    juce::String timestampStr = HistoryPanel::formatTimestamp(currentItem.timestamp);
    
    auto dateFont = juce::Font(fontManager->getSFProDisplay(11.0f, FontManager::FontWeight::Regular));
    g.setFont(dateFont);
    g.setColour(selected ? juce::Colours::white.withAlpha(0.7f) : juce::Colour(0xFF888888));
    
    int dateWidth = static_cast<int>(dateFont.getStringWidthFloat(timestampStr)) + 4;
    auto dateArea = topRow.removeFromRight(dateWidth);
    g.drawText(timestampStr, dateArea, juce::Justification::centredRight, false);
    
    topRow.removeFromRight(8);
    auto nameFont = juce::Font(fontManager->getSFProDisplay(14.0f, FontManager::FontWeight::Semibold));
    g.setFont(nameFont);
    g.setColour(selected ? juce::Colours::white : juce::Colour(0xFFE0E0E0));
    g.drawText(currentItem.name, topRow, juce::Justification::centredLeft, true);
    
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
    
    g.setColour(juce::Colour(0xFF333333));
    g.fillRect(0, height - 1, width, 1);
}

void HistoryRowComponent::mouseDrag(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    
    if (!currentItem.diatonyFile.existsAsFile())
        return;
    
    auto* dragContainer = juce::DragAndDropContainer::findParentDragContainerFor(this);
    if (dragContainer == nullptr)
        return;
    
    juce::String dragDescription = "HISTORY_ITEM:" + currentItem.diatonyFile.getFullPathName();
    
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
    
    dragContainer->startDragging(dragDescription, this, juce::ScaledImage(dragImage), true);
}

HistoryPanel::ContentContainer::ContentContainer()
{
    setOpaque(false);
}

void HistoryPanel::ContentContainer::resized()
{
    auto bounds = getLocalBounds();
    
    if (openFolderButton)
    {
        auto buttonArea = bounds.removeFromTop(BUTTON_ZONE_HEIGHT);
        openFolderButton->setBounds(buttonArea.reduced(12, 8));
    }
    
    historyList.setBounds(bounds);
}

HistoryPanel::HistoryPanel() 
    : isPanelVisible(false),
      widthTransitionFraction(0.0f)
{
    headerLabel.setText("History", juce::dontSendNotification);
    headerLabel.setJustificationType(juce::Justification::centred);
    headerLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    headerLabel.setFont(juce::Font(fontManager->getSFProDisplay(18.0f, FontManager::FontWeight::Semibold)));
    addAndMakeVisible(headerLabel);
    
    contentContainer.openFolderButton = std::make_unique<StyledButton>(
        juce::String::fromUTF8("Open Folder"),
        juce::Colour::fromString("ff808080"),
        juce::Colour::fromString("ff606060"),
        13.0f, FontManager::FontWeight::Medium
    );
    contentContainer.openFolderButton->setTooltip("Ouvrir le dossier des solutions MIDI");
    contentContainer.openFolderButton->onClick = []() { FileUtils::openMidiSolutionsFolder(); };
    contentContainer.addAndMakeVisible(*contentContainer.openFolderButton);
    
    contentContainer.historyList.setModel(this);
    contentContainer.historyList.setRowHeight(ROW_HEIGHT);
    contentContainer.historyList.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xFF1A1A1A));
    contentContainer.historyList.setColour(juce::ListBox::outlineColourId, juce::Colours::transparentBlack);
    contentContainer.historyList.setMultipleSelectionEnabled(false);
    contentContainer.addAndMakeVisible(contentContainer.historyList);
    
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
    
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, HEADER_HEIGHT - 1, getWidth(), 1);
    g.fillRect(0, 0, 1, getHeight());
}

void HistoryPanel::resized()
{
    auto bounds = getLocalBounds();
    headerLabel.setBounds(bounds.removeFromTop(HEADER_HEIGHT));
    bounds.removeFromLeft(1);
    contentContainer.setBounds(bounds);
}

void HistoryPanel::setExpanded(bool expanded) { isPanelVisible = expanded; }
bool HistoryPanel::getExpanded() const { return isPanelVisible; }
juce::Component& HistoryPanel::getFadingComponent() { return contentContainer; }
float& HistoryPanel::getWidthFractionRef() { return widthTransitionFraction; }
float HistoryPanel::getWidthFraction() const { return widthTransitionFraction; }

void HistoryPanel::refreshFromDisk()
{
    items.clear();
    
    juce::File solutionsDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
        .getChildFile(APPLICATION_SUPPORT_PATH)
        .getChildFile("DiatonyDawApplication")
        .getChildFile("Solutions")
        .getChildFile("MidiFiles");
    
    if (!solutionsDir.exists())
    {
        contentContainer.historyList.updateContent();
        return;
    }
    
    juce::Array<juce::File> diatonyFiles;
    solutionsDir.findChildFiles(diatonyFiles, juce::File::findFiles, false, "*.diatony");
    
    for (const auto& file : diatonyFiles)
    {
        HistoryItem item;
        if (parseHistoryFile(file, item))
            items.push_back(item);
    }
    
    std::sort(items.begin(), items.end(), 
        [](const HistoryItem& a, const HistoryItem& b) { return a.timestamp > b.timestamp; });
    
    contentContainer.historyList.updateContent();
}

bool HistoryPanel::parseHistoryFile(const juce::File& file, HistoryItem& outItem)
{
    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr || !xml->hasTagName("Piece"))
        return false;
    
    outItem.diatonyFile = file;
    outItem.midiFile = file.withFileExtension("mid");
    outItem.name = file.getFileNameWithoutExtension();
    outItem.timestamp = file.getLastModificationTime();
    outItem.numSections = 0;
    outItem.numModulations = 0;
    outItem.numChords = 0;
    outItem.startKey = "?";
    
    for (auto* child : xml->getChildIterator())
    {
        if (child->hasTagName("Section"))
        {
            outItem.numSections++;
            
            if (outItem.numSections == 1)
            {
                int noteIndex = child->getIntAttribute("tonalityNote", 0);
                bool isMajor = child->getBoolAttribute("isMajor", true);
                outItem.startKey = noteToKeyLabel(noteIndex, isMajor);
            }
            
            if (auto* progression = child->getChildByName("Progression"))
                outItem.numChords += progression->getNumChildElements();
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
    static const char* noteNames[] = { "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B" };
    
    if (noteIndex < 0 || noteIndex > 11)
        return "?";
    
    juce::String label = noteNames[noteIndex];
    label += isMajor ? "" : "m";
    return label;
}

juce::String HistoryPanel::formatTimestamp(const juce::Time& time)
{
    auto now = juce::Time::getCurrentTime();
    auto today = juce::Time(now.getYear(), now.getMonth(), now.getDayOfMonth(), 0, 0);
    auto yesterday = today - juce::RelativeTime::days(1);
    
    if (time >= today)
        return time.formatted("%H:%M");
    else if (time >= yesterday)
        return "Hier";
    else
        return time.formatted("%d/%m");
}

int HistoryPanel::getNumRows()
{
    return static_cast<int>(items.size());
}

void HistoryPanel::paintListBoxItem(int, juce::Graphics&, int, int, bool)
{
    // Rendu géré par refreshComponentForRow()
}

juce::Component* HistoryPanel::refreshComponentForRow(int rowNumber, bool isRowSelected,
                                                       juce::Component* existingComponentToUpdate)
{
    if (rowNumber < 0 || rowNumber >= static_cast<int>(items.size()))
    {
        if (existingComponentToUpdate != nullptr)
            delete existingComponentToUpdate;
        return nullptr;
    }
    
    auto* rowComponent = dynamic_cast<HistoryRowComponent*>(existingComponentToUpdate);
    if (rowComponent == nullptr)
        rowComponent = new HistoryRowComponent(*this);
    
    rowComponent->setRowData(items[static_cast<size_t>(rowNumber)], rowNumber, isRowSelected);
    return rowComponent;
}

void HistoryPanel::listBoxItemClicked(int, const juce::MouseEvent&) {}
void HistoryPanel::listBoxItemDoubleClicked(int, const juce::MouseEvent&) {}

void HistoryPanel::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if (property == UIStateIdentifiers::historyPanelVisible)
        updateVisibilityState();
    else if (property.toString() == "generationStatus")
    {
        juce::String status = appState.getProperty("generationStatus", "").toString();
        if (status == "completed")
            refreshFromDisk();
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
    
    if (visible)
        refreshFromDisk();
    
    if (onVisibilityChange)
        onVisibilityChange(visible);
    else
    {
        setExpanded(visible);
        widthTransitionFraction = visible ? 1.0f : 0.0f;
    }
}
