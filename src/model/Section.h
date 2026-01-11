#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Progression.h"
#include "DiatonyTypes.h"
#include "ModelIdentifiers.h"

/**
 * @brief Wrapper autour d'un ValueTree représentant une section tonale.
 * 
 * Définit une tonalité (note + altération + mode) et contient une Progression d'accords.
 * Pattern "vue" : ne stocke aucune donnée, délègue tout au ValueTree.
 */
class Section {
public:
    explicit Section(juce::ValueTree state);
    
    void setNote(Diatony::Note newNote);
    void setAlteration(Diatony::Alteration newAlteration);
    void setIsMajor(bool newIsMajor);
    void setName(const juce::String& newName);
    
    int getId() const;
    Diatony::Note getNote() const;
    Diatony::Alteration getAlteration() const;
    bool getIsMajor() const;
    juce::String getName() const;
    
    Progression getProgression() const;
    Progression getProgression();
    
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid() && state.hasType(ModelIdentifiers::SECTION); }
    
    bool isEmpty() const;
    bool hasProgression() const;
    juce::String toString() const;
    
private:
    juce::ValueTree state;
    
    static int noteToInt(Diatony::Note note);
    static Diatony::Note intToNote(int value);
    static int alterationToInt(Diatony::Alteration alteration);
    static Diatony::Alteration intToAlteration(int value);
};
