#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Classe de base pour les zones de paramètres
 * Fournit un style commun avec en-tête et corps
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
        const int titleHeight = 30;
        const int zoneBorderThickness = 1;
        const float cornerSize = 8.0f;
        const int padding = 5; // Espacement interne pour le contenu visuel
        
        // Zone interne pour le dessin, réduite par le padding
        auto drawingBounds = getLocalBounds().reduced(padding);
        
        // Séparer la zone de dessin en en-tête et corps
        auto titleArea = drawingBounds.withHeight(titleHeight);
        auto contentArea = drawingBounds.withTrimmedTop(titleHeight);
        
        // Dessiner le contour de la zone de dessin
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(drawingBounds.toFloat(), cornerSize, zoneBorderThickness);
        
        // Dessiner le corps de la zone
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        
        // Créer un chemin pour le corps avec coins arrondis en bas seulement
        juce::Path contentPath;
        contentPath.addRoundedRectangle(
            contentArea.getX(), contentArea.getY(),
            contentArea.getWidth(), contentArea.getHeight(),
            cornerSize, cornerSize,
            false, false, // Pas de coins arrondis en haut
            true, true    // Coins arrondis en bas
        );
        g.fillPath(contentPath);
        
        // Dessiner l'en-tête
        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        
        // Créer un chemin pour l'en-tête avec coins arrondis en haut seulement
        juce::Path headerPath;
        headerPath.addRoundedRectangle(
            titleArea.getX(), titleArea.getY(),
            titleArea.getWidth(), titleArea.getHeight(),
            cornerSize, cornerSize,
            true, true,  // Coins arrondis en haut
            false, false // Pas de coins arrondis en bas
        );
        g.fillPath(headerPath);
        
        // Dessiner le texte du titre
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText(title, titleArea.withTrimmedLeft(10), juce::Justification::left, false);
        
        // Laisser les classes dérivées dessiner leur contenu dans la zone de contenu interne
        paintContent(g, contentArea);
    }
    
    void resized() override
    {
        // L'espace disponible pour le contenu, sans compter l'en-tête et le padding interne
        const int titleHeight = 30;
        const int padding = 5;
        auto contentBounds = getLocalBounds().reduced(padding).withTrimmedTop(titleHeight);
        
        // Permettre aux classes dérivées de gérer leur mise en page
        resizeContent(contentBounds);
    }
    
protected:
    // Méthode que les classes dérivées peuvent surcharger pour dessiner leur contenu
    virtual void paintContent(juce::Graphics& g, const juce::Rectangle<int>& contentBounds)
    {
        // Par défaut, ne fait rien
    }
    
    // Méthode que les classes dérivées peuvent surcharger pour positionner leurs composants
    virtual void resizeContent(const juce::Rectangle<int>& contentBounds)
    {
        // Par défaut, ne fait rien
    }
    
    void setTitle(const juce::String& newTitle)
    {
        title = newTitle;
        repaint();
    }
    
    juce::String getTitle() const
    {
        return title;
    }
    
private:
    juce::String title;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseZone)
}; 