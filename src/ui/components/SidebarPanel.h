#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <vector>
#include "../../utils/DatabaseManagement.h"

// Classe représentant une solution historique
class SolutionHistoryItem {
public:
    SolutionHistoryItem(int id, const juce::String& name, const juce::String& path, juce::int64 timestamp)
        : id(id), name(name), path(path), timestamp(timestamp) {}
    
    int getId() const { return id; }
    juce::String getName() const { return name; }
    juce::String getPath() const { return path; }
    juce::int64 getTimestamp() const { return timestamp; }
    
    juce::String getFormattedDate() const {
        juce::Time time(timestamp);
        return time.formatted("%Y-%m-%d %H:%M");
    }
    
private:
    int id;
    juce::String name;
    juce::String path;
    juce::int64 timestamp;
};

// Model pour la liste de solutions
class SolutionListModel : public juce::ListBoxModel {
public:
    SolutionListModel() = default;
    
    int getNumRows() override { return (int)solutions.size(); }
    
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, 
                          bool rowIsSelected) override {
        if (rowNumber >= 0 && rowNumber < solutions.size()) {
            auto& solution = solutions[rowNumber];
            
            // Dessiner la sélection
            if (rowIsSelected) {
                g.setColour(juce::Colour(0xFF3D75C2)); // Bleu de sélection
                g.fillRect(0, 0, width, height);
            }
            
            g.setColour(rowIsSelected ? juce::Colours::white : juce::Colours::lightgrey);
            g.setFont(14.0f);
            
            // Dessiner le nom et la date
            g.drawText(solution.getName(), 5, 2, width - 10, 16, juce::Justification::topLeft, true);
            g.setFont(12.0f);
            g.drawText(solution.getFormattedDate(), 5, 20, width - 10, 14, juce::Justification::topLeft, true);
        }
    }
    
    void selectedRowsChanged(int lastRowSelected) override {
        if (onSolutionSelected && lastRowSelected >= 0 && lastRowSelected < solutions.size()) {
            onSolutionSelected(solutions[lastRowSelected]);
        }
    }
    
    void setSolutions(const std::vector<SolutionHistoryItem>& newSolutions) {
        solutions = newSolutions;
    }
    
    std::function<void(const SolutionHistoryItem&)> onSolutionSelected;
    
private:
    std::vector<SolutionHistoryItem> solutions;
};

class SidebarPanel : public juce::Component {
public:
    SidebarPanel() {
        // Titre de la barre latérale
        titleLabel = std::make_unique<juce::Label>("", "Historique");

        titleLabel->setFont(juce::Font(juce::FontOptions(16.0f, juce::Font::bold)));
        titleLabel->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*titleLabel);
        
        // Liste des solutions
        solutionsList = std::make_unique<juce::ListBox>("Solutions", &listModel);
        solutionsList->setRowHeight(40);
        solutionsList->setColour(juce::ListBox::backgroundColourId, juce::Colour(0xFF2A2A2A));
        solutionsList->setOutlineThickness(1);
        addAndMakeVisible(*solutionsList);
        
        // Bouton d'actualisation
        refreshButton = std::make_unique<juce::TextButton>("Actualiser");
        refreshButton->onClick = [this]() {
            if (onRefreshRequested) {
                onRefreshRequested();
            }
        };
        addAndMakeVisible(*refreshButton);
        
        // Bouton pour charger la solution sélectionnée
        loadButton = std::make_unique<juce::TextButton>("Charger");
        loadButton->setEnabled(false);
        loadButton->onClick = [this]() {
            if (onLoadRequested && selectedSolution) {
                onLoadRequested(*selectedSolution);
            }
        };
        addAndMakeVisible(*loadButton);
        
        // Configuration du callback de la liste
        listModel.onSolutionSelected = [this](const SolutionHistoryItem& solution) {
            selectedSolution = std::make_unique<SolutionHistoryItem>(solution);
            loadButton->setEnabled(true);
            
            if (onSolutionSelected) {
                onSolutionSelected(solution);
            }
        };
        
        // Charger les solutions depuis la base de données
        loadSolutionsFromDb();
    }
    
    void paint(juce::Graphics& g) override {
        // Dessin du fond de la barre latérale
        auto area = getLocalBounds();
        g.setColour(juce::Colour(0xFF252525)); // Un peu plus foncé que le fond principal
        g.fillRect(area);
        
        // Bordure droite
        g.setColour(juce::Colour(0xFF444444));
        g.fillRect(area.getRight() - 1, 0, 1, area.getHeight());
    }
    
    void resized() override {
        auto area = getLocalBounds().reduced(10);
        
        // Placer le titre
        titleLabel->setBounds(area.removeFromTop(25));
        
        area.removeFromTop(10); // Espace après le titre
        
        // Réserver la zone des boutons
        auto buttonArea = area.removeFromBottom(40);
        
        // Ajouter un espace supplémentaire entre la liste et les boutons
        area.removeFromBottom(10);
        
        // Placer la liste
        solutionsList->setBounds(area);
        
        // Placer les boutons côte à côte en bas
        int buttonWidth = buttonArea.getWidth() / 2 - 5;
        refreshButton->setBounds(buttonArea.removeFromLeft(buttonWidth));
        buttonArea.removeFromLeft(10); // Espace entre les boutons
        loadButton->setBounds(buttonArea);
    }
    
    // Méthode pour charger les solutions depuis la base de données
    void loadSolutions(const std::vector<SolutionHistoryItem>& solutions) {
        listModel.setSolutions(solutions);
        solutionsList->updateContent();
        loadButton->setEnabled(false);
        selectedSolution.reset();
    }
    
    // Méthode pour charger les solutions depuis la base de données
    void loadSolutionsFromDb() {
        std::vector<SolutionHistoryItem> dbSolutions;
        
        // Récupérer les solutions de la base de données
        auto solutions = Db::getAllSolutions();
        
        int id = 1;
        // Convertir les solutions JSON en SolutionHistoryItem
        for (const auto& solution : solutions) {
            juce::String name = solution["name"].get<std::string>();
            juce::String path = solution["path"].get<std::string>();
            juce::int64 timestamp = solution["timestamp"].get<juce::int64>();
            
            dbSolutions.push_back(SolutionHistoryItem(id++, name, path, timestamp));
        }
        
        // Trier les solutions par date décroissante (plus récentes en premier)
        std::sort(dbSolutions.begin(), dbSolutions.end(), 
            [](const SolutionHistoryItem& a, const SolutionHistoryItem& b) {
                return a.getTimestamp() > b.getTimestamp();
            });
        
        loadSolutions(dbSolutions);
    }
    
    // Callbacks
    std::function<void()> onRefreshRequested;
    std::function<void(const SolutionHistoryItem&)> onLoadRequested;
    std::function<void(const SolutionHistoryItem&)> onSolutionSelected;
    
private:
    SolutionListModel listModel;
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::ListBox> solutionsList;
    std::unique_ptr<juce::TextButton> refreshButton;
    std::unique_ptr<juce::TextButton> loadButton;
    std::unique_ptr<SolutionHistoryItem> selectedSolution;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarPanel)
}; 