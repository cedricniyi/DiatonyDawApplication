#include "ModulationEditor.h"
#include "ui/PluginEditor.h"
#include "ui/DiatonyText.h"

ModulationEditor::ModulationEditor()
    : fromSectionZone(SectionChordsZone::DisplayMode::LastFour)
    , toSectionZone(SectionChordsZone::DisplayMode::FirstFour)
{
    setOpaque(false);  // Transparent comme SectionEditor
    
    setupModulationNameLabel();
    setupModulationTypeZone();
    setupStatusMessageLabel();
    setupSectionChordsZones();
    updateContent();
}

ModulationEditor::~ModulationEditor()
{
    if (currentModulationState.isValid())
        currentModulationState.removeListener(this);
    
    // Désabonner des sections et progressions
    unsubscribeFromSectionsAndProgressions();
}

void ModulationEditor::paint(juce::Graphics& g)
{
    // Dessiner l'encoche centrée en haut (style verdâtre)
    drawNotch(g);
}

void ModulationEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Encoche en haut : zone pour le label "Modulation X → Y" (centré)
    constexpr int NOTCH_HEIGHT = 24;
    constexpr int NOTCH_WIDTH = 160;
    
    // Positionner le label dans l'encoche (centré horizontalement)
    int notchX = (bounds.getWidth() - NOTCH_WIDTH) / 2;
    notchArea = juce::Rectangle<int>(notchX, 0, NOTCH_WIDTH, NOTCH_HEIGHT);
    modulationNameLabel.setBounds(notchArea);
    
    // Zone du contenu : tout l'espace sous l'encoche avec padding
    contentArea = bounds.withTrimmedTop(NOTCH_HEIGHT + 4).reduced(10);
    
    // Proportions FlexBox (facilement ajustables)
    constexpr float TYPE_ZONE_FLEX = 0.30f;      // 30% pour le type de modulation
    constexpr float STATUS_FLEX = 0.15f;         // 15% pour le message de statut
    constexpr float SECTION_ZONES_FLEX = 0.55f;  // 55% pour les 2 SectionChordsZones
    constexpr int VERTICAL_SPACING = 8;
    constexpr int HORIZONTAL_SPACING = 10;
    
    // FlexBox principal (vertical)
    juce::FlexBox mainFlex;
    mainFlex.flexDirection = juce::FlexBox::Direction::column;
    mainFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mainFlex.alignItems = juce::FlexBox::AlignItems::stretch;
    
    // 1. Zone de type de modulation
    mainFlex.items.add(juce::FlexItem(modulationTypeZone)
        .withFlex(TYPE_ZONE_FLEX)
        .withMinHeight(50.0f)
        .withMaxHeight(70.0f));
    
    // Espacement
    mainFlex.items.add(juce::FlexItem().withHeight(static_cast<float>(VERTICAL_SPACING)));
    
    // 2. Message de statut centré (légèrement agrandi)
    mainFlex.items.add(juce::FlexItem(statusMessageLabel)
        .withFlex(STATUS_FLEX)
        .withMinHeight(35.0f)
        .withMaxHeight(50.0f));
    
    // Espacement
    mainFlex.items.add(juce::FlexItem().withHeight(static_cast<float>(VERTICAL_SPACING)));
    
    // 3. Container pour les 2 SectionChordsZones côte à côte
    // On utilise un FlexBox horizontal imbriqué via performLayout manuel
    mainFlex.items.add(juce::FlexItem()
        .withFlex(SECTION_ZONES_FLEX)
        .withMinHeight(70.0f));
    
    mainFlex.performLayout(contentArea.toFloat());
    
    // Récupérer la zone allouée aux SectionChordsZones (dernier item)
    auto zonesArea = mainFlex.items.getLast().currentBounds.toNearestInt();
    
    // FlexBox horizontal pour les 2 SectionChordsZones
    juce::FlexBox horizontalFlex;
    horizontalFlex.flexDirection = juce::FlexBox::Direction::row;
    horizontalFlex.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    horizontalFlex.alignItems = juce::FlexBox::AlignItems::stretch;
    
    horizontalFlex.items.add(juce::FlexItem(fromSectionZone)
        .withFlex(1.0f)
        .withMargin(juce::FlexItem::Margin(0, HORIZONTAL_SPACING / 2.0f, 0, 0)));
    
    horizontalFlex.items.add(juce::FlexItem(toSectionZone)
        .withFlex(1.0f)
        .withMargin(juce::FlexItem::Margin(0, 0, 0, HORIZONTAL_SPACING / 2.0f)));
    
    horizontalFlex.performLayout(zonesArea.toFloat());
}

void ModulationEditor::setModulationToEdit(const juce::String& modulationId)
{
    if (currentModulationId != modulationId)
    {
        currentModulationId = modulationId;
        updateContent();
        repaint();
    }
}

void ModulationEditor::setModulationState(juce::ValueTree modulationState)
{
    if (currentModulationState == modulationState)
        return;

    // Détacher l'ancien listener
    if (currentModulationState.isValid())
        currentModulationState.removeListener(this);
    
    // Désabonner des anciennes sections et progressions
    unsubscribeFromSectionsAndProgressions();

    currentModulationState = modulationState;

    if (currentModulationState.isValid())
    {
        // Écouter la modulation
        currentModulationState.addListener(this);
        
        // S'abonner aux sections ET progressions adjacentes
        subscribeToAdjacentSectionsAndProgressions();
        
        // Synchroniser l'affichage depuis le modèle
        syncFromModel();
        
        // Mettre à jour le titre
        updateContent();
    }
}

void ModulationEditor::parentHierarchyChanged()
{
    juce::Component::parentHierarchyChanged();
    findAppController();
}

void ModulationEditor::findAppController()
{
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
    }
    else
    {
        appController = nullptr;
    }
}

void ModulationEditor::unsubscribeFromSectionsAndProgressions()
{
    if (currentSection1.isValid())
    {
        currentSection1.removeListener(this);
        currentSection1 = juce::ValueTree();
    }
    
    if (currentSection2.isValid())
    {
        currentSection2.removeListener(this);
        currentSection2 = juce::ValueTree();
    }
    
    if (currentProgression1.isValid())
    {
        currentProgression1.removeListener(this);
        currentProgression1 = juce::ValueTree();
    }
    
    if (currentProgression2.isValid())
    {
        currentProgression2.removeListener(this);
        currentProgression2 = juce::ValueTree();
    }
}

void ModulationEditor::subscribeToAdjacentSectionsAndProgressions()
{
    if (!currentModulationState.isValid() || !appController)
        return;
    
    unsubscribeFromSectionsAndProgressions();
    
    Modulation modulation(currentModulationState);
    auto& piece = appController->getPiece();
    auto [fromSection, toSection] = piece.getAdjacentSections(modulation);
    
    if (fromSection.isValid())
    {
        currentSection1 = fromSection.getState();
        if (currentSection1.isValid())
            currentSection1.addListener(this);
        
        currentProgression1 = fromSection.getProgression().getState();
        if (currentProgression1.isValid())
            currentProgression1.addListener(this);
    }
    
    if (toSection.isValid())
    {
        currentSection2 = toSection.getState();
        if (currentSection2.isValid())
            currentSection2.addListener(this);
        
        currentProgression2 = toSection.getProgression().getState();
        if (currentProgression2.isValid())
            currentProgression2.addListener(this);
    }
}

void ModulationEditor::setupModulationNameLabel()
{
    modulationNameLabel.setJustificationType(juce::Justification::centred);
    modulationNameLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    
    auto fontOptions = fontManager->getSFProDisplay(14.0f, FontManager::FontWeight::Medium);
    modulationNameLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(modulationNameLabel);
}

void ModulationEditor::setupModulationTypeZone()
{
    modulationTypeZone.onTypeChanged = [this](Diatony::ModulationType newType) {
        onModulationTypeChanged(newType);
    };
    
    addAndMakeVisible(modulationTypeZone);
}

void ModulationEditor::setupStatusMessageLabel()
{
    statusMessageLabel.setJustificationType(juce::Justification::centred);
    statusMessageLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen.withAlpha(0.8f));
    
    auto fontOptions = fontManager->getSFProText(18.0f, FontManager::FontWeight::Regular);
    statusMessageLabel.setFont(juce::Font(fontOptions).italicised());
    
    addAndMakeVisible(statusMessageLabel);
}

void ModulationEditor::setupSectionChordsZones()
{
    // Configuration des callbacks pour la sélection d'accords
    fromSectionZone.onChordSelected = [this](int chordIndex) {
        onFromChordSelected(chordIndex);
    };
    
    toSectionZone.onChordSelected = [this](int chordIndex) {
        onToChordSelected(chordIndex);
    };
    
    addAndMakeVisible(fromSectionZone);
    addAndMakeVisible(toSectionZone);
}

void ModulationEditor::onModulationTypeChanged(Diatony::ModulationType newType)
{
    if (!currentModulationState.isValid() || !appController)
        return;
    
    Modulation modulation(currentModulationState);
    modulation.setModulationType(newType);
    
    updateIntervalControlsVisibility();
    
    DBG("[ModulationEditor] Type de modulation changé: " << DiatonyText::getModulationTypeName(newType));
}

void ModulationEditor::onFromChordSelected(int chordIndex)
{
    if (!currentModulationState.isValid())
        return;
    
    Modulation modulation(currentModulationState);
    modulation.setFromChordIndex(chordIndex);
    
    DBG("[ModulationEditor] fromChordIndex changé: " << chordIndex);
}

void ModulationEditor::onToChordSelected(int chordIndex)
{
    if (!currentModulationState.isValid())
        return;
    
    Modulation modulation(currentModulationState);
    modulation.setToChordIndex(chordIndex);
    
    DBG("[ModulationEditor] toChordIndex changé: " << chordIndex);
}

void ModulationEditor::updateContent()
{
    if (currentModulationId.isEmpty())
    {
        modulationNameLabel.setText("No Modulation", juce::dontSendNotification);
        modulationTypeZone.setEnabled(false);
        statusMessageLabel.setText("", juce::dontSendNotification);
        fromSectionZone.clear();
        toSectionZone.clear();
    }
    else
    {
        juce::String displayName = "Modulation";
        
        if (currentModulationState.isValid() && appController)
        {
            Modulation modulation(currentModulationState);
            auto& piece = appController->getPiece();
            
            int fromSectionId = modulation.getFromSectionId();
            int toSectionId = modulation.getToSectionId();
            
            int fromSectionIndex = piece.getSectionIndexById(fromSectionId);
            int toSectionIndex = piece.getSectionIndexById(toSectionId);
            
            if (fromSectionIndex >= 0 && toSectionIndex >= 0)
            {
                displayName = "Modulation " + juce::String(fromSectionIndex + 1) + 
                              juce::String::fromUTF8(" → ") + juce::String(toSectionIndex + 1);
            }
        }
        
        modulationNameLabel.setText(displayName, juce::dontSendNotification);
        modulationTypeZone.setEnabled(true);
    }
}

void ModulationEditor::syncFromModel()
{
    if (!currentModulationState.isValid())
    {
        fromSectionZone.clear();
        toSectionZone.clear();
        statusMessageLabel.setText("", juce::dontSendNotification);
        return;
    }
    
    if (!appController)
    {
        fromSectionZone.clear();
        toSectionZone.clear();
        return;
    }
    
    Modulation modulation(currentModulationState);
    
    // Synchroniser le type de modulation
    auto modulationType = modulation.getModulationType();
    modulationTypeZone.setSelectedType(modulationType);
    
    // Récupérer les sections adjacentes
    auto& piece = appController->getPiece();
    auto [fromSection, toSection] = piece.getAdjacentSections(modulation);
    
    if (fromSection.isValid() && toSection.isValid())
    {
        // Récupérer les index des sections
        int fromSectionId = modulation.getFromSectionId();
        int toSectionId = modulation.getToSectionId();
        int fromSectionIndex = piece.getSectionIndexById(fromSectionId);
        int toSectionIndex = piece.getSectionIndexById(toSectionId);
        
        // Mettre à jour les SectionChordsZones
        fromSectionZone.setSection(fromSection, fromSectionIndex);
        toSectionZone.setSection(toSection, toSectionIndex);
        
        // Sélectionner les accords actuels si en mode Pivot
        int fromChordIndex = modulation.getFromChordIndex();
        int toChordIndex = modulation.getToChordIndex();
        
        if (fromChordIndex >= 0)
            fromSectionZone.setSelectedChordIndex(fromChordIndex);
        
        if (toChordIndex >= 0)
            toSectionZone.setSelectedChordIndex(toChordIndex);
        
        // Mettre à jour la visibilité des contrôles
        updateIntervalControlsVisibility();
    }
    else
    {
        fromSectionZone.clear();
        toSectionZone.clear();
    }
}

void ModulationEditor::updateIntervalControlsVisibility()
{
    auto modulationType = modulationTypeZone.getSelectedType();
    
    // CAS A : Accord Pivot (PivotChord) - L'utilisateur définit l'intervalle manuellement
    if (modulationType == Diatony::ModulationType::PivotChord)
    {
        statusMessageLabel.setText(juce::String::fromUTF8("Interval to define manually"), 
                                   juce::dontSendNotification);
        statusMessageLabel.setColour(juce::Label::textColourId, juce::Colours::orange.withAlpha(0.8f));
        
        // Activer la sélection sur les SectionChordsZones
        fromSectionZone.setEnabled(true);
        toSectionZone.setEnabled(true);
    }
    // CAS B : Perfect Cadence, Alteration, Chromatic - Géré automatiquement
    else
    {
        statusMessageLabel.setText(juce::String::fromUTF8("Transition managed automatically by the solver"), 
                                   juce::dontSendNotification);
        statusMessageLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen.withAlpha(0.8f));
        
        // Désactiver la sélection (juste visualisation)
        fromSectionZone.setEnabled(false);
        toSectionZone.setEnabled(false);
    }
}

void ModulationEditor::drawNotch(juce::Graphics& g)
{
    if (notchArea.isEmpty())
        return;
    
    auto notchBounds = notchArea.toFloat();
    
    juce::Path notchPath;
    notchPath.addRoundedRectangle(
        notchBounds.getX(), notchBounds.getY(),
        notchBounds.getWidth(), notchBounds.getHeight(),
        6.0f, 6.0f,
        false, false,  // Pas de coins arrondis en haut
        true, true     // Coins arrondis en bas
    );
    
    // Fond de l'encoche - verdâtre semi-transparent
    g.setColour(notchBackgroundColour);
    g.fillPath(notchPath);
    
    // Bordure subtile verdâtre
    g.setColour(notchBorderColour);
    g.strokePath(notchPath, juce::PathStrokeType(1.0f));
}

// === ValueTree::Listener ===

void ModulationEditor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                               const juce::Identifier& property)
{
    // Si la modulation change (type, indices d'accords)
    if (currentModulationState.isValid() && treeWhosePropertyHasChanged == currentModulationState)
    {
        DBG("[ModulationEditor] Propriété de la modulation changée: " << property.toString());
        syncFromModel();
        return;
    }
    
    // Si une propriété de SECTION change (tonalité, mode, altération)
    if ((currentSection1.isValid() && treeWhosePropertyHasChanged == currentSection1) ||
        (currentSection2.isValid() && treeWhosePropertyHasChanged == currentSection2))
    {
        DBG("[ModulationEditor] Propriété de section changée: " << property.toString());
        syncFromModel();
        return;
    }
    
    // Si une propriété d'ACCORD change (degré, qualité, état) dans une des progressions surveillées
    if (treeWhosePropertyHasChanged.hasType(ModelIdentifiers::CHORD))
    {
        auto parent = treeWhosePropertyHasChanged.getParent();
        if ((currentProgression1.isValid() && parent == currentProgression1) ||
            (currentProgression2.isValid() && parent == currentProgression2))
        {
            DBG("[ModulationEditor] Propriété d'accord changée: " << property.toString());
            syncFromModel();
            return;
        }
    }
    
    juce::ignoreUnused(property);
}

void ModulationEditor::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    // Si un CHORD a été ajouté dans une des progressions, resynchroniser l'affichage
    if (childWhichHasBeenAdded.hasType(ModelIdentifiers::CHORD))
    {
        if ((currentProgression1.isValid() && parentTree == currentProgression1) ||
            (currentProgression2.isValid() && parentTree == currentProgression2))
        {
            DBG("[ModulationEditor] Accord ajouté détecté, rafraîchissement de l'affichage...");
            syncFromModel();
        }
    }
}

void ModulationEditor::valueTreeChildRemoved(juce::ValueTree& parentTree, 
                                            juce::ValueTree& childWhichHasBeenRemoved, 
                                            int index)
{
    // Si un CHORD a été supprimé dans une des progressions, resynchroniser l'affichage
    juce::ignoreUnused(index);
    
    if (childWhichHasBeenRemoved.hasType(ModelIdentifiers::CHORD))
    {
        if ((currentProgression1.isValid() && parentTree == currentProgression1) ||
            (currentProgression2.isValid() && parentTree == currentProgression2))
        {
            DBG("[ModulationEditor] Accord supprimé détecté, rafraîchissement de l'affichage...");
            syncFromModel();
        }
    }
} 