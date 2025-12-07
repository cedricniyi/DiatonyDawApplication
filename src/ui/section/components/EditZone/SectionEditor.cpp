#include "SectionEditor.h"
#include "ui/PluginEditor.h"

SectionEditor::SectionEditor()
{
    setOpaque(false);  // Transparent
    
    setupSectionNameLabel();
    
    // Ajouter les composants des zones (style BaseZone)
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
    
    // Encoche en haut : petite zone pour le label centré
    constexpr int NOTCH_HEIGHT = 24;
    constexpr int NOTCH_WIDTH = 100;
    
    // Positionner le label dans l'encoche (centré horizontalement)
    int notchX = (bounds.getWidth() - NOTCH_WIDTH) / 2;
    notchArea = juce::Rectangle<int>(notchX, 0, NOTCH_WIDTH, NOTCH_HEIGHT);
    sectionNameLabel.setBounds(notchArea);
    
    // Zone du contenu : tout l'espace sous l'encoche avec padding
    contentArea = bounds.withTrimmedTop(NOTCH_HEIGHT + 4).reduced(10);
    
    // Calculer et définir les 4 zones de contenu avec Grid
    calculateContentZones();
}

void SectionEditor::setSectionToEdit(const juce::String& sectionId)
{
    if (currentSectionId != sectionId)
    {
        currentSectionId = sectionId;
        updateContent();
        // NOTE: Ne pas appeler syncZonesFromModel() ici car currentSectionState
        // n'est pas encore initialisé. setSectionState() sera appelé juste après
        // par le code appelant et fera le sync au bon moment.
        repaint();
    }
}

void SectionEditor::setSectionState(juce::ValueTree sectionState)
{
    if (currentSectionState == sectionState)
        return;

    // Détacher les anciens listeners
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
        {
            currentProgressionState.addListener(this);
        }
        
        // Synchroniser les zones seulement si on a un ValueTree valide
        syncZonesFromModel();
        
        // Mettre à jour le titre maintenant que currentSectionState est valide
        updateContent();
    }
    else
    {
        // Invalide : clear
        currentProgressionState = juce::ValueTree();
    }
}

void SectionEditor::refreshTitle()
{
    updateContent();
}

void SectionEditor::setupSectionNameLabel()
{
    // Configuration du label pour l'encoche - discret et centré
    sectionNameLabel.setJustificationType(juce::Justification::centred);
    sectionNameLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    
    // Police plus petite pour l'encoche
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
        // ✅ S'assurer qu'on a accès à appController (peut être null au premier appel)
        if (appController == nullptr)
        {
            findAppController();
        }
        
        juce::String displayName = "Section";
        
        if (currentSectionState.isValid() && appController)
        {
            int sectionId = currentSectionState.getProperty(ModelIdentifiers::id, -1);
            auto& piece = appController->getPiece();
            int sectionIndex = piece.getSectionIndexById(sectionId);
            
            if (sectionIndex >= 0)
            {
                displayName = "Progression " + juce::String(sectionIndex + 1);
            }
        }
        else if (currentSectionId.startsWith("Section_"))
        {
            // Fallback si pas d'appController ou de state valide (ne devrait plus arriver)
            int id = currentSectionId.getTrailingIntValue();
            displayName = "Section (ID=" + juce::String(id) + ")";
        }
        
        sectionNameLabel.setText(displayName, juce::dontSendNotification);
    }
} 

void SectionEditor::drawBorder(juce::Graphics& g)
{
    // Logique inspirée de OutlineTextButton::drawButtonBackground
    auto bounds = getLocalBounds().toFloat().reduced(borderThickness * 0.5f);
    
    juce::Colour currentBorderColour = borderColour;
    
    // Gestion des états comme dans OutlineTextButton
    // Pour un éditeur, on peut simuler un état "actif" quand une section est sélectionnée
    if (isEditingSection())
    {
        // État actif : bordure plus vive
        currentBorderColour = currentBorderColour.brighter(0.3f);
    }
    else
    {
        // État inactif : bordure plus discrète
        currentBorderColour = currentBorderColour.withAlpha(0.6f);
    }
    
    g.setColour(currentBorderColour);
    g.drawRoundedRectangle(bounds, cornerRadius, borderThickness);
}

void SectionEditor::drawSeparatorLine(juce::Graphics& g)
{
    // Plus utilisé - gardé pour compatibilité
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
    
    // Fond de l'encoche - semi-transparent discret
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillPath(notchPath);
    
    // Bordure subtile
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.strokePath(notchPath, juce::PathStrokeType(1.0f));
}

void SectionEditor::calculateContentZones()
{
    if (contentArea.isEmpty())
        return;
    // Précédemment j'utilisais GRID ici
    // ═══════════════════════════════════════════════════════════════════════════
    // 🎛️ PARAMÈTRES DE LAYOUT - Modifie ces valeurs pour ajuster les proportions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // Proportions verticales (entre ligne 1 et ligne 2)
    constexpr float TOP_ROW_FLEX = 0.35f;    // Ligne 1 : KeyZone + ModeZone (25%)
    constexpr float ZONE4_FLEX = 0.65f;      // Ligne 2 : Zone4 accords (75%)
    constexpr int ROW_SPACING = 10;          // Espacement entre les lignes
    
    // Proportions horizontales (dans la ligne 1)
    constexpr float KEY_ZONE_FLEX = 75.0f;   // KeyZone : 75%
    constexpr float MODE_ZONE_FLEX = 25.0f;  // ModeZone : 25%
    constexpr int COLUMN_SPACING = 10;       // Espacement entre KeyZone et ModeZone
    
    // ═══════════════════════════════════════════════════════════════════════════
    
    // === FlexBox principal : 2 lignes verticales ===
    juce::FlexBox mainFlex;
    mainFlex.flexDirection = juce::FlexBox::Direction::column;
    mainFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mainFlex.alignItems = juce::FlexBox::AlignItems::stretch;
    
    // Ligne 1 : Conteneur pour KeyZone + ModeZone
    // On crée un composant "virtuel" pour le layout, mais on utilise un FlexBox imbriqué
    mainFlex.items.add(juce::FlexItem()
        .withFlex(TOP_ROW_FLEX)
        .withMargin(juce::FlexItem::Margin(0, 0, ROW_SPACING / 2, 0)));
    
    // Ligne 2 : Zone4 (accords)
    mainFlex.items.add(juce::FlexItem(zone4Component)
        .withFlex(ZONE4_FLEX)
        .withMargin(juce::FlexItem::Margin(ROW_SPACING / 2, 0, 0, 0)));
    
    mainFlex.performLayout(contentArea.toFloat());
    
    // Récupérer la zone calculée pour la ligne 1
    topRowArea = juce::Rectangle<int>(
        contentArea.getX(),
        contentArea.getY(),
        contentArea.getWidth(),
        static_cast<int>(contentArea.getHeight() * TOP_ROW_FLEX) - ROW_SPACING / 2
    );
    
    // === FlexBox pour ligne 1 : KeyZone + ModeZone horizontalement ===
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
    // KeyZone: Note + Altération unifiées
    keyZone.getOnKeyChanged() = [this](int noteIndex, Diatony::BaseNote base, Diatony::Alteration alt)
    {
        juce::ignoreUnused(base);
        if (!currentSectionState.isValid()) return;
        Section section(currentSectionState);
        
        // Mettre à jour la note et l'altération directement
        section.setAlteration(alt);
        section.setNote(static_cast<Diatony::Note>(noteIndex));
    };

    // ModeZone: Mode Majeur/Mineur
    modeZone.onModeChanged = [this](Diatony::Mode mode)
    {
        if (!currentSectionState.isValid()) return;
        Section section(currentSectionState);
        section.setIsMajor(mode == Diatony::Mode::Major);
    };
    
    // Zone4: Ajout d'accord
    zone4Component.onChordAdded = [this](Diatony::ChordDegree degree, 
                                         Diatony::ChordQuality quality, 
                                         Diatony::ChordState state)
    {
        if (!currentSectionState.isValid()) return;
        
        // Obtenir la progression de la section et ajouter l'accord
        Section section(currentSectionState);
        auto progression = section.getProgression();
        progression.addChord(degree, quality, state);
    };
}

void SectionEditor::syncZonesFromModel()
{
    if (!currentSectionState.isValid())
    {
        DBG("[SectionEditor] syncZones: currentSectionState INVALIDE");
        return;
    }

    Section section(currentSectionState);

    auto note = section.getNote();
    auto isMajor = section.getIsMajor();

    // Synchroniser KeyZone avec la note (0-11)
    keyZone.setKey(static_cast<int>(note));
    
    // Synchroniser ModeZone avec le mode
    modeZone.setSelectedMode(isMajor ? Diatony::Mode::Major : Diatony::Mode::Minor);
    
    // Synchroniser Zone4 avec la progression d'accords (avec les valeurs réelles)
    auto progression = section.getProgression();
    std::vector<juce::ValueTree> chords;
    for (size_t i = 0; i < progression.size(); ++i)
    {
        chords.push_back(progression.getChord(i).getState());
    }
    zone4Component.syncWithProgression(chords);
    
    // Log concis: afficher seulement si vraiment nécessaire (commenter pour réduire encore)
    // DBG("[SectionEditor] Zones synced: note=" << static_cast<int>(note) 
    //     << " alt=" << static_cast<int>(alt) << " mode=" << (isMajor ? "Maj" : "Min"));
}

void SectionEditor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                             const juce::Identifier& property)
{
    if (!currentSectionState.isValid()) return;
    
    // Si la section elle-même change (tonalité, mode, altération, nom)
    if (treeWhosePropertyHasChanged == currentSectionState)
    {
        DBG("[SectionEditor] Propriété de section changée: " << property.toString());
        syncZonesFromModel();
        return;
    }
    
    // Si une propriété d'ACCORD change (degré, qualité, état) dans la progression surveillée
    if (treeWhosePropertyHasChanged.hasType(ModelIdentifiers::CHORD))
    {
        auto parent = treeWhosePropertyHasChanged.getParent();
        if (currentProgressionState.isValid() && parent == currentProgressionState)
        {
            DBG("[SectionEditor] Propriété d'accord changée: " << property.toString());
            // Resynchroniser uniquement Zone4 (les accords)
            Section section(currentSectionState);
            auto progression = section.getProgression();
            std::vector<juce::ValueTree> chords;
            for (size_t i = 0; i < progression.size(); ++i)
            {
                chords.push_back(progression.getChord(i).getState());
            }
            zone4Component.syncWithProgression(chords);
            return;
        }
    }
}

void SectionEditor::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    if (!currentProgressionState.isValid()) return;
    
    // Si un CHORD a été ajouté dans notre PROGRESSION, resynchroniser Zone4
    if (parentTree == currentProgressionState && 
        childWhichHasBeenAdded.hasType(ModelIdentifiers::CHORD))
    {
        Section section(currentSectionState);
        auto progression = section.getProgression();
        std::vector<juce::ValueTree> chords;
        for (size_t i = 0; i < progression.size(); ++i)
        {
            chords.push_back(progression.getChord(i).getState());
        }
        zone4Component.syncWithProgression(chords);
    }
}

void SectionEditor::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index)
{
    if (!currentProgressionState.isValid()) return;
    
    // Si un CHORD a été supprimé de notre PROGRESSION, resynchroniser Zone4
    if (parentTree == currentProgressionState && 
        childWhichHasBeenRemoved.hasType(ModelIdentifiers::CHORD))
    {
        Section section(currentSectionState);
        auto progression = section.getProgression();
        std::vector<juce::ValueTree> chords;
        for (size_t i = 0; i < progression.size(); ++i)
        {
            chords.push_back(progression.getChord(i).getState());
        }
        zone4Component.syncWithProgression(chords);
    }
}