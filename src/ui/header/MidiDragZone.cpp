#include "MidiDragZone.h"
#include "utils/FontManager.h"

MidiDragZone::MidiDragZone()
{
    setTooltip(juce::String::fromUTF8("Glisser-déposer le fichier MIDI vers votre DAW"));
}

MidiDragZone::~MidiDragZone()
{
    if (selectionState.isValid())
        selectionState.removeListener(this);
}

void MidiDragZone::setSelectionState(juce::ValueTree& state)
{
    // Se désabonner de l'ancien état si nécessaire
    if (selectionState.isValid())
        selectionState.removeListener(this);

    selectionState = state;
    
    if (selectionState.isValid())
    {
        selectionState.addListener(this);
        updateFromSelectionState();
    }
}

void MidiDragZone::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Déterminer la couleur en fonction de l'état
    juce::Colour bgColour;
    juce::Colour textColour;
    
    if (isMidiFileAvailable())
    {
        // État prêt : coloré
        bgColour = isHovering ? juce::Colour(hoverColour) : juce::Colour(readyColour);
        textColour = juce::Colours::white;
    }
    else
    {
        // État inactif : grisé
        bgColour = juce::Colour(inactiveColour);
        textColour = juce::Colour(0xFFAAAAAA);  // Gris clair pour le texte
    }
    
    // Dessiner le fond avec coins arrondis
    g.setColour(bgColour);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Dessiner une bordure subtile
    g.setColour(bgColour.brighter(0.2f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, 1.0f);
    
    // Dessiner le texte "MIDI"
    g.setColour(textColour);
    
    // Utiliser FontManager pour la cohérence avec le reste de l'UI
    juce::SharedResourcePointer<FontManager> fontManager;
    auto fontOptions = fontManager->getSFProDisplay(11.0f, FontManager::FontWeight::Medium);
    g.setFont(juce::Font(fontOptions));
    
    g.drawText("MIDI", bounds, juce::Justification::centred, false);
}

void MidiDragZone::mouseDown(const juce::MouseEvent& event)
{
    // Ignorer si pas de fichier disponible
    if (!isMidiFileAvailable())
    {
        DBG("MidiDragZone::mouseDown() - Aucun fichier MIDI disponible");
        return;
    }
    
    // Vérifier que le fichier existe toujours sur le disque
    if (!midiFile.existsAsFile())
    {
        DBG("MidiDragZone::mouseDown() - Le fichier MIDI n'existe plus : " << midiFile.getFullPathName());
        return;
    }
    
    DBG("MidiDragZone::mouseDown() - Démarrage du drag externe...");
    DBG("  Fichier : " << midiFile.getFullPathName());
    
    // Créer la liste des fichiers à drag (un seul fichier)
    juce::StringArray filesToDrag;
    filesToDrag.add(midiFile.getFullPathName());
    
    // Lancer le drag externe vers le système d'exploitation
    // canMoveFiles = false : on copie le fichier, on ne le déplace pas
    bool dragStarted = juce::DragAndDropContainer::performExternalDragDropOfFiles(
        filesToDrag,
        false,  // canMoveFiles = false (copie uniquement)
        this,   // sourceComponent
        nullptr // callback (pas besoin de notification)
    );
    
    if (dragStarted)
    {
        DBG("  ✓ Drag externe démarré avec succès");
    }
    else
    {
        DBG("  ✗ Échec du démarrage du drag externe");
    }
}

void MidiDragZone::mouseEnter(const juce::MouseEvent& event)
{
    isHovering = true;
    
    // Changer le curseur si un fichier est disponible
    if (isMidiFileAvailable())
    {
        setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    }
    
    repaint();
}

void MidiDragZone::mouseExit(const juce::MouseEvent& event)
{
    isHovering = false;
    setMouseCursor(juce::MouseCursor::NormalCursor);
    repaint();
}

void MidiDragZone::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                            const juce::Identifier& property)
{
    // Réagir aux changements de generationStatus ou midiFilePath
    if (property == juce::Identifier("generationStatus") || 
        property == juce::Identifier("midiFilePath"))
    {
        // Mettre à jour sur le message thread (thread-safe)
        juce::MessageManager::callAsync([this]() {
            updateFromSelectionState();
        });
    }
}

void MidiDragZone::updateFromSelectionState()
{
    if (!selectionState.isValid())
        return;
    
    juce::String status = selectionState.getProperty("generationStatus", "").toString();
    juce::String path = selectionState.getProperty("midiFilePath", "").toString();
    
    DBG("MidiDragZone::updateFromSelectionState()");
    DBG("  generationStatus = " << status);
    DBG("  midiFilePath = " << path);
    
    // Mettre à jour le fichier MIDI si la génération est terminée avec succès
    if (status == "completed" && path.isNotEmpty())
    {
        midiFile = juce::File(path);
        DBG("  → Fichier MIDI mis à jour : " << midiFile.getFullPathName());
    }
    else
    {
        // Pas de fichier disponible (erreur ou génération en cours)
        // On garde l'ancien fichier si status == "generating" pour ne pas perdre l'état
        if (status != "generating")
        {
            midiFile = juce::File();
            DBG("  → Fichier MIDI effacé");
        }
    }
    
    // Rafraîchir l'affichage
    repaint();
}

bool MidiDragZone::isMidiFileAvailable() const
{
    return midiFile.existsAsFile();
}

