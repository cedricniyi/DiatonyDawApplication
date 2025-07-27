#include "HeaderPanel.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"
#include "ui/UIStateIdentifiers.h"
#include "utils/FontManager.h"

//==============================================================================
HeaderPanel::HeaderPanel()
    : ColoredPanel (juce::Colours::white),
      dButton (juce::String::fromUTF8("D"),
               juce::Colour::fromString ("ff808080"), // Gris
               juce::Colour::fromString ("ff606060"), // Gris plus foncé au survol
               14.0f, FontManager::FontWeight::Black)
{
    // Label gauche
    mainLabel.setText(juce::String::fromUTF8("DiatonyDAWPlugin"),juce::dontSendNotification);
    mainLabel.setJustificationType (juce::Justification::centredLeft);
    mainLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    
    // Correction : Convertir FontOptions en Font
    auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
    mainLabel.setFont(juce::Font(fontOptions));
   
    addAndMakeVisible (mainLabel);

    // Bouton D à droite - toggle direct du ValueTree
    dButton.onClick = [this]() {
        if (!appState.isValid()) {
            DBG("HeaderPanel: ERREUR: ValueTree non initialisé !");
            return;
        }
        
        bool currentState = static_cast<bool>(appState.getProperty(UIStateIdentifiers::dockVisible, false));
        appState.setProperty(UIStateIdentifiers::dockVisible, !currentState, nullptr);
        DBG("HeaderPanel: Toggle dock -> " << (!currentState ? "visible" : "caché"));
    };
    addAndMakeVisible (dButton);
}

HeaderPanel::~HeaderPanel()
{
    if (appState.isValid())
        appState.removeListener(this);
}

void HeaderPanel::setAppState(juce::ValueTree& state)
{
    if (appState.isValid())
        appState.removeListener(this);
        
    appState = state;
    appState.addListener(this);
    
    // Synchroniser l'état initial
    updateDockState();
    DBG("HeaderPanel: ValueTree connecté et état initial synchronisé");
}

void HeaderPanel::resized()
{
    auto area = getLocalBounds().reduced (20, 10);

    // 1) Mesurer la largeur exacte du texte avec GlyphArrangement
    juce::GlyphArrangement ga;
    ga.addLineOfText (mainLabel.getFont(),
                      mainLabel.getText(),
                      0, 0);
    auto labelWidth = static_cast<int>(std::ceil(ga.getBoundingBox(0, -1, false).getWidth()+10));

    // 2) Positionner le label à gauche avec sa largeur exacte
    mainLabel.setBounds(area.removeFromLeft(labelWidth));

    // 3) Positionner le bouton D à droite avec FlexBox (bouton carré)
    auto buttonSize = area.getHeight(); // Bouton carré basé sur la hauteur
    juce::FlexBox buttonFlex;
    buttonFlex.flexDirection = juce::FlexBox::Direction::row;
    buttonFlex.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    buttonFlex.alignItems = juce::FlexBox::AlignItems::center;
    buttonFlex.items.add(juce::FlexItem(dButton).withMinWidth(buttonSize).withMinHeight(buttonSize));
    
    buttonFlex.performLayout(area);
}

void HeaderPanel::paint (juce::Graphics& g)
{
    // Fond blanc via ColoredPanel
    ColoredPanel::paint (g);
}

void HeaderPanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                          const juce::Identifier& property)
{
    if (property == UIStateIdentifiers::dockVisible)
    {
        updateDockState();
    }
}

void HeaderPanel::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void HeaderPanel::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void HeaderPanel::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void HeaderPanel::valueTreeParentChanged(juce::ValueTree&) {}

void HeaderPanel::updateDockState()
{
    if (!appState.isValid()) return;
    
    bool dockVisible = static_cast<bool>(appState.getProperty(UIStateIdentifiers::dockVisible, false));
    
    // TODO: Quand le composant dock sera implémenté, ajouter la logique ici
    // Par exemple: mettre à jour la couleur du bouton D selon l'état
    DBG("HeaderPanel: État dock mis à jour -> " << (dockVisible ? "visible" : "caché"));
} 