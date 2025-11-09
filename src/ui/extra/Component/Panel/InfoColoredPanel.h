#pragma once

#include <JuceHeader.h>
#include "ColoredPanel.h"
#include "ui/extra/Component/ComboBox/DiatonyComboBox.h"
#include "utils/FontManager.h"

//==============================================================================
// Panel coloré divisé en 2 zones verticales utilisant le système Grid de JUCE
// Zone haute (50% hauteur) : numérotation + ComboBox degré
// Zone basse (50% hauteur) : 2 ComboBox côte à côte (état 60% + qualité 40%)
class InfoColoredPanel : public ColoredPanel
{
public:
    InfoColoredPanel(juce::Colour color);
    ~InfoColoredPanel();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Override pour maintenir la compatibilité avec ColoredPanel
    void setColor(juce::Colour color) override;
    
    // Accès aux zones individuelles pour personnalisation
    juce::Component* getTopZone() const { return topZone.get(); }
    juce::Component* getBottomZone() const { return bottomZone.get(); }
    
    // Accès au contenu des zones
    DiatonyComboBox& getDegreeCombo() { return degreeCombo; }
    DiatonyComboBox& getStateCombo() { return stateCombo; }
    DiatonyComboBox& getQualityCombo() { return qualityCombo; }
    
    // Méthodes pour injecter les items dans les combobox (découplage du modèle métier)
    void populateDegreeCombo(const juce::StringArray& items);
    void populateStateCombo(const juce::StringArray& items);
    void populateQualityCombo(const juce::StringArray& items);
    
    // Méthodes legacy pour compatibilité
    void populateLeftCombo(const juce::StringArray& items) { populateDegreeCombo(items); }
    void populateTopRightCombo(const juce::StringArray& items) { populateStateCombo(items); }
    void populateBottomRightCombo(const juce::StringArray& items) { populateQualityCombo(items); }
    
    // Contrôle de la visibilité de la partie basse
    void showBottomZone(bool show);
    void hideBottomZone() { showBottomZone(false); }
    void showBottomZonePanel() { showBottomZone(true); }
    bool isBottomZoneVisible() const { return bottomZoneVisible; }
    
    // Méthodes legacy pour compatibilité
    void showRightSide(bool show) { showBottomZone(show); }
    void hideRightSide() { hideBottomZone(); }
    void showRightSidePanel() { showBottomZonePanel(); }
    bool isRightSideVisible() const { return isBottomZoneVisible(); }
    
    // Gestion de la numérotation
    void setNumber(int number);
    int getNumber() const { return panelNumber; }
    
private:
    // Les 2 zones principales
    std::unique_ptr<juce::Component> topZone;
    std::unique_ptr<juce::Component> bottomZone;
    
    // Conteneur pour les 2 ComboBox du bas
    std::unique_ptr<juce::Component> bottomLeftZone;
    std::unique_ptr<juce::Component> bottomRightZone;
    
    // Couleurs
    juce::Colour panelColor;
    
    // Contenu des zones - 3 ComboBox
    DiatonyComboBox degreeCombo;   // Zone haute (numéro + degré)
    DiatonyComboBox stateCombo;    // Zone basse gauche (état)
    DiatonyComboBox qualityCombo;  // Zone basse droite (qualité)
    
    // Système Grid pour la disposition
    juce::Grid mainGrid;        // Grid principal (haut/bas)
    juce::Grid bottomGrid;      // Grid pour la zone basse (2 ComboBox)
    
    // Contrôle de visibilité de la partie basse
    bool bottomZoneVisible = true;
    
    // Numérotation
    juce::Label numberLabel;
    int panelNumber = 0;
    
    // FontManager pour le label de numérotation
    juce::SharedResourcePointer<FontManager> fontManager;
    
    void setupMainGrid();
    void setupBottomGrid();
    void setupZones(juce::Colour baseColor);
    void reconfigureMainGrid();
    void setupNumberLabel();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoColoredPanel)
};