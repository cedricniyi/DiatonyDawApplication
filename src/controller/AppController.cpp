#include "AppController.h"

AppController::AppController() 
    : piece(), currentEditMode(EditMode::Overview), selectionState(ContextIdentifiers::SELECTION_STATE)
{
    selectionState.setProperty(ContextIdentifiers::selectionType, "None", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, "", &piece.getUndoManager());
}

AppController::AppController(const juce::String& pieceTitle) 
    : piece(pieceTitle), currentEditMode(EditMode::Overview), selectionState(ContextIdentifiers::SELECTION_STATE)
{
    selectionState.setProperty(ContextIdentifiers::selectionType, "None", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, "", &piece.getUndoManager());
}

void AppController::addNewSection(const juce::String& sectionName)
{
    piece.addSection(sectionName);
    int newSectionIndex = getSectionCount() - 1;
    selectSection(newSectionIndex);
}

void AppController::removeSection(int sectionIndex)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
    
    auto section = piece.getSection(sectionIndex);
    int sectionId = section.getState().getProperty(ModelIdentifiers::id, -1);
    
    juce::String currentSelectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    juce::String currentElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String sectionElementId = "Section_" + juce::String(sectionId);
    
    if (currentSelectionType == "Section" && currentElementId == sectionElementId)
        clearSelection();
    else if (currentSelectionType == "Modulation")
        clearSelection(); // Modulations adjacentes supprimées avec la section
    
    piece.removeSection(sectionIndex);
}

void AppController::selectSection(int sectionIndex)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
    
    auto section = piece.getSection(sectionIndex);
    int sectionId = section.getState().getProperty(ModelIdentifiers::id, -1);
    
    selectionState.setProperty(ContextIdentifiers::selectionType, "Section", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              "Section_" + juce::String(sectionId), 
                              &piece.getUndoManager());
    setEditMode(EditMode::SectionEdit);
}

void AppController::selectModulation(int modulationIndex)
{
    if (!isValidModulationIndex(modulationIndex))
        return;
    
    auto modulation = piece.getModulation(modulationIndex);
    int modulationId = modulation.getState().getProperty(ModelIdentifiers::id, -1);
    
    selectionState.setProperty(ContextIdentifiers::selectionType, "Modulation", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              "Modulation_" + juce::String(modulationId), 
                              &piece.getUndoManager());
    setEditMode(EditMode::Overview);
}

void AppController::addChordToSection(int sectionIndex, Diatony::ChordDegree degree, 
                                     Diatony::ChordQuality quality, Diatony::ChordState state)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
        
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    progression.addChord(degree, quality, state);
    
    int newChordIndex = static_cast<int>(progression.size()) - 1;
    selectChord(sectionIndex, newChordIndex);
}

void AppController::removeChordFromSection(int sectionIndex, int chordIndex)
{
    if (!isValidChordIndex(sectionIndex, chordIndex))
        return;
    
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    auto chord = progression.getChord(static_cast<size_t>(chordIndex));
    int chordId = chord.getId();
    
    juce::String currentSelectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    juce::String currentElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String chordElementId = "Chord_" + juce::String(chordId);
    
    if (currentSelectionType == "Chord" && currentElementId == chordElementId)
        clearSelection();
    
    progression.removeChord(static_cast<size_t>(chordIndex));
}

void AppController::selectChord(int sectionIndex, int chordIndex)
{
    if (!isValidChordIndex(sectionIndex, chordIndex))
        return;
    
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    auto chord = progression.getChord(static_cast<size_t>(chordIndex));
    int chordId = chord.getId();
    
    selectionState.setProperty(ContextIdentifiers::selectionType, "Chord", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              "Chord_" + juce::String(chordId), 
                              &piece.getUndoManager());
    setEditMode(EditMode::ChordEdit);
}

void AppController::clearSelection()
{
    juce::String currentSelectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    if (currentSelectionType == "None")
        return;
        
    selectionState.setProperty(ContextIdentifiers::selectionType, "None", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, "", &piece.getUndoManager());
    setEditMode(EditMode::Overview);
}

void AppController::clearPiece()
{
    piece.clear();
    clearSelection();
}

void AppController::setPieceTitle(const juce::String& title)
{
    piece.setTitle(title);
}

void AppController::startGeneration()
{
    if (piece.isEmpty())
    {
        selectionState.setProperty("generationStatus", "error", nullptr);
        selectionState.setProperty("generationError", 
            juce::String::fromUTF8("La pièce est vide. Veuillez ajouter au moins une section."), nullptr);
        return;
    }
    
    selectionState.setProperty("generationStatus", "generating", nullptr);
    
    juce::String dummyPath = "";
    bool launched = generationService.startGeneration(piece, dummyPath, this);
    
    if (!launched)
    {
        selectionState.setProperty("generationStatus", "error", nullptr);
        selectionState.setProperty("generationError", generationService.getLastError(), nullptr);
    }
}

bool AppController::loadProjectFromFile(const juce::File& file)
{
    if (!file.existsAsFile())
        return false;
    
    if (!file.hasFileExtension(".diatony") && !file.hasFileExtension(".xml"))
        return false;
    
    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr)
        return false;
    
    if (xml->getTagName() != "Piece")
        return false;
    
    juce::ValueTree newState = juce::ValueTree::fromXml(*xml);
    if (!newState.isValid())
        return false;
    
    // nullptr : pas d'undo pour le chargement complet
    piece.getState().copyPropertiesAndChildrenFrom(newState, nullptr);
    clearSelection();
    
    return true;
}

void AppController::handleAsyncUpdate()
{
    bool success = generationService.getLastGenerationSuccess();
    
    if (success)
    {
        juce::String midiPath = generationService.getLastGeneratedMidiPath();
        if (midiPath.isNotEmpty())
        {
            juce::File midiFile(midiPath);
            juce::File diatonyFile = midiFile.withFileExtension("diatony");
            juce::String xmlContent = piece.getState().toXmlString();
            diatonyFile.replaceWithText(xmlContent);
        }
        
        selectionState.setProperty("generationStatus", "completed", nullptr);
        selectionState.setProperty("midiFilePath", midiPath, nullptr);
    }
    else
    {
        selectionState.setProperty("generationStatus", "error", nullptr);
        selectionState.setProperty("generationError", generationService.getLastError(), nullptr);
    }
}

void AppController::undo()
{
    if (canUndo())
    {
        piece.getUndoManager().undo();
        clearSelection();
    }
}

void AppController::redo()
{
    if (canRedo())
    {
        piece.getUndoManager().redo();
        clearSelection();
    }
}

bool AppController::canUndo() const { return piece.getUndoManager().canUndo(); }
bool AppController::canRedo() const { return piece.getUndoManager().canRedo(); }

void AppController::setEditMode(EditMode newMode)
{
    if (currentEditMode != newMode)
        currentEditMode = newMode;
}

void AppController::updateSelectionFromIndices(int sectionIndex, int chordIndex)
{
    if (chordIndex >= 0)
        selectChord(sectionIndex, chordIndex);
    else if (sectionIndex >= 0)
        selectSection(sectionIndex);
    else
        clearSelection();
}

bool AppController::isValidSectionIndex(int index) const
{
    return index >= 0 && index < getSectionCount();
}

bool AppController::isValidModulationIndex(int index) const
{
    return index >= 0 && index < getModulationCount();
}

bool AppController::isValidChordIndex(int sectionIndex, int chordIndex) const
{
    if (!isValidSectionIndex(sectionIndex))
        return false;
        
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    return chordIndex >= 0 && chordIndex < static_cast<int>(progression.size());
} 