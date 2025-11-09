#pragma once
#include <JuceHeader.h>

#if DEBUG

// ==============================================================================
// Niveaux de verbosité pour le logger
enum class LogLevel
{
    MINIMAL,   // Seulement les changements critiques (ajout/suppression d'enfants)
    NORMAL,    // Changements de propriétés importantes + enfants (par défaut)
    VERBOSE    // Tous les détails + dumps XML
};

// Macro pour contrôler le niveau de log global (à changer selon les besoins)
#ifndef VALUE_TREE_LOG_LEVEL
#define VALUE_TREE_LOG_LEVEL LogLevel::NORMAL
#endif

// ==============================================================================
// Classe utilitaire pour logger les changements du ValueTree en temps réel.
// Cette classe n'est compilée qu'en mode DEBUG.
class ValueTreeLogger : public juce::ValueTree::Listener
{
public:
    ValueTreeLogger(const juce::String& name, LogLevel level = VALUE_TREE_LOG_LEVEL, bool recursive = true) 
        : treeName(name), logLevel(level), isRecursive(recursive) {}

    // Attacher le listener à un ValueTree et tous ses descendants
    void attachTo(juce::ValueTree& tree)
    {
        tree.addListener(this);
        
        if (isRecursive)
        {
            attachToAllChildren(tree);
        }
    }
    
    // Détacher le listener d'un ValueTree et tous ses descendants
    void detachFrom(juce::ValueTree& tree)
    {
        tree.removeListener(this);
        
        if (isRecursive)
        {
            detachFromAllChildren(tree);
        }
    }

    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override
    {
        // En mode MINIMAL, ignorer les changements de propriétés
        if (logLevel == LogLevel::MINIMAL)
            return;
            
        // Log avec le chemin dans l'arborescence
        DBG("[" << treeName << "] " << getTreePath(tree) << " → " 
            << property.toString() << " = " 
            << tree.getProperty(property).toString());
            
        // En mode VERBOSE, afficher les résumés détaillés
        if (logLevel == LogLevel::VERBOSE)
        {
            if (treeName.contains("Piece"))
                logPieceStateSummary(tree);
            else if (treeName.contains("Selection"))
                logSelectionStateSummary(tree);
        }
    }

    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override
    {
        // Toujours logger l'ajout d'enfants (important)
        DBG("[" << treeName << "] +Enfant: " << getTreePath(parentTree) 
            << " ← " << childWhichHasBeenAdded.getType().toString());
            
        // Si mode récursif, attacher le listener au nouvel enfant et ses descendants
        if (isRecursive)
        {
            childWhichHasBeenAdded.addListener(this);
            attachToAllChildren(childWhichHasBeenAdded);
        }
            
        // En mode VERBOSE, afficher les résumés détaillés
        if (logLevel == LogLevel::VERBOSE)
        {
            if (treeName.contains("Piece"))
                logPieceStateSummary(parentTree);
            else if (treeName.contains("Selection"))
                logSelectionStateSummary(parentTree);
        }
    }

    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override
    {
        // Toujours logger la suppression d'enfants (important)
        DBG("[" << treeName << "] -Enfant: " << getTreePath(parentTree)
            << " ✗ " << childWhichHasBeenRemoved.getType().toString() << " (index: " << index << ")");
            
        // Si mode récursif, détacher le listener de l'enfant et ses descendants
        if (isRecursive)
        {
            childWhichHasBeenRemoved.removeListener(this);
            detachFromAllChildren(childWhichHasBeenRemoved);
        }
            
        // En mode VERBOSE, afficher les résumés détaillés
        if (logLevel == LogLevel::VERBOSE)
        {
            if (treeName.contains("Piece"))
                logPieceStateSummary(parentTree);
            else if (treeName.contains("Selection"))
                logSelectionStateSummary(parentTree);
        }
    }
    
    // Méthode pour changer le niveau de log à la volée
    void setLogLevel(LogLevel level) { logLevel = level; }

private:
    juce::String treeName;
    LogLevel logLevel;
    bool isRecursive;
    
    // Attacher récursivement à tous les enfants
    void attachToAllChildren(juce::ValueTree& tree)
    {
        for (int i = 0; i < tree.getNumChildren(); ++i)
        {
            auto child = tree.getChild(i);
            child.addListener(this);
            attachToAllChildren(child); // Récursion pour les petits-enfants
        }
    }
    
    // Détacher récursivement de tous les enfants
    void detachFromAllChildren(juce::ValueTree& tree)
    {
        for (int i = 0; i < tree.getNumChildren(); ++i)
        {
            auto child = tree.getChild(i);
            child.removeListener(this);
            detachFromAllChildren(child); // Récursion pour les petits-enfants
        }
    }
    
    // Obtenir le chemin complet dans l'arborescence (ex: "PIECE/SECTION/PROGRESSION")
    juce::String getTreePath(const juce::ValueTree& tree) const
    {
        if (!tree.isValid())
            return "INVALID";
            
        juce::String path = tree.getType().toString();
        
        // Ajouter l'ID si disponible
        if (tree.hasProperty("id"))
        {
            path += "#" + tree.getProperty("id").toString();
        }
        
        return path;
    }
    
    void logPieceStateSummary(const juce::ValueTree& pieceState)
    {
        if (!pieceState.isValid())
            return;
            
        // Utiliser la représentation XML pour une vision précise et complète
        auto xml = pieceState.createXml();
        if (!xml)
        {
            DBG("[" << treeName << "] ERREUR: Impossible de creer XML");
            return;
        }
        
        juce::String summary = "\n=== ETAT PIECE (XML) ===\n";
        summary += xml->toString();
        
        // Ajouter un résumé rapide
        int numChildren = pieceState.getNumChildren();
        int numProperties = pieceState.getNumProperties();
        
        summary += "\n--- Resume: " + juce::String(numChildren) + " enfant(s), " 
                   + juce::String(numProperties) + " propriete(s)";
        
        // Lister les types d'enfants
        if (numChildren > 0)
        {
            summary += " [";
            for (int i = 0; i < numChildren; ++i)
            {
                if (i > 0) summary += ", ";
                summary += pieceState.getChild(i).getType().toString();
            }
            summary += "]";
        }
        
        summary += "\n========================";
        
        // Éviter les problèmes d'encodage UTF-8 en utilisant directement le String
        DBG(summary);
    }
    
    void logSelectionStateSummary(const juce::ValueTree& selectionState)
    {
        if (!selectionState.isValid())
        {
            DBG("[" << treeName << "] Selection: INVALIDE");
            return;
        }
        
        // Récupérer les propriétés de sélection
        juce::String selectionType = selectionState.getProperty("selectionType", "None").toString();
        juce::String selectedElementId = selectionState.getProperty("selectedElementId", "").toString();
        
        // Log concis sur une seule ligne
        juce::String summary = "[" + treeName + "] Selection: " + selectionType;
        
        if (selectedElementId.isNotEmpty())
        {
            // Extraire l'index depuis l'ID (format: "Type_index")
            juce::String indexStr = selectedElementId.fromLastOccurrenceOf("_", false, false);
            summary += " #" + indexStr;
        }
        
        DBG(summary);
    }
};
//==============================================================================

#endif 