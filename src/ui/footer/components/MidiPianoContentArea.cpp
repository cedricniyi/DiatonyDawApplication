#include "MidiPianoContentArea.h"
#include "ui/extra/Button/StyledButton.h"
#include "ui/extra/Component/TextPill.h"
#include "utils/FontManager.h"

//==============================================================================
MidiPianoContentArea::MidiPianoContentArea() 
    : resizeButton(juce::String::fromUTF8("Show/Hide"), 
                  juce::Colour(0xff4a90e2),  // Couleur normale (bleu)
                  juce::Colour(0xff357abd),  // Couleur highlight (bleu plus foncé)
                  14.0f,                     // Taille de police
                  FontManager::FontWeight::Medium),
      selectedSecPill(juce::Colour::fromString("#ff3498db").withAlpha(0.35f)),
      pianoModePill(juce::Colours::lightgreen),
      midiInfoPill(juce::Colours::lightyellow),
      moreInfoPill(juce::Colours::lightpink),
      isZone5Visible(false)
{
    // Configuration du bouton de redimensionnement
    resizeButton.onClick = [this] { 
        DBG("Bouton Show/Hide cliqué !"); // Debug
        
        // Basculer la visibilité de la zone 5
        isZone5Visible = !isZone5Visible;
        
        if (isZone5Visible) {
            addAndMakeVisible(moreInfoPill);
            DBG("Zone 5 affichée");
        } else {
            removeChildComponent(&moreInfoPill);
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
    
    addAndMakeVisible(resizeButton);
    addAndMakeVisible(selectedSecPill);
    addAndMakeVisible(pianoModePill);
    addAndMakeVisible(midiInfoPill);
    addChildComponent(moreInfoPill);
    
    // Configuration des TextPill avec styles variés
    setupTextPills();
}

void MidiPianoContentArea::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void MidiPianoContentArea::resized()
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
    
    // Première ligne : selectedSecPill et resizeButton
    juce::FlexBox topFlexBox;
    topFlexBox.flexDirection = juce::FlexBox::Direction::row;
    topFlexBox.items.add(juce::FlexItem(selectedSecPill).withFlex(topLeftPercentage / 100.0f).withMargin(5));
    topFlexBox.items.add(juce::FlexItem(resizeButton).withFlex(topRightPercentage / 100.0f).withMargin(5));
    topFlexBox.performLayout(topRow);
    
    // Deuxième ligne : pianoModePill et midiInfoPill
    juce::FlexBox bottomFlexBox;
    bottomFlexBox.flexDirection = juce::FlexBox::Direction::row;
    bottomFlexBox.items.add(juce::FlexItem(pianoModePill).withFlex(bottomLeftPercentage / 100.0f).withMargin(5));
    bottomFlexBox.items.add(juce::FlexItem(midiInfoPill).withFlex(bottomRightPercentage / 100.0f).withMargin(5));
    bottomFlexBox.performLayout(bottomRow);
    
    // Troisième ligne : moreInfoPill (si visible, prend toute la largeur avec margin de 5)
    if (isZone5Visible) {
        moreInfoPill.setBounds(zone5Area.reduced(5));
    }
}

void MidiPianoContentArea::calculateRowPercentages(const juce::Rectangle<int>& topRow, const juce::Rectangle<int>& bottomRow)
{
    // Pour l'instant, utilisation de pourcentages fixes
    // Ligne du haut : Zone 1 (45%) et resizeButton (55%)
    topLeftPercentage = 77.5f;
    topRightPercentage = 22.5f;
    
    // Ligne du bas : Zone 3 (65%) et Zone 4 (35%)
    bottomLeftPercentage = 65.0f;
    bottomRightPercentage = 35.0f;
    
    // Debug : afficher les pourcentages calculés
    // DBG("Ligne du haut - Zone 1: " << topLeftPercentage << "%, ResizeButton: " << topRightPercentage << "%");
    // DBG("Ligne du bas - Zone 3: " << bottomLeftPercentage << "%, Zone 4: " << bottomRightPercentage << "%");
}

void MidiPianoContentArea::setupTextPills()
{
    selectedSecPill.setText("Active Section : ", "None", 
                     14.0f, FontManager::FontWeight::Medium,
                     14.0f, FontManager::FontWeight::Bold);
    selectedSecPill.setTextColor(juce::Colours::lightblue.contrasting(0.8f));

    selectedSecPill.setBorder(juce::Colour::fromString("#ff3498db"), 1.5f);
    

    pianoModePill.setText("Mode:", "", 
                     14.0f, FontManager::FontWeight::Semibold,
                     14.0f, FontManager::FontWeight::Regular);
    pianoModePill.setTextColor(juce::Colours::lightgreen.contrasting(0.8f));
    pianoModePill.setBorder(juce::Colours::lightgreen.darker(0.4f), 1.0f);
    
    midiInfoPill.setText(juce::String::fromUTF8("● "), "Midi Active", 
                     14.0f, FontManager::FontWeight::Bold,
                     14.0f, FontManager::FontWeight::Semibold);
    midiInfoPill.setTextColor(juce::Colours::lightyellow.contrasting(0.8f));
    midiInfoPill.setBorder(juce::Colours::lightyellow.darker(0.5f), 2.0f);
    
    moreInfoPill.setText("Zone 5 ", "- Hidden Area", 
                     14.0f, FontManager::FontWeight::Bold,
                     14.0f, FontManager::FontWeight::Regular);
    moreInfoPill.setTextColor(juce::Colours::lightpink.contrasting(0.8f));
} 