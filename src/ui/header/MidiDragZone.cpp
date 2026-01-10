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
    
    juce::Colour bgColour;
    juce::Colour textColour;
    
    if (isMidiFileAvailable())
    {
        bgColour = isHovering ? juce::Colour(hoverColour) : juce::Colour(readyColour);
        textColour = juce::Colours::white;
    }
    else
    {
        bgColour = juce::Colour(inactiveColour);
        textColour = juce::Colour(0xFFAAAAAA);  // Gris clair pour le texte
    }
    
    g.setColour(bgColour);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    g.setColour(bgColour.brighter(0.2f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, 1.0f);
    
    g.setColour(textColour);
    juce::SharedResourcePointer<FontManager> fontManager;
    auto fontOptions = fontManager->getSFProDisplay(11.0f, FontManager::FontWeight::Medium);
    g.setFont(juce::Font(fontOptions));
    g.drawText("MIDI", bounds, juce::Justification::centred, false);
}

void MidiDragZone::mouseDown(const juce::MouseEvent& event)
{
    if (!isMidiFileAvailable() || !midiFile.existsAsFile())
        return;
    
    juce::StringArray filesToDrag;
    filesToDrag.add(midiFile.getFullPathName());
    
    juce::DragAndDropContainer::performExternalDragDropOfFiles(
        filesToDrag,
        false,  // canMoveFiles = false (copie uniquement)
        this,
        nullptr
    );
}

void MidiDragZone::mouseEnter(const juce::MouseEvent& event)
{
    isHovering = true;
    
    if (isMidiFileAvailable())
        setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    
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
    if (property == juce::Identifier("generationStatus") || 
        property == juce::Identifier("midiFilePath"))
    {
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
    
    if (status == "completed" && path.isNotEmpty())
    {
        midiFile = juce::File(path);
    }
    else if (status != "generating")
    {
        // Garde l'ancien fichier pendant "generating" pour ne pas perdre l'état
        midiFile = juce::File();
    }
    
    repaint();
}

bool MidiDragZone::isMidiFileAvailable() const
{
    return midiFile.existsAsFile();
}
