#include "SectionEditor.h"
#include "ui/PluginEditor.h"

SectionEditor::SectionEditor()
{
    setOpaque(false);
    
    setupSectionNameLabel();
    
    addAndMakeVisible(keyZone);
    addAndMakeVisible(modeZone);
    addAndMakeVisible(zone4Component);
    
    bindZonesToModel();
    updateContent();
}

SectionEditor::~SectionEditor()
{
    if (currentSectionState.isValid())
        currentSectionState.removeListener(this);
    
    if (currentProgressionState.isValid())
        currentProgressionState.removeListener(this);
}

void SectionEditor::paint(juce::Graphics& g)
{
    // Dessiner l'encoche centrée en haut
    drawNotch(g);
}

void SectionEditor::resized()
{
    auto bounds = getLocalBounds();
    
    constexpr int NOTCH_HEIGHT = 24;
    constexpr int NOTCH_WIDTH = 100;
    
    int notchX = (bounds.getWidth() - NOTCH_WIDTH) / 2;
    notchArea = juce::Rectangle<int>(notchX, 0, NOTCH_WIDTH, NOTCH_HEIGHT);
    sectionNameLabel.setBounds(notchArea);
    
    contentArea = bounds.withTrimmedTop(NOTCH_HEIGHT + 4).reduced(10);
    calculateContentZones();
}

void SectionEditor::setSectionToEdit(const juce::String& sectionId)
{
    if (currentSectionId != sectionId)
    {
        currentSectionId = sectionId;
        updateContent();
        repaint();
    }
}

void SectionEditor::setSectionState(juce::ValueTree sectionState)
{
    if (currentSectionState == sectionState)
        return;

    if (currentSectionState.isValid())
        currentSectionState.removeListener(this);
    
    if (currentProgressionState.isValid())
        currentProgressionState.removeListener(this);

    currentSectionState = sectionState;

    if (currentSectionState.isValid())
    {
        // Écouter la section
        currentSectionState.addListener(this);
        
        // Écouter aussi la progression pour détecter les ajouts/suppressions d'accords
        Section section(currentSectionState);
        currentProgressionState = section.getProgression().getState();
        if (currentProgressionState.isValid())
            currentProgressionState.addListener(this);
        
        syncZonesFromModel();
        updateContent();
    }
    else
    {
        currentProgressionState = juce::ValueTree();
    }
}

void SectionEditor::refreshTitle() { updateContent(); }

void SectionEditor::setupSectionNameLabel()
{
    sectionNameLabel.setJustificationType(juce::Justification::centred);
    sectionNameLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    
    auto sectionNameFontOptions = fontManager->getSFProDisplay(13.0f, FontManager::FontWeight::Medium);
    sectionNameLabel.setFont(juce::Font(sectionNameFontOptions));
    
    addAndMakeVisible(sectionNameLabel);
}

void SectionEditor::parentHierarchyChanged()
{
    juce::Component::parentHierarchyChanged();
    findAppController();
}

void SectionEditor::findAppController()
{
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    appController = (pluginEditor != nullptr) ? &pluginEditor->getAppController() : nullptr;
}

void SectionEditor::updateContent()
{
    if (currentSectionId.isEmpty())
    {
        sectionNameLabel.setText("No Section", juce::dontSendNotification);
    }
    else
    {
        if (appController == nullptr)
            findAppController();
        
        juce::String displayName = "Section";
        
        if (currentSectionState.isValid() && appController)
        {
            int sectionId = currentSectionState.getProperty(ModelIdentifiers::id, -1);
            auto& piece = appController->getPiece();
            int sectionIndex = piece.getSectionIndexById(sectionId);
            
            if (sectionIndex >= 0)
                displayName = "Progression " + juce::String(sectionIndex + 1);
        }
        else if (currentSectionId.startsWith("Section_"))
        {
            int id = currentSectionId.getTrailingIntValue();
            displayName = "Section (ID=" + juce::String(id) + ")";
        }
        
        sectionNameLabel.setText(displayName, juce::dontSendNotification);
    }
} 

void SectionEditor::drawBorder(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(borderThickness * 0.5f);
    
    juce::Colour currentBorderColour = borderColour;
    
    if (isEditingSection())
        currentBorderColour = currentBorderColour.brighter(0.3f);
    else
        currentBorderColour = currentBorderColour.withAlpha(0.6f);
    
    g.setColour(currentBorderColour);
    g.drawRoundedRectangle(bounds, cornerRadius, borderThickness);
}

void SectionEditor::drawSeparatorLine(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void SectionEditor::drawNotch(juce::Graphics& g)
{
    if (notchArea.isEmpty())
        return;
    
    // Dessiner l'encoche avec coins arrondis en bas seulement
    auto notchBounds = notchArea.toFloat();
    
    juce::Path notchPath;
    notchPath.addRoundedRectangle(
        notchBounds.getX(), notchBounds.getY(),
        notchBounds.getWidth(), notchBounds.getHeight(),
        6.0f, 6.0f,    // cornerSize
        false, false,  // Pas de coins arrondis en haut
        true, true     // Coins arrondis en bas
    );
    
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillPath(notchPath);
    
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.strokePath(notchPath, juce::PathStrokeType(1.0f));
}

void SectionEditor::calculateContentZones()
{
    if (contentArea.isEmpty())
        return;
    
    // Proportions verticales
    constexpr float TOP_ROW_FLEX = 0.35f;
    constexpr float ZONE4_FLEX = 0.65f;
    constexpr int ROW_SPACING = 10;
    
    // Proportions horizontales (ligne 1)
    constexpr float KEY_ZONE_FLEX = 75.0f;
    constexpr float MODE_ZONE_FLEX = 25.0f;
    constexpr int COLUMN_SPACING = 10;
    
    // FlexBox principal : 2 lignes verticales
    juce::FlexBox mainFlex;
    mainFlex.flexDirection = juce::FlexBox::Direction::column;
    mainFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mainFlex.alignItems = juce::FlexBox::AlignItems::stretch;
    
    mainFlex.items.add(juce::FlexItem()
        .withFlex(TOP_ROW_FLEX)
        .withMargin(juce::FlexItem::Margin(0, 0, ROW_SPACING / 2, 0)));
    
    mainFlex.items.add(juce::FlexItem(zone4Component)
        .withFlex(ZONE4_FLEX)
        .withMargin(juce::FlexItem::Margin(ROW_SPACING / 2, 0, 0, 0)));
    
    mainFlex.performLayout(contentArea.toFloat());
    
    topRowArea = juce::Rectangle<int>(
        contentArea.getX(),
        contentArea.getY(),
        contentArea.getWidth(),
        static_cast<int>(contentArea.getHeight() * TOP_ROW_FLEX) - ROW_SPACING / 2
    );
    
    // FlexBox ligne 1 : KeyZone + ModeZone horizontalement
    juce::FlexBox topRowFlex;
    topRowFlex.flexDirection = juce::FlexBox::Direction::row;
    topRowFlex.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    topRowFlex.alignItems = juce::FlexBox::AlignItems::stretch;
    
    topRowFlex.items.add(juce::FlexItem(keyZone)
        .withFlex(KEY_ZONE_FLEX)
        .withMargin(juce::FlexItem::Margin(0, COLUMN_SPACING / 2, 0, 0)));
    
    topRowFlex.items.add(juce::FlexItem(modeZone)
        .withFlex(MODE_ZONE_FLEX)
        .withMargin(juce::FlexItem::Margin(0, 0, 0, COLUMN_SPACING / 2)));
    
    topRowFlex.performLayout(topRowArea.toFloat());
} 

void SectionEditor::bindZonesToModel()
{
    keyZone.getOnKeyChanged() = [this](int noteIndex, Diatony::BaseNote base, Diatony::Alteration alt)
    {
        juce::ignoreUnused(base);
        if (!currentSectionState.isValid()) return;
        Section section(currentSectionState);
        section.setAlteration(alt);
        section.setNote(static_cast<Diatony::Note>(noteIndex));
    };

    modeZone.onModeChanged = [this](Diatony::Mode mode)
    {
        if (!currentSectionState.isValid()) return;
        Section section(currentSectionState);
        section.setIsMajor(mode == Diatony::Mode::Major);
    };
    
    zone4Component.onChordAdded = [this](Diatony::ChordDegree degree, 
                                         Diatony::ChordQuality quality, 
                                         Diatony::ChordState state)
    {
        if (!currentSectionState.isValid()) return;
        Section section(currentSectionState);
        auto progression = section.getProgression();
        progression.addChord(degree, quality, state);
    };
    
    zone4Component.onChordRemoved = [this](int chordIndex)
    {
        if (!currentSectionState.isValid() || appController == nullptr) return;
        
        int sectionId = currentSectionState.getProperty(ModelIdentifiers::id, -1);
        int sectionIndex = appController->getPiece().getSectionIndexById(sectionId);
        
        if (sectionIndex >= 0)
            appController->removeChordFromSection(sectionIndex, chordIndex);
    };
}

void SectionEditor::syncZonesFromModel()
{
    if (!currentSectionState.isValid())
        return;

    Section section(currentSectionState);

    auto note = section.getNote();
    auto isMajor = section.getIsMajor();

    keyZone.setKey(static_cast<int>(note));
    modeZone.setSelectedMode(isMajor ? Diatony::Mode::Major : Diatony::Mode::Minor);
    
    auto progression = section.getProgression();
    std::vector<juce::ValueTree> chords;
    for (size_t i = 0; i < progression.size(); ++i)
        chords.push_back(progression.getChord(i).getState());
    zone4Component.syncWithProgression(chords);
}

void SectionEditor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                             const juce::Identifier& property)
{
    if (!currentSectionState.isValid()) return;
    
    if (treeWhosePropertyHasChanged == currentSectionState)
    {
        syncZonesFromModel();
        return;
    }
    
    if (treeWhosePropertyHasChanged.hasType(ModelIdentifiers::CHORD))
    {
        auto parent = treeWhosePropertyHasChanged.getParent();
        if (currentProgressionState.isValid() && parent == currentProgressionState)
        {
            Section section(currentSectionState);
            auto progression = section.getProgression();
            std::vector<juce::ValueTree> chords;
            for (size_t i = 0; i < progression.size(); ++i)
                chords.push_back(progression.getChord(i).getState());
            zone4Component.syncWithProgression(chords);
            return;
        }
    }
    
    juce::ignoreUnused(property);
}

void SectionEditor::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    if (!currentProgressionState.isValid()) return;
    
    if (parentTree == currentProgressionState && 
        childWhichHasBeenAdded.hasType(ModelIdentifiers::CHORD))
    {
        Section section(currentSectionState);
        auto progression = section.getProgression();
        std::vector<juce::ValueTree> chords;
        for (size_t i = 0; i < progression.size(); ++i)
            chords.push_back(progression.getChord(i).getState());
        zone4Component.syncWithProgression(chords);
    }
}

void SectionEditor::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int)
{
    if (!currentProgressionState.isValid()) return;
    
    if (parentTree == currentProgressionState && 
        childWhichHasBeenRemoved.hasType(ModelIdentifiers::CHORD))
    {
        Section section(currentSectionState);
        auto progression = section.getProgression();
        std::vector<juce::ValueTree> chords;
        for (size_t i = 0; i < progression.size(); ++i)
            chords.push_back(progression.getChord(i).getState());
        zone4Component.syncWithProgression(chords);
    }
}
