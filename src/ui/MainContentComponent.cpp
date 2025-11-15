#include "MainContentComponent.h"
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "footer/FooterPanel.h"
#include "UIStateIdentifiers.h"
#include "extra/Component/DiatonyAlertWindow.h"

//==============================================================================
MainContentComponent::MainContentComponent() 
    : appState(),
      headerPanel(),
      sectionPanel(),
      footerPanel(),
      headerFlex(7.5f),
      sectionFlex(57.5f),
      footerFlex(15.0f)
{        
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(sectionPanel);
    addAndMakeVisible(footerPanel);
}

MainContentComponent::~MainContentComponent()
{
    if (appState.isValid())
        appState.removeListener(this);
    
    if (selectionState.isValid())
        selectionState.removeListener(this);
}

void MainContentComponent::setAppState(juce::ValueTree& state)
{
    // Déconnexion de l'ancien listener
    if (appState.isValid())
        appState.removeListener(this);
        
    // Connexion au nouveau ValueTree
    appState = state;
    appState.addListener(this);
    
    // Propagation de l'état aux panels enfants
    headerPanel.setAppState(appState);
    footerPanel.setAppState(appState);
    // sectionPanel.setAppState(appState);  // Si nécessaire plus tard
}

void MainContentComponent::setSelectionState(juce::ValueTree& state)
{
    // Déconnexion de l'ancien listener
    if (selectionState.isValid())
        selectionState.removeListener(this);
        
    // Connexion au selectionState pour écouter les changements de génération
    selectionState = state;
    selectionState.addListener(this);
}

void MainContentComponent::paint(juce::Graphics& g)
{   
    // Dégradé linéaire à 135 degrés
    auto bounds = getLocalBounds().toFloat();
    
    auto center = bounds.getCentre();
    auto diagonal = std::sqrt(bounds.getWidth() * bounds.getWidth() + bounds.getHeight() * bounds.getHeight()) * 0.5f;
    
    auto angleRad = juce::MathConstants<float>::pi * 135.0f / 180.0f;
    auto startPoint = center - juce::Point<float>(std::cos(angleRad), std::sin(angleRad)) * diagonal;
    auto endPoint = center + juce::Point<float>(std::cos(angleRad), std::sin(angleRad)) * diagonal;
    
    juce::ColourGradient gradient(
        juce::Colour::fromString("fff5f7fa"), startPoint,
        juce::Colour::fromString("ffc3cfe2"), endPoint,
        false
    );
    
    g.setGradientFill(gradient);
    g.fillRect(bounds);
}

void MainContentComponent::resized()
{
    int padding = 8;
    auto content = getLocalBounds().reduced(padding);

    // Layout vertical avec FlexBox
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;

    fb.items = {
        juce::FlexItem(headerPanel).withFlex(headerFlex).withMargin({ 0, 0, 4, 0 }),
        juce::FlexItem(sectionPanel).withFlex(sectionFlex).withMargin({ 4, 0, 4, 0 }),
        juce::FlexItem(footerPanel).withFlex(footerFlex).withMargin({ 4, 0, 0, 0 })
    };

    fb.performLayout(content);
}

float& MainContentComponent::getHeaderFlexRef()
{
    return headerFlex;
}

float& MainContentComponent::getSectionFlexRef()
{
    return sectionFlex;
}

float& MainContentComponent::getFooterFlexRef()
{
    return footerFlex;
}

FooterPanel& MainContentComponent::getFooterPanel()
{
    return footerPanel;
}

HeaderPanel& MainContentComponent::getHeaderPanel()
{
    return headerPanel;
}

SectionPanel& MainContentComponent::getSectionPanel()
{
    return sectionPanel;
}

void MainContentComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                   const juce::Identifier& property)
{
    // === GÉNÉRATION : Écoute du statut pour afficher les pop-ups (depuis selectionState) ===
    if (treeWhosePropertyHasChanged == selectionState && property == juce::Identifier("generationStatus"))
    {
        auto status = treeWhosePropertyHasChanged.getProperty("generationStatus").toString();
        
        if (status == "generating")
        {
            // TODO: Afficher un spinner de chargement si nécessaire
            DBG("🔄 MainContentComponent: Génération en cours...");
        }
        else if (status == "completed")
        {
            // ✅ Succès : Afficher le pop-up de succès
            DBG("✅ MainContentComponent: Génération réussie !");
            
            juce::MessageManager::callAsync([this]() {
                DiatonyAlertWindow::show(
                    DiatonyAlertWindow::AlertType::Success,
                    juce::String::fromUTF8("Génération Réussie"),
                    juce::String::fromUTF8("Le fichier MIDI a été généré avec succès !\n\nLa solution a été trouvée par le solveur Diatony."),
                    "OK"
                );
            });
        }
        else if (status == "error")
        {
            // ❌ Erreur : Afficher le pop-up d'erreur
            juce::String errorMessage = treeWhosePropertyHasChanged
                                            .getProperty("generationError")
                                            .toString();
            
            DBG("❌ MainContentComponent: Erreur de génération - " << errorMessage);
            
            // Utiliser callAsync pour éviter d'afficher la fenêtre modale pendant un callback ValueTree
            juce::MessageManager::callAsync([errorMessage]() {
                DiatonyAlertWindow::show(
                    DiatonyAlertWindow::AlertType::Error,
                    juce::String::fromUTF8("Échec de la Génération"),
                    juce::String::fromUTF8("Le solveur n'a pas pu trouver de solution :\n\n") + errorMessage,
                    "OK"
                );
            });
        }
    }
    
    // === LAYOUT : Notification du RootAnimator (depuis appState) ===
    if (treeWhosePropertyHasChanged == appState && property == UIStateIdentifiers::interactivePianoVisible)
    {
        if (onLayoutAnimationNeeded)
        {
            onLayoutAnimationNeeded();
        }
    }
}

void MainContentComponent::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void MainContentComponent::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void MainContentComponent::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void MainContentComponent::valueTreeParentChanged(juce::ValueTree&) {} 