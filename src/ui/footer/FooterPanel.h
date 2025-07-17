#pragma once

#include <JuceHeader.h>
#include "components/MidiPianoArea.h"
#include "../extra/ColoredPanel.h"
#include "../animation/AnimationManager.h"
#include "../../utils/FontManager.h"

//==============================================================================
class FooterPanel : public juce::Component
{
public:
    FooterPanel() 
        : midiPianoArea(),
          fadingDemoPanel(juce::Colour(0xff6c5ce7)), // Couleur violette
          isFadingComponentVisible(false),
          isGridExpanded(false), // État initial : grille compacte
          gridTransitionFraction(0.0f) // Paramètre d'interpolation (0 = compact, 1 = élargi)
    {
        // Configurer le callback pour le bouton de redimensionnement
        midiPianoArea.onResizeToggle = [this] { 
            DBG("FooterPanel: Callback onResizeToggle reçu !"); // Debug
            if (onRequestResize) {
                DBG("FooterPanel: Callback onRequestResize trouvé, appel en cours...");
                onRequestResize(); // Déclenche l'animation de redimensionnement du footer
            } else {
                DBG("FooterPanel: ERREUR: Callback onRequestResize est null !");
            }
            
            // EN PLUS : Déclencher l'animation de fade + toggle de la grille
            triggerGridToggleAnimation();
        };
        
        // Configurer le composant qui va fade in/out
        setupFadingComponent();
        
        addAndMakeVisible (midiPianoArea);
        addAndMakeVisible (fadingDemoPanel);
    }
    
    void paint(juce::Graphics& g) override
    {

    }
    
    void resized() override
    {
        auto area = getLocalBounds();

        // Utiliser Grid avec interpolation progressive selon gridTransitionFraction
        juce::Grid grid;
        
        // Définir une seule ligne
        grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        
        // Interpolation smooth entre les deux configurations
        if (gridTransitionFraction <= 0.001f)
        {
            // État compact : 3 colonnes [Fr(1) | 400px | Fr(1)]
            grid.templateColumns = { 
                juce::Grid::TrackInfo(juce::Grid::Fr(1)),      // Colonne gauche (flexible)
                juce::Grid::TrackInfo(juce::Grid::Px(400)),    // Colonne centre MidiPianoArea
                juce::Grid::TrackInfo(juce::Grid::Fr(1))       // Colonne droite (flexible)
            };
            
            // Ajouter seulement le MidiPianoArea
            grid.items.add(juce::GridItem(midiPianoArea)
                          .withArea(1, 2));       // Ligne 1, Colonne 2 (centre)
        }
        else
        {
            // État progressif : 4 colonnes avec interpolation
            // La largeur de la colonne "mystère" grandit progressivement
            float mysteryColumnWidth = 400.0f * gridTransitionFraction;
            
            grid.templateColumns = { 
                juce::Grid::TrackInfo(juce::Grid::Fr(1)),                           // Colonne gauche (flexible)
                juce::Grid::TrackInfo(juce::Grid::Px(400)),                        // Colonne MidiPianoArea (fixe)
                juce::Grid::TrackInfo(juce::Grid::Px(static_cast<int>(mysteryColumnWidth))), // Colonne FadingDemoPanel (progressive)
                juce::Grid::TrackInfo(juce::Grid::Fr(1))                           // Colonne droite (flexible)
            };
            
            // Ajouter les deux composants
            grid.items.add(juce::GridItem(midiPianoArea)
                          .withArea(1, 2)         // Ligne 1, Colonne 2
                          .withMargin(juce::GridItem::Margin(0, 4, 0, 0))); // Marge droite
            
            grid.items.add(juce::GridItem(fadingDemoPanel)
                          .withArea(1, 3)         // Ligne 1, Colonne 3
                          .withMargin(juce::GridItem::Margin(0, 0, 0, 4))); // Marge gauche
        }
        
        // Centrer le contenu
        grid.justifyContent = juce::Grid::JustifyContent::center;
        grid.alignContent = juce::Grid::AlignContent::center;
        grid.alignItems = juce::Grid::AlignItems::stretch;
        
        // Appliquer le layout
        grid.performLayout(area);
    }
    
    /** Callback déclenché quand une animation de redimensionnement est demandée */
    std::function<void()> onRequestResize;
    
private:
    MidiPianoArea midiPianoArea;
    
    // Composant qui hérite de ColoredPanel pour l'animation de fade
    class FadingDemoPanel : public ColoredPanel
    {
    public:
        FadingDemoPanel(juce::Colour color) 
            : ColoredPanel(color),
              fontManager()
        {
            // Texte de démonstration
            label.setText(juce::String::fromUTF8("🎵 Fade In/Out Demo 🎵"), juce::dontSendNotification);
            label.setJustificationType(juce::Justification::centred);
            label.setColour(juce::Label::textColourId, juce::Colours::white);
            
            // Configurer la police
            auto fontOptions = fontManager->getSFProDisplay(16.0f, FontManager::FontWeight::Medium);
            label.setFont(juce::Font(fontOptions));
            
            addAndMakeVisible(label);
        }
        
        void paint(juce::Graphics& g) override
        {
            // Appeler le paint de ColoredPanel pour le fond
            ColoredPanel::paint(g);
            
            // Ajouter une bordure brillante
            auto bounds = getLocalBounds().toFloat();
            g.setColour(juce::Colour(0xff5a4fcf));
            g.drawRoundedRectangle(bounds.reduced(1.0f), 10.0f, 2.0f);
        }
        
        void resized() override
        {
            label.setBounds(getLocalBounds());
        }
        
    private:
        juce::Label label;
        juce::SharedResourcePointer<FontManager> fontManager;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FadingDemoPanel)
    };
    
    FadingDemoPanel fadingDemoPanel;
    bool isFadingComponentVisible;
    bool isGridExpanded; // État du toggle de la grille
    float gridTransitionFraction; // Paramètre d'interpolation (0 = compact, 1 = élargi)
    
    void setupFadingComponent()
    {
        // Initialiser le composant comme invisible
        fadingDemoPanel.setAlpha(0.0f);
        fadingDemoPanel.setVisible(false);
        isFadingComponentVisible = false;
    }
    
    void triggerGridToggleAnimation()
    {
        DBG("FooterPanel: Déclenchement du toggle de grille, état actuel : " << (isGridExpanded ? "élargi" : "compact"));
        
        if (isGridExpanded)
        {
            // MASQUER : fade out → puis rétrécir la grille progressivement
            DBG("FooterPanel: Séquence MASQUER - Étape 1: Fade out du composant");
            
            // Étape 1 : Faire disparaître le composant
            isFadingComponentVisible = false;
            AnimationManager::getInstance()->fadeComponent(
                fadingDemoPanel, 
                false, // fade out
                300.0, // durée
                [this]() { // callback : quand le fade out est terminé
                    DBG("FooterPanel: Séquence MASQUER - Étape 2: Animer la grille (rétrécissement)");
                    
                    // Étape 2 : Animer la grille progressivement (1.0f → 0.0f)
                    AnimationManager::getInstance()->animateValueSimple(
                        gridTransitionFraction,
                        0.0f, // target : grille compacte
                        350.0, // durée
                        [this]() { // onUpdate : redessiner la grille à chaque frame
                            resized();
                        }
                    );
                    
                    // Marquer l'état comme compact
                    isGridExpanded = false;
                    
                    DBG("FooterPanel: Séquence MASQUER terminée");
                }
            );
        }
        else
        {
            // RÉVÉLER : élargir la grille progressivement → puis fade in
            DBG("FooterPanel: Séquence RÉVÉLER - Étape 1: Animer la grille (élargissement)");
            
            // Étape 1 : Animer la grille progressivement (0.0f → 1.0f)
            AnimationManager::getInstance()->animateValueSimple(
                gridTransitionFraction,
                1.0f, // target : grille élargie
                350.0, // durée
                [this]() { // onUpdate : redessiner la grille à chaque frame
                    resized();
                }
            );
            
            // Marquer l'état comme élargi
            isGridExpanded = true;
            
            // Petite pause pour laisser la grille se déployer un peu
            juce::Timer::callAfterDelay(150, [this]() {
                DBG("FooterPanel: Séquence RÉVÉLER - Étape 2: Fade in du composant");
                
                // Étape 2 : Faire apparaître le composant
                isFadingComponentVisible = true;
                AnimationManager::getInstance()->fadeComponent(
                    fadingDemoPanel, 
                    true, // fade in
                    300.0, // durée
                    [this]() { // callback : quand le fade in est terminé
                        DBG("FooterPanel: Séquence RÉVÉLER terminée");
                    }
                );
            });
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
}; 