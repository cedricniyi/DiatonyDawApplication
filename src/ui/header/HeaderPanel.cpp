#include "HeaderPanel.h"
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"
#include "ui/UIStateIdentifiers.h"
#include "ui/PluginEditor.h"
#include "controller/AppController.h"
#include "utils/FontManager.h"
#include "utils/FileUtils.h"

//==============================================================================
HeaderPanel::HeaderPanel()
    : ColoredPanel (juce::Colour::fromString("#FF333333")),
      generateButton (juce::String::fromUTF8("Generate"),
                      juce::Colour::fromString ("#ff22c55e"), // Vert
                      juce::Colour::fromString ("#ff16a34a"), // Vert plus foncé au survol
                      14.0f, FontManager::FontWeight::Medium),
      dButton (juce::String::fromUTF8("D"),
               juce::Colour::fromString ("ff808080"), // Gris
               juce::Colour::fromString ("ff606060"), // Gris plus foncé au survol
               14.0f, FontManager::FontWeight::Black)
{
    // Configuration du label principal
    mainLabel.setText(juce::String::fromUTF8("DiatonyDAWPlugin"),juce::dontSendNotification);
    mainLabel.setJustificationType (juce::Justification::centredLeft);
    mainLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    
    // Application de la police via FontManager
    auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
    mainLabel.setFont(juce::Font(fontOptions));
   
    addAndMakeVisible (mainLabel);

    // Configuration du bouton Generate
    generateButton.setTooltip(juce::String::fromUTF8("Générer une solution musicale"));
    addAndMakeVisible (generateButton);

    // Configuration du bouton D - ouvre le dossier des solutions MIDI
    dButton.setTooltip("Ouvrir le dossier des solutions MIDI");
    dButton.onClick = []() {
        FileUtils::openMidiSolutionsFolder();
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
    
    // Synchronisation de l'état initial
    updateDockState();
}

void HeaderPanel::resized()
{
    auto bounds = getLocalBounds();
    
    // Calcul de la largeur exacte du texte pour définir la zone titre
    juce::GlyphArrangement ga;
    ga.addLineOfText(mainLabel.getFont(), mainLabel.getText(), 0, 0);
    auto labelWidth = static_cast<int>(std::ceil(ga.getBoundingBox(0, -1, false).getWidth() + 40));
    
    // La zone titre = largeur du label + padding
    titleZoneWidth = labelWidth + 20;
    
    // Zone titre (à gauche)
    auto titleArea = bounds.removeFromLeft(titleZoneWidth).reduced(20, 10);
    mainLabel.setBounds(titleArea);
    
    // Zone boutons (à droite) - avec padding
    auto buttonArea = bounds.reduced(20, 10);
    auto buttonSize = buttonArea.getHeight();
    
    juce::FlexBox buttonFlex;
    buttonFlex.flexDirection = juce::FlexBox::Direction::row;
    buttonFlex.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    buttonFlex.alignItems = juce::FlexBox::AlignItems::center;
    
    // Bouton Generate (rectangulaire, 80px de large)
    buttonFlex.items.add(juce::FlexItem(generateButton)
        .withMinWidth(80.0f)
        .withMinHeight(static_cast<float>(buttonSize))
        .withMargin(juce::FlexItem::Margin(0, 12, 0, 0)));
    
    // Bouton D (carré)
    buttonFlex.items.add(juce::FlexItem(dButton)
        .withMinWidth(static_cast<float>(buttonSize))
        .withMinHeight(static_cast<float>(buttonSize)));
    
    buttonFlex.performLayout(buttonArea);
}

void HeaderPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Zone du titre (partie gauche) - couleur un peu plus foncée
    auto titleZone = bounds.removeFromLeft(titleZoneWidth);
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRect(titleZone);
    
    // Zone des boutons (partie droite) - couleur légèrement plus claire
    g.setColour(juce::Colour(0xFF2A2A2A));
    g.fillRect(bounds);
    
    // Bordure en bas du header
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
    
    // Séparation verticale entre zone titre et zone boutons
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(titleZoneWidth, 0, 1, getHeight());
}

// === DÉCOUVERTE DE SERVICE ===

void HeaderPanel::parentHierarchyChanged()
{
    ColoredPanel::parentHierarchyChanged();
    findAppController();
}

void HeaderPanel::findAppController()
{
    // Recherche de l'AppController via la hiérarchie des composants
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
        DBG("HeaderPanel: AppController trouvé !");
        
        // Connecter le bouton de génération une fois AppController trouvé
        connectGenerateButton();
    }
    else
    {
        appController = nullptr;
        DBG("HeaderPanel: AppController NON trouvé");
    }
}

void HeaderPanel::connectGenerateButton()
{
    if (!appController)
    {
        DBG("HeaderPanel::connectGenerateButton() - Pas de contrôleur disponible");
        return;
    }
    
    // Connecter le callback du bouton Generate au contrôleur
    generateButton.onClick = [this]() {
        DBG("🎵 Bouton Generate cliqué ! Appel du contrôleur...");
        
        if (appController)
        {
            appController->startGeneration();
        }
        else
        {
            DBG("  ❌ Erreur : AppController non disponible");
        }
    };
    
    DBG("HeaderPanel: Bouton Generate connecté au contrôleur ✓");
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
    // Note: Cette méthode est conservée pour une éventuelle utilisation future
    // avec d'autres états UI via le ValueTree
}
