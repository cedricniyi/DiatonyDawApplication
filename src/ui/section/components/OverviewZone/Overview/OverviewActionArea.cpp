#include "OverviewActionArea.h"
#include "utils/FontManager.h"
#include "controller/AppController.h"
#include "ui/PluginEditor.h"

//==============================================================================
OverviewActionArea::OverviewActionArea() 
    : ColoredPanel(juce::Colour::fromString("#fffcfcff"))
{
    // Définir l'alpha pour que le composant en dessous soit visible
    setAlpha(0.85f); 
    
    // Ajouter les composants
    addAndMakeVisible(playbackActionArea);
    addAndMakeVisible(overviewArea);
    addAndMakeVisible(generationButtons);
    
    // Configuration du FlexBox
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    flexBox.alignItems = juce::FlexBox::AlignItems::center;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    
    // Ajouter les éléments au FlexBox
    // PlaybackActionArea à gauche (taille fixe)
    auto playbackSize = playbackActionArea.getPreferredSize();
    flexBox.items.add(juce::FlexItem(playbackActionArea)
        .withWidth(playbackSize.getWidth())
        .withHeight(playbackSize.getHeight())
        .withFlex(0, 0, playbackSize.getWidth())
        .withMargin(juce::FlexItem::Margin(0, 10, 0, 0)));
    
    // OverviewArea au centre (responsive en largeur)
    auto overviewSize = overviewArea.getPreferredSize();
    flexBox.items.add(juce::FlexItem(overviewArea)
        .withMinWidth(150.0f)
        .withHeight(overviewSize.getHeight())
        .withFlex(1, 1, 200.0f) // Flexible en largeur
        .withMargin(juce::FlexItem::Margin(0, 10, 0, 10)));
    
    // GenerationButtons à droite (taille fixe)
    auto generationSize = generationButtons.getPreferredSize();
    flexBox.items.add(juce::FlexItem(generationButtons)
        .withWidth(generationSize.getWidth())
        .withHeight(generationSize.getHeight())
        .withFlex(0, 0, generationSize.getWidth())
        .withMargin(juce::FlexItem::Margin(0, 0, 0, 10)));
}

void OverviewActionArea::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
}

void OverviewActionArea::resized()
{
    auto bounds = getLocalBounds();
    
    // Marges adaptatives basées sur la taille disponible (réduites pour plus d'espace)
    int horizontalMargin = juce::jmin(8, bounds.getWidth() / 18);
    int verticalMargin = juce::jmin(4, bounds.getHeight() / 25);
    
    auto area = bounds.reduced(horizontalMargin, verticalMargin);
    flexBox.performLayout(area.toFloat());
}

GenerationButtons& OverviewActionArea::getGenerationButtons()
{
    return generationButtons;
}

PlaybackActionArea& OverviewActionArea::getPlaybackActionArea()
{
    return playbackActionArea;
}

OverviewArea& OverviewActionArea::getOverviewArea()
{
    return overviewArea;
}

// === DÉCOUVERTE DE SERVICE ===

void OverviewActionArea::parentHierarchyChanged()
{
    ColoredPanel::parentHierarchyChanged();
    findAppController();
}

void OverviewActionArea::findAppController()
{
    // Recherche de l'AppController via la hiérarchie des composants
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
        DBG("OverviewActionArea: AppController trouvé !");
        
        // Connecter le bouton de génération une fois AppController trouvé
        connectGenerationButton();
    }
    else
    {
        appController = nullptr;
        DBG("OverviewActionArea: AppController NON trouvé");
    }
}

void OverviewActionArea::connectGenerationButton()
{
    if (!appController)
    {
        DBG("OverviewActionArea::connectGenerationButton() - Pas de contrôleur disponible");
        return;
    }
    
    // Connecter le callback du bouton Generate au contrôleur
    generationButtons.onGenerateClicked = [this]() {
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
    
    DBG("OverviewActionArea: Bouton Generate connecté au contrôleur ✓");
} 