#include "RootUIController.h"
#include "core/PluginProcessor.h"
#include "ui/header/HeaderPanel.h"
#include "ui/section/SectionPanel.h"
#include "ui/footer/FooterPanel.h"

// =================================================================================
// Constructeur et Destructeur

RootUIController::RootUIController(AudioPluginAudioProcessor& processor, juce::ValueTree& appStateRef) 
    : audioProcessor(processor),
      appState(appStateRef)  // NOUVEAU: Référence vers le ValueTree créé par PluginEditor
{
    setupStateListeners();
    
    DBG("RootUIController: Contrôleur initialisé avec ValueTree externe (ownership dans PluginEditor)");
}

RootUIController::~RootUIController()
{
    appState.removeListener(this);
}

// =================================================================================
// Initialisation

void RootUIController::initializePanels(HeaderPanel& header, SectionPanel& section, FooterPanel& footer)
{
    headerPanel = &header;
    sectionPanel = &section;
    footerPanel = &footer;
    
    DBG("RootUIController: Panels initialisés");
}

// SUPPRIMÉ: initializeAppState() - maintenant fait par PluginEditor

void RootUIController::setupStateListeners()
{
    appState.addListener(this);
}

// =================================================================================
// Actions publiques (appelées par les panels)

void RootUIController::setSelectedSection(int sectionIndex)
{
    appState.setProperty(UIStateIdentifiers::selectedSection, sectionIndex, nullptr);
    DBG("RootUIController: Section -> " << sectionIndex);
}

void RootUIController::setFooterExpanded(bool expanded)
{
    appState.setProperty(UIStateIdentifiers::footerExpanded, expanded, nullptr);
    DBG("RootUIController: Footer -> " << (expanded ? "étendu" : "réduit"));
}

// =================================================================================
// ValueTree::Listener callbacks

void RootUIController::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                              const juce::Identifier& property)
{
    DBG("RootUIController: " << property.toString() 
        << " = " << treeWhosePropertyHasChanged.getProperty(property).toString());
    
    notifyPanelsOfStateChange(property);
}

void RootUIController::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void RootUIController::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void RootUIController::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void RootUIController::valueTreeParentChanged(juce::ValueTree&) {}

// =================================================================================
// Notification des panels

void RootUIController::notifyPanelsOfStateChange(const juce::Identifier& property)
{
    if (property == UIStateIdentifiers::selectedSection && headerPanel != nullptr)
    {
        // TODO: headerPanel->onSectionChanged(getCurrentSelectedSection());
        DBG("RootUIController: -> HeaderPanel: section " << getCurrentSelectedSection());
    }
    
    if (property == UIStateIdentifiers::footerExpanded && footerPanel != nullptr)
    {
        // TODO: footerPanel->onFooterExpandedChanged(isFooterExpanded());
        DBG("RootUIController: -> FooterPanel: " << (isFooterExpanded() ? "étendu" : "réduit"));
    }
}

// =================================================================================
// Méthodes utilitaires

int RootUIController::getCurrentSelectedSection() const
{
    return static_cast<int>(appState.getProperty(UIStateIdentifiers::selectedSection, -1));
}

bool RootUIController::isFooterExpanded() const
{
    return static_cast<bool>(appState.getProperty(UIStateIdentifiers::footerExpanded, false));
} 