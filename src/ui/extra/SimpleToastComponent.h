#pragma once

#include <JuceHeader.h>

class SimpleToastComponent : public juce::Component, 
                            private juce::Timer
{
public:
    SimpleToastComponent()
    {
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(false, false);
        setVisible(false);
    }
    
    ~SimpleToastComponent() override
    {
        stopTimer();
    }
    
    void showMessage(const juce::String& message, int durationMs = 2000)
    {
        toastMessage = message;
        opacity = 1.0f;
        setVisible(true);
        
        // Arrêter tout timer précédent
        stopTimer();
        
        // Démarrer le timer pour la disparition
        startTimer(durationMs);
        
        repaint();
    }
    
    void paint(juce::Graphics& g) override
    {
        if (!isVisible() || toastMessage.isEmpty()) return;
        
        auto bounds = getLocalBounds().reduced(20, 0);
        
        // Calculer la largeur du texte
        juce::Font font(juce::FontOptions(14.0f));
        int textWidth = font.getStringWidth(toastMessage);
        int toastWidth = textWidth + 40; // Marge supplémentaire
        
        // Créer un rectangle aligné à droite pour le toast
        auto toastBounds = juce::Rectangle<int>(
            bounds.getRight() - toastWidth,
            bounds.getY() + 20,
            toastWidth,
            30
        );
        
        // Dessiner le fond du toast avec l'opacité actuelle
        g.setColour(juce::Colour(0xCC333333).withAlpha(opacity));
        g.fillRoundedRectangle(toastBounds.toFloat(), 6.0f);
        
        // Dessiner la bordure
        g.setColour(juce::Colour(0xFF666666).withAlpha(opacity));
        g.drawRoundedRectangle(toastBounds.toFloat(), 6.0f, 1.0f);
        
        // Dessiner le texte
        g.setColour(juce::Colours::white.withAlpha(opacity));
        g.setFont(font);
        g.drawText(toastMessage, toastBounds, juce::Justification::centred, false);
    }
    
    void resized() override
    {
        // Pas besoin de disposition spécifique
    }
    
private:
    void timerCallback() override
    {
        // Fin du délai, démarrer le fondu
        stopTimer();
        startFadeOut();
    }
    
    void startFadeOut()
    {
        // Fondu simple sans timer externe
        opacity = 0.0f;
        setVisible(false);
        repaint();
    }
    
    juce::String toastMessage;
    float opacity = 1.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleToastComponent)
}; 