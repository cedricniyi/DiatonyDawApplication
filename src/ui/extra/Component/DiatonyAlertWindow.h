#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "../Button/StyledButton.h"

//==============================================================================
/**
 * @brief Pop-up modal moderne pour afficher des messages à l'utilisateur
 * 
 * Utilise FontManager pour la typographie et suit le design system de l'application.
 * Supporte différents types d'alertes : succès, erreur, avertissement, info.
 */
class DiatonyAlertWindow : public juce::Component
{
public:
    enum class AlertType
    {
        Success,    // ✓ Vert
        Error,      // ✗ Rouge
        Warning,    // ⚠ Orange
        Info        // ℹ Bleu
    };
    
    DiatonyAlertWindow(AlertType type,
                      const juce::String& titleText,
                      const juce::String& messageText,
                      const juce::String& buttonText,
                      std::function<void()> onCloseCallback);

    void paint(juce::Graphics& g) override;
    void resized() override;

    /**
     * @brief Affiche le pop-up de manière asynchrone (non-bloquant)
     */
    static void show(AlertType type,
                    const juce::String& title,
                    const juce::String& message,
                    const juce::String& buttonText = "OK",
                    std::function<void()> onCloseCallback = nullptr);

private:
    void drawIcon(juce::Graphics& g, juce::Rectangle<float> iconArea);
    juce::Colour getAccentColour() const;
    
    // Composants pour le layout FlexBox
    class IconComponent : public juce::Component
    {
    public:
        IconComponent(AlertType type, const juce::Colour& colour) 
            : alertType(type), accentColour(colour) 
        {
            setOpaque(false);
        }
        
        void paint(juce::Graphics& g) override;
        
    private:
        AlertType alertType;
        juce::Colour accentColour;
        juce::SharedResourcePointer<FontManager> fontManager;
    };
    
    class TitleComponent : public juce::Component
    {
    public:
        TitleComponent(const juce::String& text) : titleText(text)
        {
            setOpaque(false);
        }
        
        void paint(juce::Graphics& g) override;
        
    private:
        juce::String titleText;
        juce::SharedResourcePointer<FontManager> fontManager;
    };
    
    class MessageComponent : public juce::Component
    {
    public:
        MessageComponent(const juce::String& text) : messageText(text)
        {
            setOpaque(false);
        }
        
        void paint(juce::Graphics& g) override;
        
    private:
        juce::String messageText;
        juce::SharedResourcePointer<FontManager> fontManager;
    };
    
    AlertType alertType;
    juce::String title;
    juce::String message;
    
    std::unique_ptr<IconComponent> iconComponent;
    std::unique_ptr<TitleComponent> titleComponent;
    std::unique_ptr<MessageComponent> messageComponent;
    std::unique_ptr<StyledButton> okButton;
    
    std::function<void()> onClose;
    juce::SharedResourcePointer<FontManager> fontManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyAlertWindow)
};

//==============================================================================
/**
 * @brief Composant wrapper qui affiche un overlay et le pop-up centré
 */
class DiatonyAlertWindowWithOverlay : public juce::Component
{
public:
    DiatonyAlertWindowWithOverlay(std::unique_ptr<DiatonyAlertWindow> alertWindow)
        : alert(std::move(alertWindow))
    {
        // Optimisation : dire à JUCE que ce composant est opaque pour éviter les repaints inutiles du parent
        setOpaque(true);
        addAndMakeVisible(alert.get());
    }
    
    void paint(juce::Graphics& g) override
    {
        // Overlay semi-transparent (optimisé : fillAll est rapide)
        g.fillAll(juce::Colours::black.withAlpha(0.4f));
    }
    
    void resized() override
    {
        // Centrer le pop-up (calcul minimal)
        if (alert)
        {
            auto bounds = getLocalBounds();
            auto alertBounds = juce::Rectangle<int>(0, 0, alert->getWidth(), alert->getHeight());
            alert->setBounds(alertBounds.withCentre(bounds.getCentre()));
        }
    }
    
private:
    std::unique_ptr<DiatonyAlertWindow> alert;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyAlertWindowWithOverlay)
};