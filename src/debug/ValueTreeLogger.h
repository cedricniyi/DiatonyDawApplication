#pragma once
#include <JuceHeader.h>

#if DEBUG

// ==============================================================================
// Classe utilitaire pour logger les changements du ValueTree en temps réel.
// Cette classe n'est compilée qu'en mode DEBUG.
class ValueTreeLogger : public juce::ValueTree::Listener
{
public:
    ValueTreeLogger(const juce::String& name) : treeName(name) {}

    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override
    {
        DBG("Logger<" << treeName << ">: Propriété changée -> "
            << property.toString() << " = "
            << tree.getProperty(property).toString());
            
        // Si c'est le logger de la pièce, afficher un résumé complet
        if (treeName.contains("Piece"))
        {
            logPieceStateSummary(tree);
        }
        // Si c'est le logger de sélection, afficher un résumé de sélection
        else if (treeName.contains("Selection"))
        {
            logSelectionStateSummary(tree);
        }
    }

    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override
    {
        DBG("Logger<" << treeName << ">: Enfant ajouté -> " 
            << childWhichHasBeenAdded.getType().toString());
            
        if (treeName.contains("Piece"))
        {
            logPieceStateSummary(parentTree);
        }
        else if (treeName.contains("Selection"))
        {
            logSelectionStateSummary(parentTree);
        }
    }

    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override
    {
        DBG("Logger<" << treeName << ">: Enfant retiré -> " 
            << childWhichHasBeenRemoved.getType().toString() << " (index: " << index << ")");
            
        if (treeName.contains("Piece"))
        {
            logPieceStateSummary(parentTree);
        }
        else if (treeName.contains("Selection"))
        {
            logSelectionStateSummary(parentTree);
        }
    }

private:
    juce::String treeName;
    
    void logPieceStateSummary(const juce::ValueTree& pieceState)
    {
        if (!pieceState.isValid())
            return;
            
        // Utiliser la représentation XML pour une vision précise et complète
        auto xml = pieceState.createXml();
        if (!xml)
        {
            DBG("=== ERREUR: Impossible de créer XML pour le ValueTree ===");
            return;
        }
        
        juce::String summary = "=== ETAT DE LA PIECE (XML) ===\n";
        summary += xml->toString();
        summary += "\n========================";
        
        // Ajouter aussi un résumé rapide basé sur le XML
        int numChildren = pieceState.getNumChildren();
        int numProperties = pieceState.getNumProperties();
        
        summary += "\nResume rapide:";
        summary += "\n  Nombre d'enfants directs: " + juce::String(numChildren);
        summary += "\n  Nombre de proprietes: " + juce::String(numProperties);
        
        // Lister les types d'enfants pour diagnostic
        if (numChildren > 0)
        {
            summary += "\n  Types d'enfants:";
            for (int i = 0; i < numChildren; ++i)
            {
                auto child = pieceState.getChild(i);
                summary += "\n    [" + juce::String(i) + "] " + child.getType().toString();
            }
        }
        
        summary += "\n========================";
        
        DBG(juce::String::fromUTF8(summary.toUTF8()));
    }
    
    void logSelectionStateSummary(const juce::ValueTree& selectionState)
    {
        if (!selectionState.isValid())
        {
            DBG("=== ETAT DE SELECTION: INVALIDE ===");
            return;
        }
        
        // Récupérer les propriétés de sélection
        juce::String selectionType = selectionState.getProperty("selectionType", "None").toString();
        juce::String selectedElementId = selectionState.getProperty("selectedElementId", "").toString();
        
        juce::String summary = "=== ETAT DE SELECTION ===\n";
        summary += "Type de sélection: " + selectionType + "\n";
        summary += "ID de l'élément: " + selectedElementId + "\n";
        
        // Ajouter des détails contextuels selon le type
        if (selectionType == "Section")
        {
            // Extraire l'index de la section depuis l'ID (format: "Section_0")
            juce::String indexStr = selectedElementId.fromLastOccurrenceOf("_", false, false);
            summary += "-> Section sélectionnée (index: " + indexStr + ")\n";
        }
        else if (selectionType == "Chord")
        {
            juce::String indexStr = selectedElementId.fromLastOccurrenceOf("_", false, false);
            summary += "-> Accord sélectionné (index: " + indexStr + ")\n";
        }
        else if (selectionType == "Modulation")
        {
            juce::String indexStr = selectedElementId.fromLastOccurrenceOf("_", false, false);
            summary += "-> Modulation sélectionnée (index: " + indexStr + ")\n";
        }
        else if (selectionType == "None")
        {
            summary += "-> Aucune sélection active\n";
        }
        
        summary += "========================";
        
        DBG(juce::String::fromUTF8(summary.toUTF8()));
    }
};
//==============================================================================

#endif 