#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Progression.h"
#include "DiatonyTypes.h"
#include "ModelIdentifiers.h"

/**
 * Représente une section tonale complète avec sa tonalité, son mode et sa progression
 * Wrapper léger autour d'un ValueTree qui ne stocke pas de données lui-même
 * mais agit comme interface de haut niveau pour lire/écrire dans le ValueTree
 */
class Section {
public:
    // Constructeur avec ValueTree existant (mode wrapper)
    explicit Section(juce::ValueTree state);
    
    // Méthode statique pour créer une nouvelle Section dans un parent
    static Section createIn(juce::ValueTree parentTree, const juce::String& sectionName);
    
    // Setters (modifient directement le ValueTree)
    void setNote(Diatony::Note newNote);
    void setAlteration(Diatony::Alteration newAlteration);
    void setIsMajor(bool newIsMajor);
    void setName(const juce::String& newName);
    void setProgression(const Progression& newProgression);
    
    // Getters (lisent directement du ValueTree)
    Diatony::Note getNote() const;
    Diatony::Alteration getAlteration() const;
    bool getIsMajor() const;
    const juce::String getName() const;
    
    // Accès à la progression (wrapper créé à la demande)
    Progression getProgression() const;
    Progression getProgression();
    
    // Accès au ValueTree sous-jacent
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid(); }
    
    // Méthodes utilitaires simples
    bool isEmpty() const;
    bool isValidSection() const;  // Vérifie si la section est valide
    juce::String toString() const;
    
    // Création d'un nouveau nœud Section dans un ValueTree  
    static juce::ValueTree createSectionNode(const juce::String& sectionName);
    
private:
    juce::ValueTree state;
    
    // Helpers pour la conversion des types
    static int noteToInt(Diatony::Note note);
    static Diatony::Note intToNote(int value);
    static int alterationToInt(Diatony::Alteration alteration);
    static Diatony::Alteration intToAlteration(int value);
    
    // Assure qu'un nœud progression existe
    void ensureProgressionExists();
}; 