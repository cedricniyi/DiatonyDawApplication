#pragma once

#include <JuceHeader.h>
#include <map>
#include <string>
#include <functional>
#include "KeyboardLayout.h"
#include "layouts/QwertyLayout.h"
#include "layouts/AzertyLayout.h"

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
        // Par défaut, utiliser le layout QWERTY
        setKeyboardLayout(std::make_unique<QwertyLayout>());
        
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
    
    // Changer le layout du clavier
    void setKeyboardLayout(std::unique_ptr<KeyboardLayout> newLayout)
    {
        currentLayout = std::move(newLayout);
        
        // Réinitialiser la keyboardMap
        keyboardMap.clear();
        
        // Initialiser les touches à partir du layout
        const auto& keys = currentLayout->getKeys();
        for (const auto& keyInfo : keys) {
            keyboardMap[keyInfo.keyCode] = InteractiveKeyboard::KeyInfo(keyInfo);
        }
        
        // Recalculer les dimensions et redessiner
        calculateKeyDimensions();
        repaint();
    }
    
    // Basculer entre les layouts disponibles
    void toggleLayout()
    {
        if (currentLayout->getName() == "Qwerty") {
            setKeyboardLayout(std::make_unique<AzertyLayout>());
        } else {
            setKeyboardLayout(std::make_unique<QwertyLayout>());
        }
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
            keyInfo.label = keyInfo.layoutProperties.defaultLabel;
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
        KeyboardLayout::KeyInfo layoutProperties; // Composition: contient les propriétés du layout
        juce::Rectangle<float> bounds;            // Spécifique au dessin interactif
        juce::String label;                       // Étiquette actuelle (peut surcharger layoutProperties.defaultLabel)
        bool enabled = false;                     // État d'activation interactif
        bool isCurrentlyPressed = false;          // État de pression interactif

        // Constructeur pour initialiser à partir de KeyboardLayout::KeyInfo
        KeyInfo(const KeyboardLayout::KeyInfo& layoutProps)
            : layoutProperties(layoutProps),
              label(layoutProps.defaultLabel), // Le label initial est le defaultLabel du layout
              enabled(false),                  // Par défaut, non activée interactivement
              isCurrentlyPressed(false)
        {}

        // Définir explicitement le constructeur par défaut pour std::map
        KeyInfo()
            : layoutProperties(0, juce::String(), juce::String()) // Initialise layoutProperties avec des valeurs par défaut valides
        {
            // les autres membres (bounds, label, enabled, isCurrentlyPressed) 
            // sont soit default-constructibles, soit ont des initialiseurs par défaut en place.
        }
    };
    
    std::map<int, KeyInfo> keyboardMap;
    juce::String currentContext;
    std::unique_ptr<KeyboardLayout> currentLayout;
    
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
            if (keyboardMap.find(keyCode) != keyboardMap.end()) {
                keyboardMap[keyCode].bounds = juce::Rectangle<float>(x1, y1, keyWidth1 - margin, rowHeight - margin);
                x1 += keyWidth1;
            }
        }
        
        // Centrer la deuxième rangée
        float row2Start = bounds.getX() + (bounds.getWidth() - (keyWidth2 * row2.size())) / 2.0f;
        float x2 = row2Start;
        for (int keyCode : row2) {
            if (keyboardMap.find(keyCode) != keyboardMap.end()) {
                keyboardMap[keyCode].bounds = juce::Rectangle<float>(x2, y2, keyWidth2 - margin, rowHeight - margin);
                x2 += keyWidth2;
            }
        }
        
        // Décaler et centrer la troisième rangée (ASDFGHJKL)
        float row3Width = keyWidth3 * row3.size();
        float row3Start = bounds.getX() + (bounds.getWidth() - row3Width) / 2.0f;
        float x3 = row3Start;
        for (int keyCode : row3) {
            if (keyboardMap.find(keyCode) != keyboardMap.end()) {
                keyboardMap[keyCode].bounds = juce::Rectangle<float>(x3, y3, keyWidth3 - margin, rowHeight - margin);
                x3 += keyWidth3;
            }
        }
        
        // Décaler et centrer la quatrième rangée (ZXCVBNM)
        float row4Width = keyWidth4 * row4.size();
        float row4Start = bounds.getX() + (bounds.getWidth() - row4Width) / 2.0f;
        float x4 = row4Start;
        for (int keyCode : row4) {
            if (keyboardMap.find(keyCode) != keyboardMap.end()) {
                keyboardMap[keyCode].bounds = juce::Rectangle<float>(x4, y4, keyWidth4 - margin, rowHeight - margin);
                x4 += keyWidth4;
            }
        }
    }
    
    // Dessiner le clavier
    void drawKeyboard(juce::Graphics& g)
    {
        // Ajouter une étiquette pour le contexte actuel
        if (currentLayout) {
            juce::String contextInfo = currentLayout->getName();
            if (currentContext.isNotEmpty()) {
                contextInfo += " - Mode: " + currentContext;
            }
            
            g.setColour(juce::Colours::white.withAlpha(0.6f));
            g.setFont(14.0f); // Taille de police plus grande
            g.drawText(contextInfo, getLocalBounds().removeFromTop(20), 
                      juce::Justification::centred, false);
        }
        
        // Dessiner chaque touche
        for (const auto& [keyCode, keyInfo] : keyboardMap) {
            // Couleur de fond de la touche
            juce::Colour keyColourToDraw;
            if (keyInfo.isCurrentlyPressed) {
                keyColourToDraw = keyInfo.layoutProperties.pressedKeyColour;
            } else if (keyInfo.enabled) {
                keyColourToDraw = keyInfo.layoutProperties.activeKeyColour;
            } else {
                keyColourToDraw = keyInfo.layoutProperties.keyColour;
            }
            
            // Dessiner le fond de la touche
            g.setColour(keyColourToDraw);
            g.fillRoundedRectangle(keyInfo.bounds, 6.0f); // Rayon de coin plus grand
            
            // Dessiner le contour de la touche
            g.setColour(juce::Colours::white.withAlpha(0.5f));
            g.drawRoundedRectangle(keyInfo.bounds, 6.0f, 1.5f); // Contour plus épais
            
            // Dessiner l'étiquette de la touche (plus grande pour meilleure lisibilité)
            float fontSize = keyInfo.bounds.getHeight() * 0.5f; // 50% de la hauteur de la touche
            g.setFont(juce::Font(juce::FontOptions(fontSize,juce::Font::bold)));
            
            // Ajuster la couleur du texte en fonction de l'état d'activation
            if (keyInfo.enabled) {
                g.setColour(keyInfo.layoutProperties.activeTextColour);
            } else {
                g.setColour(keyInfo.layoutProperties.textColour);
            }
            
            g.drawText(keyInfo.label, keyInfo.bounds.reduced(3), juce::Justification::centred, true);
        }
    }
    
    // Gestion des événements clavier
    bool keyPressed(const juce::KeyPress& key) override
    {
        int keyCode = key.getKeyCode();
        
        // Gérer Ctrl+Tab pour changer de layout (Tab a le code 9)
        if (keyCode == 9 && key.getModifiers().isCtrlDown()) {
            toggleLayout();
            return true;
        }
        
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