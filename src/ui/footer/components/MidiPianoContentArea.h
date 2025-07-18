#pragma once

#include <JuceHeader.h>
#include "../../extra/StyledButton.h"
#include "../../../utils/FontManager.h"

//==============================================================================
// Composant simple coloré pour les zones
class SimpleColoredZone : public juce::Component
{
public:
    SimpleColoredZone(juce::Colour color, const juce::String& name) 
        : zoneColor(color), zoneName(name)
    {
        setOpaque(false);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Fond coloré avec coins arrondis
        auto bounds = getLocalBounds().toFloat();
        g.setColour(zoneColor);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Texte de debug au centre
        g.setColour(zoneColor.contrasting(0.8f));
        g.setFont(14.0f);
        g.drawText(zoneName, bounds.toNearestInt(), juce::Justification::centred);
    }
    
private:
    juce::Colour zoneColor;
    juce::String zoneName;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleColoredZone)
};

//==============================================================================
class MidiPianoContentArea : public juce::Component
{
public:
    MidiPianoContentArea() 
        : resizeButton(juce::String::fromUTF8("Show/Hide"), 
                      juce::Colour(0xff4a90e2),  // Couleur normale (bleu)
                      juce::Colour(0xff357abd),  // Couleur highlight (bleu plus foncé)
                      14.0f,                     // Taille de police
                      FontManager::FontWeight::Medium),
          zone1(juce::Colours::lightblue, "Zone 1"),
          zone2(juce::Colours::lightcoral, "Zone 2"),
          zone3(juce::Colours::lightgreen, "Zone 3"),
          zone4(juce::Colours::lightyellow, "Zone 4"),
          zone5(juce::Colours::lightpink, "Zone 5"),
          isZone5Visible(false)
    {
        // Configuration du bouton de redimensionnement
        resizeButton.onClick = [this] { 
            DBG("Bouton Show/Hide cliqué !"); // Debug
            
            // Basculer la visibilité de la zone 5
            isZone5Visible = !isZone5Visible;
            
            if (isZone5Visible) {
                addAndMakeVisible(zone5);
                DBG("Zone 5 affichée");
            } else {
                removeChildComponent(&zone5);
                DBG("Zone 5 masquée");
            }
            
            // Redéclencher le layout
            resized();
            
            // Appeler le callback si défini
            if (onResizeToggle) {
                DBG("Callback onResizeToggle trouvé, appel en cours...");
                onResizeToggle(); // Déclenche l'animation de redimensionnement
            }
        };
        
        // Le bouton remplace la zone 2 (en haut à droite)
        addAndMakeVisible(resizeButton);
        
        // Ajout des 3 zones colorées (zone2 remplacée par le bouton)
        addAndMakeVisible(zone1);
        addChildComponent(zone2); // Zone 2 masquée car remplacée par le bouton
        addAndMakeVisible(zone3);
        addAndMakeVisible(zone4);
        
        // Zone 5 initialement masquée
        addChildComponent(zone5);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Fond général
        g.fillAll(juce::Colours::white);
        
        // Bordure pour délimiter clairement le conteneur
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        
        // Si la zone 5 est visible, réserver de l'espace pour elle
        juce::Rectangle<int> zone5Area;
        if (isZone5Visible) {
            zone5Area = area.removeFromBottom(area.getHeight() / 3); // Zone 5 prend 1/3 de la hauteur
        }
        
        // Diviser l'aire restante en deux lignes égales
        auto topRow = area.removeFromTop(area.getHeight() / 2);
        auto bottomRow = area;
        
        // Calculer les pourcentages pour chaque ligne
        calculateRowPercentages(topRow, bottomRow);
        
        // Première ligne : Zone 1 et resizeButton (remplace zone 2)
        juce::FlexBox topFlexBox;
        topFlexBox.flexDirection = juce::FlexBox::Direction::row;
        topFlexBox.items.add(juce::FlexItem(zone1).withFlex(topLeftPercentage / 100.0f).withMargin(5));
        topFlexBox.items.add(juce::FlexItem(resizeButton).withFlex(topRightPercentage / 100.0f).withMargin(5));
        topFlexBox.performLayout(topRow);
        
        // Deuxième ligne : Zone 3 et Zone 4
        juce::FlexBox bottomFlexBox;
        bottomFlexBox.flexDirection = juce::FlexBox::Direction::row;
        bottomFlexBox.items.add(juce::FlexItem(zone3).withFlex(bottomLeftPercentage / 100.0f).withMargin(5));
        bottomFlexBox.items.add(juce::FlexItem(zone4).withFlex(bottomRightPercentage / 100.0f).withMargin(5));
        bottomFlexBox.performLayout(bottomRow);
        
        // Troisième ligne : Zone 5 (si visible, prend toute la largeur avec margin de 5)
        if (isZone5Visible) {
            zone5.setBounds(zone5Area.reduced(5));
        }
    }
    
    void calculateRowPercentages(const juce::Rectangle<int>& topRow, const juce::Rectangle<int>& bottomRow)
    {
        // Pour l'instant, utilisation de pourcentages fixes
        // Ligne du haut : Zone 1 (45%) et resizeButton (55%)
        topLeftPercentage = 77.5f;
        topRightPercentage = 22.5f;
        
        // Ligne du bas : Zone 3 (65%) et Zone 4 (35%)
        bottomLeftPercentage = 65.0f;
        bottomRightPercentage = 35.0f;
        
        // Debug : afficher les pourcentages calculés
        DBG("Ligne du haut - Zone 1: " << topLeftPercentage << "%, ResizeButton: " << topRightPercentage << "%");
        DBG("Ligne du bas - Zone 3: " << bottomLeftPercentage << "%, Zone 4: " << bottomRightPercentage << "%");
    }
    
    /** Callback déclenché quand le bouton de redimensionnement est cliqué */
    std::function<void()> onResizeToggle;
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Bouton de redimensionnement (masqué mais gardé dans le code)
    StyledButton resizeButton;
    
    // Les 5 zones colorées (maintenant des juce::Component simples)
    SimpleColoredZone zone1;
    SimpleColoredZone zone2;
    SimpleColoredZone zone3;
    SimpleColoredZone zone4;
    SimpleColoredZone zone5;
    
    // État de visibilité de la zone 5
    bool isZone5Visible;
    
    // Pourcentages pour chaque ligne
    float topLeftPercentage = 50.0f;
    float topRightPercentage = 50.0f;
    float bottomLeftPercentage = 50.0f;
    float bottomRightPercentage = 50.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPianoContentArea)
}; 