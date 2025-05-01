#pragma once

#include <JuceHeader.h>
#include <map>
#include <string>
#include <functional>

//==============================================================================
/**
 * Composant affichant un clavier QWERTY interactif
 * Permet de visualiser les touches disponibles et leur fonction actuellement
 */
class InteractiveKeyboard : public juce::Component
{
public:
    InteractiveKeyboard()
    {
        // Configuration des touches QWERTY standard
        setupStandardKeyboard();
        
        // Activer les événements clavier
        setWantsKeyboardFocus(true);
    }

    ~InteractiveKeyboard() override = default;

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        // Couleur de fond légèrement plus sombre que le fond général
        g.fillAll(juce::Colour(0xFF232323));
        
        // Dessiner un cadre subtil
        g.setColour(juce::Colours::grey.withAlpha(0.5f));
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 3.0f, 1.0f);
        
        // Dessiner les touches
        drawKeyboard(g);
    }

    void resized() override
    {
        // Calculer les dimensions des touches en fonction de la taille du composant
        calculateKeyDimensions();
    }
    
    // Activer/désactiver des touches spécifiques avec une fonction personnalisée et étiquette
    void setKeyEnabled(juce::KeyPress key, bool enabled, const juce::String& label = {})
    {
        // Vérifier si la touche existe dans notre map
        int keyCode = key.getKeyCode();
        if (keyboardMap.find(keyCode) != keyboardMap.end()) {
            auto& keyInfo = keyboardMap[keyCode];
            keyInfo.enabled = enabled;
            
            // Mettre à jour l'étiquette si fournie
            if (label.isNotEmpty()) {
                keyInfo.label = label;
            }
            
            repaint();
        }
    }
    
    // Définir un contexte qui active/désactive des touches spécifiques
    void setContext(const juce::String& contextName, 
                   const std::map<int, juce::String>& activeKeys)
    {
        // Réinitialiser toutes les touches
        for (auto& [keyCode, keyInfo] : keyboardMap) {
            keyInfo.enabled = false;
            keyInfo.label = keyInfo.defaultLabel;
        }
        
        // Activer les touches spécifiées avec leurs étiquettes
        for (const auto& [keyCode, label] : activeKeys) {
            if (keyboardMap.find(keyCode) != keyboardMap.end()) {
                keyboardMap[keyCode].enabled = true;
                if (label.isNotEmpty()) {
                    keyboardMap[keyCode].label = label;
                }
            }
        }
        
        currentContext = contextName;
        repaint();
    }
    
    // Fonction à appeler lors d'un appui sur une touche
    std::function<void(int keyCode)> onKeyPressed;

private:
    struct KeyInfo {
        juce::Rectangle<float> bounds;
        juce::String defaultLabel;
        juce::String label;
        bool enabled = false;
        bool isCurrentlyPressed = false;
    };
    
    std::map<int, KeyInfo> keyboardMap;
    juce::String currentContext;
    
    // Configuration du clavier QWERTY standard
    void setupStandardKeyboard()
    {
        // Première rangée (1-0)
        std::vector<std::pair<int, juce::String>> row1 = {
            {49, "1"}, {50, "2"}, {51, "3"}, {52, "4"}, {53, "5"}, 
            {54, "6"}, {55, "7"}, {56, "8"}, {57, "9"}, {48, "0"}
        };
        
        // Deuxième rangée (QWERTYUIOP)
        std::vector<std::pair<int, juce::String>> row2 = {
            {81, "Q"}, {87, "W"}, {69, "E"}, {82, "R"}, {84, "T"}, 
            {89, "Y"}, {85, "U"}, {73, "I"}, {79, "O"}, {80, "P"}
        };
        
        // Troisième rangée (ASDFGHJKL)
        std::vector<std::pair<int, juce::String>> row3 = {
            {65, "A"}, {83, "S"}, {68, "D"}, {70, "F"}, {71, "G"}, 
            {72, "H"}, {74, "J"}, {75, "K"}, {76, "L"}
        };
        
        // Quatrième rangée (ZXCVBNM)
        std::vector<std::pair<int, juce::String>> row4 = {
            {90, "Z"}, {88, "X"}, {67, "C"}, {86, "V"}, {66, "B"}, 
            {78, "N"}, {77, "M"}
        };
        
        // Initialiser chaque touche
        for (const auto& row : {row1, row2, row3, row4}) {
            for (const auto& [keyCode, label] : row) {
                KeyInfo info;
                info.defaultLabel = label;
                info.label = label;
                info.enabled = false;
                keyboardMap[keyCode] = info;
            }
        }
    }
    
    // Calculer les dimensions des touches
    void calculateKeyDimensions()
    {
        // Utiliser plus de l'espace disponible pour les touches
        auto bounds = getLocalBounds().reduced(8);
        
        // Réserver une petite zone en haut pour le contexte
        int contextHeight = 20;
        bounds.removeFromTop(contextHeight);
        
        // Hauteur de chaque rangée (4 rangées au total avec des espaces entre elles)
        float totalRowsSpace = bounds.getHeight() * 0.9f; // Utiliser 90% de la hauteur disponible
        float gapBetweenRows = bounds.getHeight() * 0.02f; // 2% de la hauteur en espacement
        float rowHeight = (totalRowsSpace - (3 * gapBetweenRows)) / 4.0f;
        
        // Première rangée (1-0)
        std::vector<int> row1 = {49, 50, 51, 52, 53, 54, 55, 56, 57, 48};
        float totalWidth = bounds.getWidth() * 0.95f; // Utiliser 95% de la largeur
        float keyWidth1 = totalWidth / static_cast<float>(row1.size());
        float y1 = bounds.getY();
        
        // Deuxième rangée (QWERTYUIOP)
        std::vector<int> row2 = {81, 87, 69, 82, 84, 89, 85, 73, 79, 80};
        float keyWidth2 = totalWidth / static_cast<float>(row2.size());
        float y2 = y1 + rowHeight + gapBetweenRows;
        
        // Troisième rangée (ASDFGHJKL)
        std::vector<int> row3 = {65, 83, 68, 70, 71, 72, 74, 75, 76};
        float keyWidth3 = totalWidth / static_cast<float>(row2.size()); // Même largeur que la rangée 2
        float y3 = y2 + rowHeight + gapBetweenRows;
        
        // Quatrième rangée (ZXCVBNM)
        std::vector<int> row4 = {90, 88, 67, 86, 66, 78, 77};
        float keyWidth4 = keyWidth3; // Même largeur de touches que la rangée 3
        float y4 = y3 + rowHeight + gapBetweenRows;
        
        // Assigner les bounds à chaque touche avec des espaces entre elles
        float margin = 3.0f; // Marge entre les touches
        
        // Centrer la première rangée
        float row1Start = bounds.getX() + (bounds.getWidth() - (keyWidth1 * row1.size())) / 2.0f;
        float x1 = row1Start;
        for (int keyCode : row1) {
            keyboardMap[keyCode].bounds = juce::Rectangle<float>(x1, y1, keyWidth1 - margin, rowHeight - margin);
            x1 += keyWidth1;
        }
        
        // Centrer la deuxième rangée
        float row2Start = bounds.getX() + (bounds.getWidth() - (keyWidth2 * row2.size())) / 2.0f;
        float x2 = row2Start;
        for (int keyCode : row2) {
            keyboardMap[keyCode].bounds = juce::Rectangle<float>(x2, y2, keyWidth2 - margin, rowHeight - margin);
            x2 += keyWidth2;
        }
        
        // Décaler et centrer la troisième rangée (ASDFGHJKL)
        float row3Width = keyWidth3 * row3.size();
        float row3Start = bounds.getX() + (bounds.getWidth() - row3Width) / 2.0f;
        float x3 = row3Start;
        for (int keyCode : row3) {
            keyboardMap[keyCode].bounds = juce::Rectangle<float>(x3, y3, keyWidth3 - margin, rowHeight - margin);
            x3 += keyWidth3;
        }
        
        // Décaler et centrer la quatrième rangée (ZXCVBNM)
        float row4Width = keyWidth4 * row4.size();
        float row4Start = bounds.getX() + (bounds.getWidth() - row4Width) / 2.0f;
        float x4 = row4Start;
        for (int keyCode : row4) {
            keyboardMap[keyCode].bounds = juce::Rectangle<float>(x4, y4, keyWidth4 - margin, rowHeight - margin);
            x4 += keyWidth4;
        }
    }
    
    // Dessiner le clavier
    void drawKeyboard(juce::Graphics& g)
    {
        // Ajouter une étiquette pour le contexte actuel
        if (currentContext.isNotEmpty()) {
            g.setColour(juce::Colours::white.withAlpha(0.6f));
            g.setFont(14.0f); // Taille de police plus grande
            g.drawText("Mode: " + currentContext, getLocalBounds().removeFromTop(20), 
                       juce::Justification::centred, false);
        }
        
        // Dessiner chaque touche
        for (const auto& [keyCode, keyInfo] : keyboardMap) {
            // Couleur de fond de la touche
            juce::Colour keyColour;
            
            if (keyInfo.isCurrentlyPressed) {
                keyColour = juce::Colours::blue.withAlpha(0.7f);
            } else if (keyInfo.enabled) {
                keyColour = juce::Colours::orange.withAlpha(0.5f);
            } else {
                keyColour = juce::Colours::grey.withAlpha(0.3f);
            }
            
            // Dessiner le fond de la touche
            g.setColour(keyColour);
            g.fillRoundedRectangle(keyInfo.bounds, 6.0f); // Rayon de coin plus grand
            
            // Dessiner le contour de la touche
            g.setColour(juce::Colours::white.withAlpha(0.5f));
            g.drawRoundedRectangle(keyInfo.bounds, 6.0f, 1.5f); // Contour plus épais
            
            // Dessiner l'étiquette de la touche (plus grande pour meilleure lisibilité)
            float fontSize = keyInfo.bounds.getHeight() * 0.5f; // 50% de la hauteur de la touche
            g.setFont(juce::Font(fontSize).withStyle(juce::Font::bold));
            
            // Ajuster la couleur du texte en fonction de l'état d'activation
            if (keyInfo.enabled) {
                g.setColour(juce::Colours::white);
            } else {
                g.setColour(juce::Colours::white.withAlpha(0.7f));
            }
            
            g.drawText(keyInfo.label, keyInfo.bounds.reduced(3), juce::Justification::centred, true);
        }
    }
    
    // Gestion des événements clavier
    bool keyPressed(const juce::KeyPress& key) override
    {
        int keyCode = key.getKeyCode();
        
        if (keyboardMap.find(keyCode) != keyboardMap.end()) {
            keyboardMap[keyCode].isCurrentlyPressed = true;
            repaint();
            
            // Appeler le callback si la touche est activée et que le callback est défini
            if (keyboardMap[keyCode].enabled && onKeyPressed) {
                onKeyPressed(keyCode);
            }
            
            // On capture tous les événements clavier de notre map, même s'ils ne sont pas activés
            return true;
        }
        
        return false;
    }
    
    bool keyStateChanged(bool isKeyDown) override
    {
        // Réinitialiser l'état "pressed" pour toutes les touches si relâchées
        if (!isKeyDown) {
            bool needsRepaint = false;
            
            for (auto& [keyCode, keyInfo] : keyboardMap) {
                if (keyInfo.isCurrentlyPressed) {
                    keyInfo.isCurrentlyPressed = false;
                    needsRepaint = true;
                }
            }
            
            if (needsRepaint) {
                repaint();
            }
        }
        
        return false;
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractiveKeyboard)
}; 