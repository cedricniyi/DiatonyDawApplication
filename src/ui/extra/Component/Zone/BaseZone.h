#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"

/**
 * @brief Classe de base pour les zones de paramètres avec en-tête et corps.
 *
 * Le rectangle visuel remplit tout l'espace (le parent gère l'espacement externe).
 */
class BaseZone : public juce::Component
{
public:
    BaseZone(const juce::String& zoneTitle)
    : title(zoneTitle)
    {
        setOpaque(false);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        
        auto titleArea = bounds.withHeight(titleHeight);
        auto bodyArea = bounds.withTrimmedTop(titleHeight);
        
        // Contour global
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, borderThickness);
        
        // Corps (coins arrondis en bas)
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        juce::Path bodyPath;
        bodyPath.addRoundedRectangle(
            bodyArea.getX(), bodyArea.getY(),
            bodyArea.getWidth(), bodyArea.getHeight(),
            cornerSize, cornerSize,
            false, false,  // Pas de coins arrondis en haut
            true, true     // Coins arrondis en bas
        );
        g.fillPath(bodyPath);
        
        // En-tête (coins arrondis en haut)
        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        juce::Path headerPath;
        headerPath.addRoundedRectangle(
            titleArea.getX(), titleArea.getY(),
            titleArea.getWidth(), titleArea.getHeight(),
            cornerSize, cornerSize,
            true, true,    // Coins arrondis en haut
            false, false   // Pas de coins arrondis en bas
        );
        g.fillPath(headerPath);
        
        // Texte du titre
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        auto titleFont = juce::Font(fontManager->getSFProDisplay(14.0f, FontManager::FontWeight::Semibold));
        g.setFont(titleFont);
        g.drawText(title, titleArea.withTrimmedLeft(10), juce::Justification::left, false);
        
        auto contentArea = bodyArea.reduced(internalPadding);
        paintContent(g, contentArea);
    }
    
    void resized() override
    {
        auto contentBounds = getLocalBounds()
            .withTrimmedTop(titleHeight)
            .reduced(internalPadding);
        
        resizeContent(contentBounds);
    }
    
protected:
    virtual void paintContent(juce::Graphics& g, const juce::Rectangle<int>& contentBounds)
    {
        juce::ignoreUnused(g, contentBounds);
    }
    
    virtual void resizeContent(const juce::Rectangle<int>& contentBounds)
    {
        juce::ignoreUnused(contentBounds);
    }
    
    void setTitle(const juce::String& newTitle)
    {
        title = newTitle;
        repaint();
    }
    
    juce::String getTitle() const { return title; }
    
    // === Paramètres configurables ===
    int titleHeight = 26;           // Hauteur de l'en-tête
    int internalPadding = 6;        // Espace entre les bords et le contenu
    float cornerSize = 8.0f;        // Rayon des coins arrondis
    float borderThickness = 1.0f;   // Épaisseur de la bordure
    
private:
    juce::String title;
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseZone)
};
