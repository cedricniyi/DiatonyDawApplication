// Created by Cédric Niyikiza
//

#pragma once

#include <juce_core/juce_core.h>
#include <nlohmann/json.hpp>

#ifndef APPLICATION_SUPPORT_PATH
    #error "APPLICATION_SUPPORT_PATH n'est pas défini"
#endif

namespace Db {
    
    /**
     * Obtient le chemin du fichier de base de données JSON
     * @return Chemin complet vers le fichier JSON de la base de données
     */
    inline juce::File getSolutionsDbPath() {
        // Chemin vers le dossier Solutions dans Application Support
        juce::File solutionsDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
                          .getChildFile(APPLICATION_SUPPORT_PATH)
                          .getChildFile("DiatonyDawApplication")
                          .getChildFile("Solutions");
        
        // Créer l'arborescence de dossiers si nécessaire
        if (!solutionsDir.exists()) {
            solutionsDir.createDirectory();  // createDirectory() crée aussi les dossiers parents si nécessaire
        }
        
        return solutionsDir.getChildFile("solutions_db.json");
    }
    
    /**
     * Charge la base de données JSON des solutions
     * @return Un objet JSON contenant la liste des solutions
     */
    inline nlohmann::json loadSolutionsDb() {
        juce::File dbFile = getSolutionsDbPath();
        
        // Si le fichier n'existe pas, créer une structure JSON vide
        if (!dbFile.existsAsFile()) {
            nlohmann::json emptyDb;
            emptyDb["solutions"] = nlohmann::json::array();
            return emptyDb;
        }
        
        // Charger le contenu du fichier
        juce::String jsonContent = dbFile.loadFileAsString();
        
        try {
            // Parser le contenu JSON
            return nlohmann::json::parse(jsonContent.toStdString());
        } catch (const std::exception& e) {
            // En cas d'erreur, retourner une structure JSON vide
            nlohmann::json emptyDb;
            emptyDb["solutions"] = nlohmann::json::array();
            return emptyDb;
        }
    }
    
    /**
     * Sauvegarde la base de données JSON des solutions
     * @param db L'objet JSON à sauvegarder
     * @return true si la sauvegarde a réussi, false sinon
     */
    inline bool saveSolutionsDb(const nlohmann::json& db) {
        juce::File dbFile = getSolutionsDbPath();
        
        // Convertir l'objet JSON en string
        std::string jsonStr = db.dump(4); // 4 espaces d'indentation pour lisibilité
        
        // Sauvegarder dans le fichier
        return dbFile.replaceWithText(jsonStr);
    }
    
    /**
     * Ajoute une solution à la base de données JSON
     * @param name Nom du fichier de la solution
     * @param path Chemin complet vers le fichier de la solution
     * @return true si l'ajout a réussi, false sinon
     */
    inline bool addSolution(const juce::String& name, const juce::String& path) {
        // Charger la base de données
        nlohmann::json db = loadSolutionsDb();
        
        // Créer l'entrée pour la nouvelle solution
        nlohmann::json newSolution;
        newSolution["name"] = name.toStdString();
        newSolution["path"] = path.toStdString();
        newSolution["timestamp"] = juce::Time::getCurrentTime().toMilliseconds();
        
        // Ajouter la solution à la liste
        db["solutions"].push_back(newSolution);
        
        // Sauvegarder la base de données mise à jour
        return saveSolutionsDb(db);
    }
    
    /**
     * Récupère toutes les solutions sauvegardées
     * @return Un tableau JSON contenant toutes les solutions
     */
    inline nlohmann::json getAllSolutions() {
        return loadSolutionsDb()["solutions"];
    }
} 