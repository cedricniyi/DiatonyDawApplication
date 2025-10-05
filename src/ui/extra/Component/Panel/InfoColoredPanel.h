#pragma once

#include <JuceHeader.h>
#include "ColoredPanel.h"
#include "ui/extra/Component/ComboBox/DiatonyComboBox.h"

//==============================================================================
// Panel coloré divisé en 3 zones utilisant le système Grid de JUCE
// Zone de gauche (50% largeur) + 2 zones à droite (50% largeur chacune, empilées verticalement)
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
    juce::Component* getLeftZone() const { return leftZone.get(); }
    juce::Component* getTopRightZone() const { return topRightZone.get(); }
    juce::Component* getBottomRightZone() const { return bottomRightZone.get(); }
    
    // Accès au contenu des zones
    DiatonyComboBox& getLeftCombo() { return leftCombo; }
    DiatonyComboBox& getTopRightCombo() { return topRightCombo; }
    DiatonyComboBox& getBottomRightCombo() { return bottomRightCombo; }
    
    // Contrôle de la visibilité de la partie droite
    void showRightSide(bool show);
    void hideRightSide() { showRightSide(false); }
    void showRightSidePanel() { showRightSide(true); }
    bool isRightSideVisible() const { return rightSideVisible; }
    
private:
    // Les 3 zones
    std::unique_ptr<juce::Component> leftZone;
    std::unique_ptr<juce::Component> topRightZone;
    std::unique_ptr<juce::Component> bottomRightZone;
    
    // Couleurs des zones
    juce::Colour leftZoneColor;
    juce::Colour topRightZoneColor;
    juce::Colour bottomRightZoneColor;
    
    // Contenu des zones - maintenant 3 ComboBox
    DiatonyComboBox leftCombo;
    DiatonyComboBox topRightCombo;
    DiatonyComboBox bottomRightCombo;
    
    // Système Grid pour la disposition
    juce::Grid grid;
    
    // Contrôle de visibilité de la partie droite
    bool rightSideVisible = true;
    
    void setupGrid();
    void setupZones(juce::Colour baseColor);
    void reconfigureGrid();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoColoredPanel)
};