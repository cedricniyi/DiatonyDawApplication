#pragma once

#include <JuceHeader.h>
#include "BaseZone.h"
#include "utils/DiatonyConstants.h" // ce chemin est correct et ne le change pas
#include <vector>
#include <functional>
#include <map> // Pour mapper les valeurs de note

//==============================================================================
/**
 * Zone pour la sélection de tonalité avec 12 boutons disposés comme un clavier de piano
 */
class TonalityZone : public BaseZone
{
public:
    TonalityZone()
    : BaseZone(juce::String::fromUTF8("Tonality"))
    {
        // Mapper les valeurs de note à leur index pour un accès facile
        for(size_t i = 0; i < DiatonyConstants::NOTES.size(); ++i) {
            noteValueToIndexMap[DiatonyConstants::NOTES[i].value % 12] = i;
        }

        // Créer les 12 boutons de tonalité
        tonalityButtons.resize(DiatonyConstants::NOTES.size());
        for (size_t i = 0; i < DiatonyConstants::NOTES.size(); ++i)
        {
            const auto& noteInfo = DiatonyConstants::NOTES[i];
            
            // Utiliser le nom directement depuis la structure NoteInfo
            juce::String buttonText = juce::String(noteInfo.name);
            
            tonalityButtons[i] = std::make_unique<juce::TextButton>(buttonText);
            auto* button = tonalityButtons[i].get();
            
            // Stocker la valeur de la note dans l'ID du composant
            button->setComponentID(juce::String(noteInfo.value)); 
            
            // Définir le callback onClick
            button->onClick = [this, noteValue = noteInfo.value, buttonText]() {
                handleTonalityButtonClick(noteValue, buttonText);
            };
            
            // Style des boutons (sera ajusté dans resize)
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::ivory);
            button->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
            button->setColour(juce::TextButton::textColourOnId, juce::Colours::white); // Texte blanc si sélectionné
            button->setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(135, 206, 235)); // Bleu clair doux quand cliqué

            // Empêcher le bouton de prendre le focus clavier au clic
            button->setWantsKeyboardFocus(false);

            addAndMakeVisible(*button);
        }
        
        // Sélectionner "C" par défaut (visuellement)
        setSelectedButton(C);
    }
    
    ~TonalityZone() override = default;
    
    // Callback pour notifier le parent du changement de tonalité
    std::function<void(int noteValue)> onTonalitySelected;

protected:
    void paintContent(juce::Graphics& g, const juce::Rectangle<int>& contentBounds) override
    {
        // Pas de dessin spécifique nécessaire ici
    }
    
    // Positionner les 12 boutons comme un clavier de piano
    void resizeContent(const juce::Rectangle<int>& initialContentBounds) override
    {
        if (tonalityButtons.empty() || initialContentBounds.isEmpty()) return;

        // Appliquer un padding interne à la zone de contenu
        const int internalPadding = 15; 
        auto contentBounds = initialContentBounds.reduced(internalPadding); 

        if (contentBounds.isEmpty()) return; // Vérifier si le padding n'a pas rendu la zone vide

        const int numWhiteKeys = 7;
        const int numBlackKeys = 5;
        const float blackKeyWidthRatio = 0.6f;  // Largeur des touches noires par rapport aux blanches
        const float blackKeyHeightRatio = 0.6f; // Hauteur des touches noires par rapport aux blanches
        const int whiteKeyIndices[] = {0, 2, 4, 5, 7, 9, 11}; // Indices des notes blanches (C, D, E, F, G, A, B) dans noteValueMod12
        const int blackKeyIndices[] = {1, 3, 6, 8, 10};      // Indices des notes noires (C#, Eb, F#, Ab, Bb) dans noteValueMod12

        // Calculer les dimensions (basées sur la zone réduite)
        float whiteKeyWidth = static_cast<float>(contentBounds.getWidth()) / numWhiteKeys;
        float whiteKeyHeight = static_cast<float>(contentBounds.getHeight());
        float blackKeyWidth = whiteKeyWidth * blackKeyWidthRatio;
        float blackKeyHeight = whiteKeyHeight * blackKeyHeightRatio;

        // Positionner les touches blanches
        for (int i = 0; i < numWhiteKeys; ++i)
        {
            int noteValueMod12 = whiteKeyIndices[i];
            if (noteValueToIndexMap.count(noteValueMod12)) {
                int buttonIndex = noteValueToIndexMap[noteValueMod12];
                float x = contentBounds.getX() + i * whiteKeyWidth; // Utilise contentBounds.getX()
                tonalityButtons[buttonIndex]->setBounds(static_cast<int>(x), contentBounds.getY(), static_cast<int>(whiteKeyWidth - 1), static_cast<int>(whiteKeyHeight)); // Utilise contentBounds.getY()
                // Les couleurs sont gérées par setSelectedButton
            }
        }

        // Positionner les touches noires
        int blackKeyCounter = 0;
        for (int i = 0; i < numWhiteKeys -1; ++i) 
        {
            if (whiteKeyIndices[i] == 4 || whiteKeyIndices[i] == 11) continue; 

            if (blackKeyCounter < numBlackKeys) {
                 int noteValueMod12 = blackKeyIndices[blackKeyCounter];
                 if (noteValueToIndexMap.count(noteValueMod12)) {
                    int buttonIndex = noteValueToIndexMap[noteValueMod12];
                    float x = contentBounds.getX() + (i + 1) * whiteKeyWidth - (blackKeyWidth / 2.0f); // Utilise contentBounds.getX()
                    tonalityButtons[buttonIndex]->setBounds(static_cast<int>(x), contentBounds.getY(), static_cast<int>(blackKeyWidth), static_cast<int>(blackKeyHeight)); // Utilise contentBounds.getY()
                    // Les couleurs sont gérées par setSelectedButton
                    tonalityButtons[buttonIndex]->toFront(true); 
                    blackKeyCounter++;
                 }
            }
        }
        setSelectedButton(currentSelectedNote); 
    }

private:
    std::vector<std::unique_ptr<juce::TextButton>> tonalityButtons;
    int currentSelectedNote = C; // Initialiser avec Do
    std::map<int, size_t> noteValueToIndexMap; // Map pour trouver l'index d'un bouton par sa valeur de note (0-11)

    // Gérer le clic sur un bouton de tonalité
    void handleTonalityButtonClick(int noteValue, const juce::String& buttonText)
    {
        DBG(juce::String::fromUTF8  ("Selected tonality: ") + buttonText + " (" + juce::String(noteValue) + ")");
        
        setSelectedButton(noteValue);
        
        if (onTonalitySelected)
        {
            onTonalitySelected(noteValue);
        }
    }
    
    // Mettre à jour l'apparence des boutons pour indiquer la sélection
    void setSelectedButton(int noteValue)
    {
        currentSelectedNote = noteValue;
        int selectedNoteMod12 = noteValue % 12;

        for (size_t i = 0; i < tonalityButtons.size(); ++i)
        {
            const auto& buttonPtr = tonalityButtons[i];
            int buttonNoteValue = DiatonyConstants::NOTES[i].value % 12;
            bool isBlackKey = (buttonNoteValue == 1 || buttonNoteValue == 3 || buttonNoteValue == 6 || buttonNoteValue == 8 || buttonNoteValue == 10);

            // La police est gérée globalement par le LookAndFeel

            if (buttonNoteValue == selectedNoteMod12)
            {
                // Bouton sélectionné
                buttonPtr->setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(135, 206, 235)); // Bleu clair doux
                buttonPtr->setColour(juce::TextButton::textColourOffId, juce::Colours::white); // Texte blanc sur fond bleu
                buttonPtr->setToggleState(true, juce::dontSendNotification); 
            }
            else
            {
                // Bouton non sélectionné
                if (isBlackKey) {
                    buttonPtr->setColour(juce::TextButton::buttonColourId, juce::Colours::black);
                    buttonPtr->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
                } else {
                    buttonPtr->setColour(juce::TextButton::buttonColourId, juce::Colours::ivory);
                    buttonPtr->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
                }
                buttonPtr->setToggleState(false, juce::dontSendNotification);
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TonalityZone)
}; 