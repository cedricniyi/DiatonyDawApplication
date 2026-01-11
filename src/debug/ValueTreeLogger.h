#pragma once
#include <JuceHeader.h>

#if DEBUG

/** @brief Niveaux de verbosité pour le logger. */
enum class LogLevel
{
    MINIMAL,   // Ajout/suppression d'enfants uniquement
    NORMAL,    // Propriétés importantes + enfants
    VERBOSE    // Tous les détails + dumps XML
};

#ifndef VALUE_TREE_LOG_LEVEL
#define VALUE_TREE_LOG_LEVEL LogLevel::NORMAL
#endif

/** @brief Logger temps réel des changements ValueTree (DEBUG uniquement). */
class ValueTreeLogger : public juce::ValueTree::Listener
{
public:
    ValueTreeLogger(const juce::String& name, LogLevel level = VALUE_TREE_LOG_LEVEL, bool recursive = true) 
        : treeName(name), logLevel(level), isRecursive(recursive) {}

    void attachTo(juce::ValueTree& tree)
    {
        tree.addListener(this);
        if (isRecursive)
            attachToAllChildren(tree);
    }
    
    void detachFrom(juce::ValueTree& tree)
    {
        tree.removeListener(this);
        if (isRecursive)
            detachFromAllChildren(tree);
    }

    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override
    {
        if (logLevel == LogLevel::MINIMAL)
            return;
            
        DBG("[" << treeName << "] " << getTreePath(tree) << " → " 
            << property.toString() << " = " 
            << tree.getProperty(property).toString());
            
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
        DBG("[" << treeName << "] +Enfant: " << getTreePath(parentTree) 
            << " ← " << childWhichHasBeenAdded.getType().toString());
            
        if (isRecursive)
        {
            childWhichHasBeenAdded.addListener(this);
            attachToAllChildren(childWhichHasBeenAdded);
        }
            
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
        DBG("[" << treeName << "] -Enfant: " << getTreePath(parentTree)
            << " ✗ " << childWhichHasBeenRemoved.getType().toString() << " (index: " << index << ")");
            
        if (isRecursive)
        {
            childWhichHasBeenRemoved.removeListener(this);
            detachFromAllChildren(childWhichHasBeenRemoved);
        }
            
        if (logLevel == LogLevel::VERBOSE)
        {
            if (treeName.contains("Piece"))
                logPieceStateSummary(parentTree);
            else if (treeName.contains("Selection"))
                logSelectionStateSummary(parentTree);
        }
    }
    
    void setLogLevel(LogLevel level) { logLevel = level; }

private:
    juce::String treeName;
    LogLevel logLevel;
    bool isRecursive;
    
    void attachToAllChildren(juce::ValueTree& tree)
    {
        for (int i = 0; i < tree.getNumChildren(); ++i)
        {
            auto child = tree.getChild(i);
            child.addListener(this);
            attachToAllChildren(child);
        }
    }
    
    void detachFromAllChildren(juce::ValueTree& tree)
    {
        for (int i = 0; i < tree.getNumChildren(); ++i)
        {
            auto child = tree.getChild(i);
            child.removeListener(this);
            detachFromAllChildren(child);
        }
    }
    
    juce::String getTreePath(const juce::ValueTree& tree) const
    {
        if (!tree.isValid())
            return "INVALID";
            
        juce::String path = tree.getType().toString();
        
        if (tree.hasProperty("id"))
            path += "#" + tree.getProperty("id").toString();
        
        return path;
    }
    
    void logPieceStateSummary(const juce::ValueTree& pieceState)
    {
        if (!pieceState.isValid())
            return;
            
        auto xml = pieceState.createXml();
        if (!xml)
        {
            DBG("[" << treeName << "] ERREUR: Impossible de creer XML");
            return;
        }
        
        juce::String summary = "\n=== ETAT PIECE (XML) ===\n";
        summary += xml->toString();
        
        int numChildren = pieceState.getNumChildren();
        int numProperties = pieceState.getNumProperties();
        
        summary += "\n--- Resume: " + juce::String(numChildren) + " enfant(s), " 
                   + juce::String(numProperties) + " propriete(s)";
        
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
        DBG(summary);
    }
    
    void logSelectionStateSummary(const juce::ValueTree& selectionState)
    {
        if (!selectionState.isValid())
        {
            DBG("[" << treeName << "] Selection: INVALIDE");
            return;
        }
        
        juce::String selectionType = selectionState.getProperty("selectionType", "None").toString();
        juce::String selectedElementId = selectionState.getProperty("selectedElementId", "").toString();
        
        juce::String summary = "[" + treeName + "] Selection: " + selectionType;
        
        if (selectedElementId.isNotEmpty())
        {
            juce::String indexStr = selectedElementId.fromLastOccurrenceOf("_", false, false);
            summary += " #" + indexStr;
        }
        
        DBG(summary);
    }
};

#endif
