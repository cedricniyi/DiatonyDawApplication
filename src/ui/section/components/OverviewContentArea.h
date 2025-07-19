#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "utils/FontManager.h"
#include "utils/ColorGenerator.h"
#include "ScrollableContentPanel.h"

//==============================================================================
/**
 * Zone de contenu d'aperçu gérant l'affichage de progressions avec gestion d'état vide
 */
class OverviewContentArea : public ColoredPanel
{
public:
    OverviewContentArea() 
        : ColoredPanel(juce::Colour::fromString("#ffe3e3ff")),
          scrollableContent(std::make_unique<ScrollableContentPanel>())
    {
        // Configuration de base
        setAlpha(1.0f);
        
        // Configuration des composants
        setupViewport();
        setupEmptyLabel();
        
        // Mettre à jour la visibilité initiale
        updateVisibility();
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        ColoredPanel::resized();
        auto contentArea = getLocalBounds().reduced(CONTENT_MARGIN);
        
        // Configurer les bounds des composants
        viewport.setBounds(contentArea);
        emptyLabel.setBounds(contentArea);
        
        // Configurer le contenu scrollable
        if (scrollableContent)
        {
            scrollableContent->setSize(scrollableContent->getWidth(), PREFERRED_HEIGHT);
        }
    }
    
    // Gestion du contenu
    void addSmallPanel()
    {
        if (!scrollableContent)
            return;
        
        // Créer un nouveau petit ColoredPanel avec une couleur aléatoire
        auto newPanel = std::make_unique<ColoredPanel>(ColorGenerator::generateRandomColor());
        
        // L'ajouter au contenu scrollable
        scrollableContent->addSmallPanel(std::move(newPanel));
        
        // Mettre à jour la visibilité
        updateVisibility();
        
        // Forcer le redimensionnement pour s'assurer du layout
        resized();
    }
    
    void clearAllPanels()
    {
        if (scrollableContent)
        {
            scrollableContent->clearAllPanels();
            updateVisibility();
            resized();
        }
    }
    
    bool hasContent() const
    {
        return scrollableContent && scrollableContent->getNumPanels() > 0;
    }
    
    // Configuration
    juce::Rectangle<int> getPreferredSize() const
    {
        return juce::Rectangle<int>(0, 0, PREFERRED_WIDTH, PREFERRED_HEIGHT);
    }

private:
    // Composants UI
    juce::Viewport viewport;
    std::unique_ptr<ScrollableContentPanel> scrollableContent;
    juce::Label emptyLabel;
    
    // Configuration
    static constexpr int PREFERRED_WIDTH = 300;
    static constexpr int PREFERRED_HEIGHT = 35;
    static constexpr int CONTENT_MARGIN = 5;
    
    // Méthodes privées
    void setupViewport()
    {
        // Donner une taille initiale au contenu scrollable
        scrollableContent->setSize(100, PREFERRED_HEIGHT);
        
        // Configuration du viewport
        viewport.setViewedComponent(scrollableContent.get(), false); // false = on gère nous-mêmes la lifetime
        
        // Configurer les scrollbars : seulement horizontale, et seulement quand nécessaire
        viewport.setScrollBarsShown(false, true, false, false);
        
        // Positionner la scrollbar en bas
        viewport.setScrollBarPosition(true, true);
        
        // Ajouter le viewport à notre interface
        addAndMakeVisible(viewport);
    }
    
    void setupEmptyLabel()
    {
        // Configuration du label pour le texte par défaut
        emptyLabel.setText(juce::String::fromUTF8("Aucun progression n'a été ajouté à la pièce"), juce::dontSendNotification);
        emptyLabel.setJustificationType(juce::Justification::centred);
        emptyLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withAlpha(0.7f));
        
        // Utiliser FontManager pour la font
        juce::SharedResourcePointer<FontManager> fontManager;
        auto fontOptions = fontManager->getSFProText(18.0f, FontManager::FontWeight::Regular);
        emptyLabel.setFont(juce::Font(fontOptions));
        
        // Ajouter le label
        addAndMakeVisible(emptyLabel);
    }
    
    void updateVisibility()
    {
        bool hasContentNow = hasContent();
        
        // Debug temporaire
        DBG("updateVisibility: hasContent=" + juce::String(hasContentNow ? "true" : "false"));
        DBG("  - numPanels=" + juce::String(scrollableContent ? scrollableContent->getNumPanels() : -1));
        
        viewport.setVisible(hasContentNow);
        emptyLabel.setVisible(!hasContentNow);
        
        // Debug temporaire
        DBG("  - viewport visible=" + juce::String(viewport.isVisible() ? "true" : "false"));
        DBG("  - label visible=" + juce::String(emptyLabel.isVisible() ? "true" : "false"));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewContentArea)
}; 