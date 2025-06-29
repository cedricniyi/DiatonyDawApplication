#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ToastComponent : public juce::Component, 
                       private juce::Timer
{
public:
    ToastComponent()
    {
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(false, false);
    }
    
    ~ToastComponent() override
    {
        stopTimer();
    }
    
    void showMessage(const juce::String& message, int durationMs = 2000)
    {
        // Stocker le message
        toastMessage = message;
        
        // Arrêter toute animation en cours
        juce::Desktop::getInstance().getAnimator().cancelAnimation(this, false);
        
        // Réinitialiser l'opacité et rendre visible
        opacity = 1.0f;
        setVisible(true);
        
        // Arrêter tout timer précédent
        stopTimer();
        
        // Démarrer le timer pour la disparition
        startTimer(durationMs);
        
        // Déclencher un redraw
        repaint();
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().reduced(20, 0);
        
        // Obtenir la largeur du texte pour centrer en utilisant TextLayout au lieu de getStringWidth
        juce::Font font(juce::FontOptions(16.0f));
        g.setFont(font);
        
        juce::GlyphArrangement glyphs;
        glyphs.addLineOfText(font, toastMessage, 0.0f, 0.0f);
        int textWidth = (int)glyphs.getBoundingBox(0, -1, true).getWidth();
        int toastWidth = textWidth + 40; // Marge supplémentaire
        
        // Créer un rectangle aligné à droite pour le toast
        auto toastBounds = juce::Rectangle<int>(
            bounds.getRight() - toastWidth,  // 20 pixels de marge à droite
            bounds.getY() + 55,                   // 52 pixels depuis le haut
            toastWidth,
            36
        );
        
        // Dessiner le fond du toast avec l'opacité actuelle
        g.setColour(juce::Colour(0xCC333333).withAlpha(opacity));
        g.fillRoundedRectangle(toastBounds.toFloat(), 8.0f);
        
        // Dessiner la bordure
        g.setColour(juce::Colour(0xFF555555).withAlpha(opacity));
        g.drawRoundedRectangle(toastBounds.toFloat(), 8.0f, 1.0f);
        
        // Dessiner le texte
        g.setColour(juce::Colours::white.withAlpha(opacity));
        g.drawText(toastMessage, toastBounds, juce::Justification::centred, false);
    }
    
    void resized() override
    {
        // Pas besoin de disposition spécifique car nous calculons tout dans paint()
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
        // Au lieu d'utiliser l'animateur, nous allons gérer manuellement le fondu
        // Cela évitera des problèmes potentiels avec l'animateur de JUCE
        
        opacity = 1.0f;
        
        class FadeOutTimerCallback : public juce::Timer 
        {
        public:
            FadeOutTimerCallback(ToastComponent* owner) : owner(owner) {}
            
            void timerCallback() override 
            {
                // Réduire l'opacité graduellement
                owner->opacity -= 0.1f;
                
                if (owner->opacity <= 0.0f) {
                    // Animation terminée
                    owner->opacity = 0.0f;
                    owner->setVisible(false);
                    
                    // S'auto-détruire
                    stopTimer();
                    delete this;
                } else {
                    // Continuer l'animation
                    owner->repaint();
                }
            }
            
        private:
            ToastComponent* owner;
        };
        
        // Démarrer le timer de fondu (30ms entre chaque étape = ~300ms pour le fondu complet)
        (new FadeOutTimerCallback(this))->startTimer(30);
    }
    
    juce::String toastMessage;
    float opacity = 1.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToastComponent)
}; 