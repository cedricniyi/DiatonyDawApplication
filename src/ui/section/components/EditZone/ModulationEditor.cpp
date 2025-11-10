#include "ModulationEditor.h"
#include "ui/PluginEditor.h"
#include "ui/DiatonyText.h"

ModulationEditor::ModulationEditor()
    : ColoredPanel(juce::Colours::lightgreen.withAlpha(0.25f)) // Vert clair plus visible
{
    setupModulationNameLabel();
    setupInfoLabels();
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
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
    
    // Dessiner la bordure sophistiquée
    drawBorder(g);
    
    // Dessiner le trait de séparation
    drawSeparatorLine(g);
}

void ModulationEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // Zone du header : titre + espacement pour le trait
    headerArea = bounds.removeFromTop(60);
    
    // Zone du contenu : le reste de l'espace
    contentArea = bounds;
    
    // Positionner le titre dans la zone header
    auto titleBounds = headerArea.reduced(0, 5).removeFromTop(40);
    modulationNameLabel.setBounds(titleBounds);
    
    // Positionner les labels d'information dans la zone de contenu
    auto infoBounds = contentArea.reduced(10);
    int labelHeight = 25;
    int spacing = 10;
    
    modulationTypeLabel.setBounds(infoBounds.removeFromTop(labelHeight));
    infoBounds.removeFromTop(spacing);
    
    fromSectionLabel.setBounds(infoBounds.removeFromTop(labelHeight));
    infoBounds.removeFromTop(spacing / 2);
    
    // Liste des accords de la section source (multi-ligne)
    fromChordsLabel.setBounds(infoBounds.removeFromTop(labelHeight * 2));
    infoBounds.removeFromTop(spacing);
    
    toSectionLabel.setBounds(infoBounds.removeFromTop(labelHeight));
    infoBounds.removeFromTop(spacing / 2);
    
    // Liste des accords de la section destination (multi-ligne)
    toChordsLabel.setBounds(infoBounds.removeFromTop(labelHeight * 2));
    infoBounds.removeFromTop(spacing);
    
    chordIndicesLabel.setBounds(infoBounds.removeFromTop(labelHeight));
}

void ModulationEditor::setModulationToEdit(const juce::String& modulationId)
{
    if (currentModulationId != modulationId)
    {
        currentModulationId = modulationId;
        updateContent();
        // NOTE: Ne pas appeler syncFromModel() ici car currentModulationState
        // n'est pas encore initialisé. setModulationState() sera appelé juste après
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
    }
}

void ModulationEditor::parentHierarchyChanged()
{
    ColoredPanel::parentHierarchyChanged();
    findAppController();
}

void ModulationEditor::findAppController()
{
    // Recherche de l'AppController via la hiérarchie des composants
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
    // Désabonner des sections
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
    
    // Désabonner des progressions
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
    
    // D'abord se désabonner des anciennes sections et progressions
    unsubscribeFromSectionsAndProgressions();
    
    // Créer un wrapper Modulation depuis le ValueTree
    Modulation modulation(currentModulationState);
    
    // Récupérer les sections adjacentes
    auto& piece = appController->getPiece();
    auto [fromSection, toSection] = piece.getAdjacentSections(modulation);
    
    if (fromSection.isValid())
    {
        int fromSectionId = static_cast<int>(fromSection.getState().getProperty(ModelIdentifiers::id, -1));
        
        // S'abonner à la SECTION source (pour détecter les changements de tonalité/mode)
        currentSection1 = fromSection.getState();
        if (currentSection1.isValid())
        {
            currentSection1.addListener(this);
            DBG("[ModulationEditor] Abonné à la section source (ID=" << fromSectionId << ")");
        }
        
        // S'abonner à la PROGRESSION de la section source (pour détecter les changements d'accords)
        currentProgression1 = fromSection.getProgression().getState();
        if (currentProgression1.isValid())
        {
            currentProgression1.addListener(this);
            DBG("[ModulationEditor] Abonné à la progression de la section source (ID=" << fromSectionId << ")");
        }
    }
    
    if (toSection.isValid())
    {
        int toSectionId = static_cast<int>(toSection.getState().getProperty(ModelIdentifiers::id, -1));
        
        // S'abonner à la SECTION destination (pour détecter les changements de tonalité/mode)
        currentSection2 = toSection.getState();
        if (currentSection2.isValid())
        {
            currentSection2.addListener(this);
            DBG("[ModulationEditor] Abonné à la section destination (ID=" << toSectionId << ")");
        }
        
        // S'abonner à la PROGRESSION de la section destination (pour détecter les changements d'accords)
        currentProgression2 = toSection.getProgression().getState();
        if (currentProgression2.isValid())
        {
            currentProgression2.addListener(this);
            DBG("[ModulationEditor] Abonné à la progression de la section destination (ID=" << toSectionId << ")");
        }
    }
}

void ModulationEditor::setupModulationNameLabel()
{
    // Configuration du label de nom de modulation
    modulationNameLabel.setJustificationType(juce::Justification::centredLeft);
    modulationNameLabel.setColour(juce::Label::textColourId, juce::Colours::darkgreen);
    
    // Application de la police via FontManager - taille légèrement plus grande
    auto modulationNameFontOptions = fontManager->getSFProDisplay(28.0f, FontManager::FontWeight::Bold);
    modulationNameLabel.setFont(juce::Font(modulationNameFontOptions));
    
    addAndMakeVisible(modulationNameLabel);
}

void ModulationEditor::setupInfoLabels()
{
    // Configuration des labels d'information
    auto labelFontOptions = fontManager->getSFProText(16.0f, FontManager::FontWeight::Regular);
    juce::Font labelFont(labelFontOptions);
    
    modulationTypeLabel.setFont(labelFont);
    modulationTypeLabel.setColour(juce::Label::textColourId, juce::Colours::darkgreen.darker(0.3f));
    modulationTypeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(modulationTypeLabel);
    
    fromSectionLabel.setFont(labelFont);
    fromSectionLabel.setColour(juce::Label::textColourId, juce::Colours::darkgreen.darker(0.3f));
    fromSectionLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(fromSectionLabel);
    
    toSectionLabel.setFont(labelFont);
    toSectionLabel.setColour(juce::Label::textColourId, juce::Colours::darkgreen.darker(0.3f));
    toSectionLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(toSectionLabel);
    
    chordIndicesLabel.setFont(labelFont);
    chordIndicesLabel.setColour(juce::Label::textColourId, juce::Colours::darkgreen.darker(0.3f));
    chordIndicesLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(chordIndicesLabel);
    
    // Labels pour les listes d'accords (avec wrapping pour texte long)
    auto chordsFontOptions = fontManager->getSFProText(14.0f, FontManager::FontWeight::Regular);
    juce::Font chordsFont(chordsFontOptions);
    
    fromChordsLabel.setFont(chordsFont);
    fromChordsLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
    fromChordsLabel.setJustificationType(juce::Justification::topLeft);
    fromChordsLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(fromChordsLabel);
    
    toChordsLabel.setFont(chordsFont);
    toChordsLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
    toChordsLabel.setJustificationType(juce::Justification::topLeft);
    toChordsLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(toChordsLabel);
}

void ModulationEditor::updateContent()
{
    if (currentModulationId.isEmpty())
    {
        modulationNameLabel.setText("No Modulation", juce::dontSendNotification);
        modulationTypeLabel.setText("", juce::dontSendNotification);
        fromSectionLabel.setText("", juce::dontSendNotification);
        fromChordsLabel.setText("", juce::dontSendNotification);
        toSectionLabel.setText("", juce::dontSendNotification);
        toChordsLabel.setText("", juce::dontSendNotification);
        chordIndicesLabel.setText("", juce::dontSendNotification);
    }
    else
    {
        // Extraire l'index de la modulation depuis l'ID
        juce::String displayName = "Modulation";
        if (currentModulationId.startsWith("Modulation_"))
        {
            int index = currentModulationId.getTrailingIntValue();
            int fromSection = index + 1;
            int toSection = index + 2;
            displayName = "Modulation " + juce::String(fromSection) + " → " + juce::String(toSection);
        }
        
        modulationNameLabel.setText(displayName, juce::dontSendNotification);
    }
}

void ModulationEditor::syncFromModel()
{
    if (!currentModulationState.isValid())
    {
        DBG("[ModulationEditor] syncFromModel: currentModulationState INVALIDE");
        modulationTypeLabel.setText("Aucune information disponible", juce::dontSendNotification);
        fromSectionLabel.setText("", juce::dontSendNotification);
        fromChordsLabel.setText("", juce::dontSendNotification);
        toSectionLabel.setText("", juce::dontSendNotification);
        toChordsLabel.setText("", juce::dontSendNotification);
        chordIndicesLabel.setText("", juce::dontSendNotification);
        return;
    }
    
    if (!appController)
    {
        DBG("[ModulationEditor] syncFromModel: AppController non trouvé");
        modulationTypeLabel.setText("AppController non disponible", juce::dontSendNotification);
        fromChordsLabel.setText("", juce::dontSendNotification);
        toChordsLabel.setText("", juce::dontSendNotification);
        return;
    }
    
    // Créer un wrapper Modulation depuis le ValueTree
    Modulation modulation(currentModulationState);
    
    // Récupérer le type de modulation
    auto modulationType = modulation.getModulationType();
    juce::String typeText = "Type : " + DiatonyText::getModulationTypeName(modulationType);
    modulationTypeLabel.setText(typeText, juce::dontSendNotification);
    
    // Récupérer les indices d'accords
    int fromChordIndex = modulation.getFromChordIndex();
    int toChordIndex = modulation.getToChordIndex();
    juce::String chordText = "Accords : [" + juce::String(fromChordIndex) + " → " + juce::String(toChordIndex) + "]";
    chordIndicesLabel.setText(chordText, juce::dontSendNotification);
    
    // Récupérer les sections adjacentes via le helper de Piece
    auto& piece = appController->getPiece();
    auto [fromSection, toSection] = piece.getAdjacentSections(modulation);
    
    if (fromSection.isValid() && toSection.isValid())
    {
        // Informations de la section source
        juce::String fromText = "De : " + fromSection.getName() + " (";
        fromText += DiatonyText::getNoteName(fromSection.getNote(), fromSection.getAlteration());
        fromText += " " + juce::String(fromSection.getIsMajor() ? "Majeur" : "Mineur");
        fromText += ", " + juce::String(fromSection.getProgression().size()) + " accords)";
        fromSectionLabel.setText(fromText, juce::dontSendNotification);
        
        // Liste des accords de la section source
        juce::String fromChords = formatSectionChords(fromSection);
        fromChordsLabel.setText(fromChords, juce::dontSendNotification);
        
        // Informations de la section destination
        juce::String toText = "Vers : " + toSection.getName() + " (";
        toText += DiatonyText::getNoteName(toSection.getNote(), toSection.getAlteration());
        toText += " " + juce::String(toSection.getIsMajor() ? "Majeur" : "Mineur");
        toText += ", " + juce::String(toSection.getProgression().size()) + " accords)";
        toSectionLabel.setText(toText, juce::dontSendNotification);
        
        // Liste des accords de la section destination
        juce::String toChords = formatSectionChords(toSection);
        toChordsLabel.setText(toChords, juce::dontSendNotification);
        
        DBG("[ModulationEditor] Sync réussi: " << typeText);
    }
    else
    {
        fromSectionLabel.setText("Section source invalide", juce::dontSendNotification);
        fromChordsLabel.setText("", juce::dontSendNotification);
        toSectionLabel.setText("Section destination invalide", juce::dontSendNotification);
        toChordsLabel.setText("", juce::dontSendNotification);
        DBG("[ModulationEditor] syncFromModel: Sections invalides");
    }
}

juce::String ModulationEditor::formatSectionChords(const Section& section) const
{
    if (!section.isValid())
        return "Section invalide";
    
    auto progression = section.getProgression();
    size_t numChords = progression.size();
    
    if (numChords == 0)
        return "  (Aucun accord)";
    
    juce::String result = "  Accords : ";
    
    for (size_t i = 0; i < numChords; ++i)
    {
        auto chord = progression.getChord(i);
        
        // Formater l'accord : Degré + Qualité + État
        juce::String chordStr;
        
        // Degré (ex: "I", "II", "III", etc.)
        chordStr += DiatonyText::getChordDegreeName(chord.getDegree());
        
        // Qualité (ex: "M", "m", "dim", "aug", etc.)
        auto quality = chord.getQuality();
        if (quality != Diatony::ChordQuality::Major)  // Ne pas afficher "M" par défaut
        {
            chordStr += DiatonyText::getChordQualitySymbol(quality);
        }
        
        // État/Renversement (ex: "⁰", "¹", "²", "³")
        auto state = chord.getChordState();
        if (state != Diatony::ChordState::Fundamental)  // Ne pas afficher si fondamental
        {
            chordStr += DiatonyText::getChordStateName(state);
        }
        
        result += chordStr;
        
        // Ajouter une virgule sauf pour le dernier accord
        if (i < numChords - 1)
            result += ", ";
    }
    
    return result;
}

void ModulationEditor::drawBorder(juce::Graphics& g)
{
    // Logique inspirée de OutlineTextButton::drawButtonBackground
    auto bounds = getLocalBounds().toFloat().reduced(borderThickness * 0.5f);
    
    juce::Colour currentBorderColour = borderColour;
    
    // Gestion des états comme dans OutlineTextButton
    // Pour un éditeur, on peut simuler un état "actif" quand une modulation est sélectionnée
    if (isEditingModulation())
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

void ModulationEditor::drawSeparatorLine(juce::Graphics& g)
{
    // Dessiner un trait de séparation entre le header et le contenu
    auto separatorY = headerArea.getBottom(); // Aligné avec le bas du header
    auto separatorStart = headerArea.getX();
    auto separatorEnd = headerArea.getRight();
    
    g.setColour(juce::Colours::darkgreen.withAlpha(0.15f));
    g.drawHorizontalLine(separatorY, static_cast<float>(separatorStart), static_cast<float>(separatorEnd));
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
    // Si un CHORD a été supprimé d'une des progressions, resynchroniser l'affichage
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