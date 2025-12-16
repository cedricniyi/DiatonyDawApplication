#pragma once

#include <JuceHeader.h>
#include "ColoredPanel.h"
#include "ui/extra/Component/ComboBox/DiatonyComboBox.h"
#include "utils/FontManager.h"
#include "IconBinaryData.h"

// ═══════════════════════════════════════════════════════════════════════════════
// 🎛️ PARAMÈTRES D'AFFICHAGE - Modifie ces valeurs !
// ═══════════════════════════════════════════════════════════════════════════════

namespace InfoPanelConfig
{
    // Bande de couleur harmonique en haut
    constexpr float STRIP_HEIGHT = 8.0f;          // ← Hauteur de la bande colorée
    
    // Zone des 3 petits carrés
    constexpr int TOP_ROW_HEIGHT = 24;            // ← Hauteur totale de la zone des carrés
    constexpr int TOP_PADDING = 0;                // ← Espacement entre bande et carrés
    constexpr int SQUARE_SIZE = 18;               // ← Taille des carrés
    constexpr int SQUARE_SPACING = 4;             // ← Espacement entre les carrés
    
    // ComboBox
    constexpr int COMBO_HEIGHT = 20;              // ← Hauteur des ComboBox
    constexpr int HORIZONTAL_PADDING = 6;         // ← Padding horizontal des ComboBox
    
    // Coins arrondis
    constexpr float CORNER_RADIUS = 8.0f;         // ← Rayon des coins du panel
}

// ═══════════════════════════════════════════════════════════════════════════════

//==============================================================================
/**
 * Panel coloré avec 4 zones empilées verticalement :
 * - Ligne de 3 petits carrés (numéro + 2 indicateurs)
 * - Degré ComboBox
 * - Qualité ComboBox
 * - État ComboBox
 */
class InfoColoredPanel : public ColoredPanel, 
                         private juce::ComboBox::Listener,
                         private juce::Timer
{
public:
    InfoColoredPanel(juce::Colour color);
    ~InfoColoredPanel();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setColor(juce::Colour color) override;
    
    // Accès aux ComboBox
    DiatonyComboBox& getDegreeCombo() { return degreeCombo; }
    DiatonyComboBox& getQualityCombo() { return qualityCombo; }
    DiatonyComboBox& getStateCombo() { return stateCombo; }
    
    // Méthodes pour peupler les ComboBox
    void populateDegreeCombo(const juce::StringArray& items);
    void populateQualityCombo(const juce::StringArray& items);
    void populateStateCombo(const juce::StringArray& items);
    
    // Surcharges avec textes courts pour affichage compact
    void populateDegreeCombo(const juce::StringArray& items, const juce::StringArray& shortItems);
    void populateQualityCombo(const juce::StringArray& items, const juce::StringArray& shortItems);
    void populateStateCombo(const juce::StringArray& items, const juce::StringArray& shortItems);
    
    // Gestion de la numérotation
    void setNumber(int number);
    int getNumber() const { return panelNumber; }
    
    // Carré 2 : Cadenas (verrouillage)
    void setLocked(bool locked);
    bool isLocked() const { return locked; }
    std::function<void(bool)> onLockToggled;  // Callback quand le cadenas est cliqué
    
    // Carré 3 : Suppression (long press)
    std::function<void()> onDeleteRequested;  // Callback pour suppression
    
    // Interaction souris
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    // 3 ComboBox empilés verticalement
    DiatonyComboBox degreeCombo;
    DiatonyComboBox qualityCombo;
    DiatonyComboBox stateCombo;
    
    // Numérotation (carré 1)
    juce::Label numberLabel;
    int panelNumber = 0;
    
    // Cadenas (carré 2)
    bool locked = false;
    juce::Rectangle<int> lockSquareArea;
    std::unique_ptr<juce::Drawable> lockIcon;
    std::unique_ptr<juce::Drawable> unlockIcon;
    
    // Suppression (carré 3) - Long press avec feedback visuel
    juce::Rectangle<int> deleteSquareArea;
    static constexpr int LONG_PRESS_DURATION_MS = 2500;  // 2.5 secondes
    juce::uint32 deleteStartTime = 0;
    float deleteProgress = 0.0f;  // 0 → 1
    bool isDeleteHeldDown = false;
    
    // FontManager
    juce::SharedResourcePointer<FontManager> fontManager;
    
    void setupLabels();
    void drawTopSquares(juce::Graphics& g, const juce::Rectangle<int>& topArea);
    void drawLockIcon(juce::Graphics& g, const juce::Rectangle<int>& area, bool isLocked);
    void drawDeleteIcon(juce::Graphics& g, const juce::Rectangle<int>& area);
    
    // Bande de codage couleur fonctionnelle (Tonique/Sous-Dominante/Dominante)
    juce::Colour getFunctionalStripColor() const;
    
    // Listener pour redessiner la bande quand le degré change
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    
    // Timer pour le long press progressif
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoColoredPanel)
};
