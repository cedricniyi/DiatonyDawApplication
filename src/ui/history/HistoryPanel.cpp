#include "HistoryPanel.h"
#include "ui/UIStateIdentifiers.h"
#include "utils/FontManager.h"

//==============================================================================
// ContentArea implementation
HistoryPanel::ContentArea::ContentArea()
{
    setOpaque(false);
}

void HistoryPanel::ContentArea::paint(juce::Graphics& g)
{
    // Placeholder : fond légèrement différent pour visualiser la zone
    g.fillAll(juce::Colour(0xFF1A1A1A));
    
    // Bordure à gauche (continuité avec le header)
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, 0, 1, getHeight());
    
    // Message placeholder
    g.setColour(juce::Colours::grey);
    g.setFont(14.0f);
    g.drawText("Historique des actions...", 
               getLocalBounds().reduced(16), 
               juce::Justification::centredTop, true);
}

//==============================================================================
// HistoryPanel implementation
HistoryPanel::HistoryPanel() 
    : isPanelVisible(false),
      widthTransitionFraction(0.0f)
{
    // Configuration du header - titre centré
    headerLabel.setText("History", juce::dontSendNotification);
    headerLabel.setJustificationType(juce::Justification::centred);
    headerLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    
    // Même style de police que le header principal
    auto fontOptions = fontManager->getSFProDisplay(18.0f, FontManager::FontWeight::Semibold);
    headerLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(headerLabel);
    
    // Zone de contenu (initialement invisible pour le fade)
    contentArea.setAlpha(0.0f);
    addAndMakeVisible(contentArea);
}

HistoryPanel::~HistoryPanel()
{
    if (appState.isValid())
        appState.removeListener(this);
}

void HistoryPanel::setAppState(juce::ValueTree& state)
{
    if (appState.isValid())
        appState.removeListener(this);
        
    appState = state;
    appState.addListener(this);
    
    // Synchronise l'état initial
    updateVisibilityState();
}

void HistoryPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Fond principal du panneau (légèrement plus foncé que le fond principal)
    g.fillAll(juce::Colour(0xFF1E1E1E));
    
    // Header zone - fond identique au HeaderPanel principal
    auto headerZone = bounds.removeFromTop(HEADER_HEIGHT);
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRect(headerZone);
    
    // Bordure en bas du header (même style que HeaderPanel)
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, HEADER_HEIGHT - 1, getWidth(), 1);
    
    // Bordure à gauche du panneau (séparation avec le contenu principal)
    g.setColour(juce::Colour(0xFF444444));
    g.fillRect(0, 0, 1, getHeight());
}

void HistoryPanel::resized()
{
    auto bounds = getLocalBounds();
    
    // Header - titre centré sur toute la largeur
    auto headerArea = bounds.removeFromTop(HEADER_HEIGHT);
    headerLabel.setBounds(headerArea);
    
    // Zone de contenu (reste de l'espace)
    contentArea.setBounds(bounds);
}

void HistoryPanel::setExpanded(bool expanded)
{
    isPanelVisible = expanded;
}

bool HistoryPanel::getExpanded() const
{
    return isPanelVisible;
}

juce::Component& HistoryPanel::getFadingComponent()
{
    return contentArea;
}

float& HistoryPanel::getWidthFractionRef()
{
    return widthTransitionFraction;
}

float HistoryPanel::getWidthFraction() const
{
    return widthTransitionFraction;
}

void HistoryPanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                           const juce::Identifier& property)
{
    if (property == UIStateIdentifiers::historyPanelVisible)
    {
        updateVisibilityState();
    }
}

void HistoryPanel::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void HistoryPanel::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void HistoryPanel::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void HistoryPanel::valueTreeParentChanged(juce::ValueTree&) {}

void HistoryPanel::updateVisibilityState()
{
    if (!appState.isValid()) return;
    
    bool visible = static_cast<bool>(appState.getProperty(UIStateIdentifiers::historyPanelVisible, false));
    
    // Notifie l'Animator pour déclencher l'animation de visibilité
    if (onVisibilityChange)
    {
        onVisibilityChange(visible);
    }
    else
    {
        // Fallback : mise à jour directe de l'état si aucun animateur n'est attaché
        setExpanded(visible);
        widthTransitionFraction = visible ? 1.0f : 0.0f;
    }
}

