#pragma once

#include <JuceHeader.h>
#include "ColoredPanel.h"
#include "ui/extra/Component/ComboBox/DiatonyComboBox.h"
#include "utils/FontManager.h"
#include "IconBinaryData.h"

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

/**
 * @brief Panel coloré avec 4 zones : carrés (numéro/cadenas/suppression) + 3 ComboBox (degré/qualité/état).
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
    
    DiatonyComboBox& getDegreeCombo() { return degreeCombo; }
    DiatonyComboBox& getQualityCombo() { return qualityCombo; }
    DiatonyComboBox& getStateCombo() { return stateCombo; }
    
    void populateDegreeCombo(const juce::StringArray& items);
    void populateQualityCombo(const juce::StringArray& items);
    void populateStateCombo(const juce::StringArray& items);
    
    void populateDegreeCombo(const juce::StringArray& items, const juce::StringArray& shortItems);
    void populateQualityCombo(const juce::StringArray& items, const juce::StringArray& shortItems);
    void populateStateCombo(const juce::StringArray& items, const juce::StringArray& shortItems);
    
    void setNumber(int number);
    int getNumber() const { return panelNumber; }
    
    void setLocked(bool locked);
    bool isLocked() const { return locked; }
    std::function<void(bool)> onLockToggled;
    
    std::function<void()> onDeleteRequested;
    
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    DiatonyComboBox degreeCombo;
    DiatonyComboBox qualityCombo;
    DiatonyComboBox stateCombo;
    
    juce::Label numberLabel;
    int panelNumber = 0;
    
    bool locked = false;
    juce::Rectangle<int> lockSquareArea;
    std::unique_ptr<juce::Drawable> lockIcon;
    std::unique_ptr<juce::Drawable> unlockIcon;
    
    juce::Rectangle<int> deleteSquareArea;
    static constexpr int LONG_PRESS_DURATION_MS = 2500;
    juce::uint32 deleteStartTime = 0;
    float deleteProgress = 0.0f;
    bool isDeleteHeldDown = false;
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    void setupLabels();
    void drawTopSquares(juce::Graphics& g, const juce::Rectangle<int>& topArea);
    void drawLockIcon(juce::Graphics& g, const juce::Rectangle<int>& area, bool isLocked);
    void drawDeleteIcon(juce::Graphics& g, const juce::Rectangle<int>& area);
    
    /** @brief Retourne la couleur de la bande selon la fonction tonale (Tonique/Sous-Dominante/Dominante). */
    juce::Colour getFunctionalStripColor() const;
    
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoColoredPanel)
};
