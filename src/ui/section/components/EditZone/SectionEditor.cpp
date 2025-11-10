#include "SectionEditor.h"

SectionEditor::SectionEditor()
    : ColoredPanel(juce::Colours::lightblue.withAlpha(0.25f)) // Bleu clair plus visible
{
    setupSectionNameLabel();
    
    // Ajouter les composants des 4 zones
    addAndMakeVisible(zone1Component);
    addAndMakeVisible(zone2Component);
    addAndMakeVisible(zone3Component);
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
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
    
    // Dessiner la bordure sophistiquée
    drawBorder(g);
    
    // Dessiner le trait de séparation
    drawSeparatorLine(g);
}

void SectionEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // Zone du header : titre + espacement pour le trait
    headerArea = bounds.removeFromTop(60); // Hauteur augmentée pour inclure le trait
    
    // Zone du contenu : le reste de l'espace
    contentArea = bounds; // Garde tout le reste pour le contenu futur
    
    // Positionner le titre dans la zone header (avec un peu de marge)
    auto titleBounds = headerArea.reduced(0, 5).removeFromTop(40);
    sectionNameLabel.setBounds(titleBounds);
    
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
    }
    else
    {
        // Invalide : clear
        currentProgressionState = juce::ValueTree();
    }
}

void SectionEditor::setupSectionNameLabel()
{
    // Configuration du label de nom de section
    sectionNameLabel.setJustificationType(juce::Justification::centred);
    sectionNameLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
    
    // Application de la police via FontManager - taille légèrement plus grande
    auto sectionNameFontOptions = fontManager->getSFProDisplay(28.0f, FontManager::FontWeight::Bold);
    sectionNameLabel.setFont(juce::Font(sectionNameFontOptions));
    
    addAndMakeVisible(sectionNameLabel);
}

void SectionEditor::updateContent()
{
    if (currentSectionId.isEmpty())
    {
        sectionNameLabel.setText("No Section", juce::dontSendNotification);
    }
    else
    {
        // Extraire l'index de la section depuis l'ID (ex: "Section_0" -> "Section 1")
        juce::String displayName = "Section";
        if (currentSectionId.startsWith("Section_"))
        {
            int index = currentSectionId.getTrailingIntValue();
            displayName = "Section " + juce::String(index + 1);
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
    // Dessiner un trait de séparation entre le header et le contenu
    auto separatorY = headerArea.getBottom(); // 10px de décalage depuis le header
    auto separatorStart = headerArea.getX();
    auto separatorEnd = headerArea.getRight();
    
    g.setColour(juce::Colours::darkblue.withAlpha(0.15f));
    g.drawHorizontalLine(separatorY, static_cast<float>(separatorStart), static_cast<float>(separatorEnd));
}

void SectionEditor::calculateContentZones()
{
    if (contentArea.isEmpty())
        return;
    
    // Utiliser juce::Grid pour diviser la zone de contenu en 4 zones
    juce::Grid grid;
    
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;
    
    // Définir les lignes : 40% pour la première ligne, 60% pour la seconde
    grid.templateRows = { 
        Track(Fr(40)), 
        Track(Fr(60)) 
    };
    
    // Définir les colonnes : 3 colonnes égales pour la première ligne
    grid.templateColumns = { 
        Track(Fr(1)), 
        Track(Fr(1)), 
        Track(Fr(1))
    };
    
    // Espacement entre les zones
    grid.columnGap = Px(10);
    grid.rowGap = Px(10);
    
    // Ajouter les composants réels à la grille
    grid.items.add(juce::GridItem(zone1Component).withArea(1, 1));  // Zone 1: ligne 1, colonne 1
    grid.items.add(juce::GridItem(zone2Component).withArea(1, 2));  // Zone 2: ligne 1, colonne 2
    grid.items.add(juce::GridItem(zone3Component).withArea(1, 3));  // Zone 3: ligne 1, colonne 3
    grid.items.add(juce::GridItem(zone4Component).withArea(2, 1, 3, 4)); // Zone 4: ligne 2, colonnes 1-3
    
    // Appliquer le layout
    grid.performLayout(contentArea);
    
    // Stocker les bounds calculés pour référence (optionnel, pour debugging)
    zone1Area = zone1Component.getBounds();
    zone2Area = zone2Component.getBounds();
    zone3Area = zone3Component.getBounds();
    zone4Area = zone4Component.getBounds();
} 

void SectionEditor::bindZonesToModel()
{
    // Zone1: Base note
    zone1Component.onBaseNoteChanged = [this](Diatony::BaseNote base)
    {
        if (!currentSectionState.isValid()) return;
        Section section(currentSectionState);
        auto alt = section.getAlteration();
        auto note = Diatony::toDiatonyNote(base, alt);
        section.setNote(note);
    };

    // Zone2: Alteration
    zone2Component.onAlterationChanged = [this](Diatony::Alteration alt)
    {
        if (!currentSectionState.isValid()) return;
        Section section(currentSectionState);
        // Conserver la lettre (BaseNote) courante: la déduire avec l'ancienne altération
        auto oldAlt = section.getAlteration();
        auto note = section.getNote();
        auto base = Diatony::toBaseNote(note, oldAlt);

        // Appliquer la nouvelle altération et recalculer la note chromatique
        section.setAlteration(alt);
        section.setNote(Diatony::toDiatonyNote(base, alt));
    };

    // Zone3: Mode
    zone3Component.onModeChanged = [this](Diatony::Mode mode)
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
    auto alt = section.getAlteration();
    auto isMajor = section.getIsMajor();

    // Déduire BaseNote depuis Note + Altération
    auto base = Diatony::toBaseNote(note, alt);

    zone1Component.setSelectedBaseNote(base);
    zone2Component.setSelectedAlteration(alt);
    zone3Component.setSelectedMode(isMajor ? Diatony::Mode::Major : Diatony::Mode::Minor);
    
    // Synchroniser Zone4 avec la progression d'accords (avec les valeurs réelles)
    auto progression = section.getProgression();
    std::vector<juce::ValueTree> chords;
    for (size_t i = 0; i < progression.size(); ++i)
    {
        chords.push_back(progression.getChordState(i));
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
                chords.push_back(progression.getChordState(i));
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
            chords.push_back(progression.getChordState(i));
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
            chords.push_back(progression.getChordState(i));
        }
        zone4Component.syncWithProgression(chords);
    }
}