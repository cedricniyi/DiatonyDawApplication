#include "Zone4ContentArea.h"
#include "ui/extra/Component/Panel/InfoColoredPanel.h"

//==============================================================================
Zone4ContentArea::Zone4ContentArea() 
    : ColoredPanel(juce::Colours::lightsteelblue.withAlpha(0.75f)),
      scrollableContent(std::make_unique<Zone4ScrollablePanel>())
{
    setAlpha(1.0f);
    
    setupViewport();
    setupEmptyLabel();
    updateVisibility();
}

Zone4ContentArea::~Zone4ContentArea()
{
}

void Zone4ContentArea::paint(juce::Graphics& g)
{
    ColoredPanel::paint(g);
}

void Zone4ContentArea::resized()
{
    ColoredPanel::resized();
    
    // Padding asymétrique : côtés + haut, mais pas en bas pour garder la scrollbar accessible
    auto bounds = getLocalBounds();
    auto contentArea = bounds.withTrimmedLeft(HORIZONTAL_PADDING)
                             .withTrimmedRight(HORIZONTAL_PADDING)
                             .withTrimmedTop(TOP_PADDING)
                             .withTrimmedBottom(BOTTOM_PADDING);
    
    viewport.setBounds(contentArea);
    emptyLabel.setBounds(contentArea);
    
    if (scrollableContent)
    {
        // Plus de hauteur disponible pour les rectangles
        int availableHeight = contentArea.getHeight();
        scrollableContent->setSize(scrollableContent->getWidth(), availableHeight);
    }
}

void Zone4ContentArea::addRectangle()
{
    // Version sans ValueTree - appelle la version complète avec un ValueTree invalide
    addRectangle(juce::ValueTree());
}

void Zone4ContentArea::addRectangle(juce::ValueTree chordState)
{
    if (!scrollableContent)
        return;
    
    // Créer un rectangle coloré avec zones d'information
    auto newRectangle = std::make_unique<InfoColoredPanel>(getNextColour());
    
    // Configuration du rectangle
    newRectangle->setAlpha(0.8f);
    
    // Définir le numéro de ce panel (utilise nextRectangleId avant son incrémentation dans getNextColour)
    newRectangle->setNumber(nextRectangleId - 1);
    
    // Peupler les combobox avec les valeurs validées depuis DiatonyTypes
    populateInfoColoredPanel(newRectangle.get());
    
    // Si un ValueTree est fourni, initialiser les comboboxes et connecter au modèle
    if (chordState.isValid())
    {
        // Lire les valeurs actuelles du chord
        int degree = chordState.getProperty(ModelIdentifiers::degree, 0);
        int quality = chordState.getProperty(ModelIdentifiers::quality, -1); // Auto par défaut
        int state = chordState.getProperty(ModelIdentifiers::state, 0);
        
        // Initialiser les comboboxes avec ces valeurs
        // Note : pour quality, on doit convertir entre enum value (-1, 0, 1...) et combobox index (0, 1, 2...)
        // Formule : comboboxIndex = enumValue + 1 (car Auto=-1 est à l'index 0)
        newRectangle->getDegreeCombo().setSelectedItemIndex(degree, juce::dontSendNotification);
        newRectangle->getStateCombo().setSelectedItemIndex(state, juce::dontSendNotification);
        newRectangle->getQualityCombo().setSelectedItemIndex(quality + 1, juce::dontSendNotification);
        
        // Connecter les onChange des comboboxes au ValueTree
        // On capture le ValueTree par valeur (c'est léger, juste un pointeur interne)
        // et les pointeurs vers les comboboxes
        auto* degreeCombo = &newRectangle->getDegreeCombo();
        auto* stateCombo = &newRectangle->getStateCombo();
        auto* qualityCombo = &newRectangle->getQualityCombo();
        
        degreeCombo->onChange = [chordState, degreeCombo]() mutable {
            int newDegree = degreeCombo->getSelectedItemIndex();
            chordState.setProperty(ModelIdentifiers::degree, newDegree, nullptr);
        };
        
        stateCombo->onChange = [chordState, stateCombo]() mutable {
            int newState = stateCombo->getSelectedItemIndex();
            chordState.setProperty(ModelIdentifiers::state, newState, nullptr);
        };
        
        qualityCombo->onChange = [chordState, qualityCombo]() mutable {
            int comboboxIndex = qualityCombo->getSelectedItemIndex();
            // Conversion inverse : enumValue = comboboxIndex - 1 (car Auto=-1 est à l'index 0)
            int newQuality = comboboxIndex - 1;
            chordState.setProperty(ModelIdentifiers::quality, newQuality, nullptr);
        };
    }
    else
    {
        // Si pas de ValueTree, définir les valeurs par défaut visuelles
        // Degré : I (index 0), État : Fondamental (index 0), Qualité : Auto (index 0)
        newRectangle->getDegreeCombo().setSelectedItemIndex(0, juce::dontSendNotification);
        newRectangle->getStateCombo().setSelectedItemIndex(0, juce::dontSendNotification);
        newRectangle->getQualityCombo().setSelectedItemIndex(0, juce::dontSendNotification); // Auto
    }
    
    // Ajout au contenu scrollable
    std::unique_ptr<juce::Component> component(newRectangle.release());
    scrollableContent->addRectangle(std::move(component));
    
    updateVisibility();
    resized();
    
    // Faire défiler automatiquement vers le dernier élément ajouté
    // Attendre que le layout soit terminé avant de scroller
    juce::MessageManager::callAsync([this]() {
        if (scrollableContent)
        {
            // Calculer la position X du dernier rectangle
            int numRectangles = scrollableContent->getNumRectangles();
            if (numRectangles > 0)
            {
                // Largeur totale du contenu - largeur de la zone visible
                int contentWidth = scrollableContent->getWidth();
                int viewWidth = viewport.getViewWidth();
                
                // Position X pour afficher le dernier élément (scroll à droite)
                int scrollToX = juce::jmax(0, contentWidth - viewWidth);
                
                // Faire défiler horizontalement (Y reste à 0)
                viewport.setViewPosition(scrollToX, 0);
            }
        }
    });
}

void Zone4ContentArea::clearAllRectangles()
{
    if (scrollableContent)
    {
        scrollableContent->clearAllRectangles();
        nextRectangleId = 1; // Reset du compteur
        updateVisibility();
        resized();
    }
}

bool Zone4ContentArea::hasContent() const
{
    return scrollableContent && scrollableContent->getNumRectangles() > 0;
}

juce::Rectangle<int> Zone4ContentArea::getPreferredSize() const
{
    return juce::Rectangle<int>(0, 0, PREFERRED_WIDTH, PREFERRED_HEIGHT);
}

void Zone4ContentArea::setupViewport()
{
    // Hauteur initiale calculée avec le nouveau padding
    int initialHeight = PREFERRED_HEIGHT - TOP_PADDING - BOTTOM_PADDING;
    scrollableContent->setSize(100, initialHeight);
    
    // Configuration du viewport avec scrollbar horizontale uniquement
    viewport.setViewedComponent(scrollableContent.get(), false);
    viewport.setScrollBarsShown(false, true, false, false);
    viewport.setScrollBarPosition(true, true);
    
    addAndMakeVisible(viewport);
}

void Zone4ContentArea::setupEmptyLabel()
{
    // Configuration du label d'état vide
    emptyLabel.setText(juce::String::fromUTF8("Aucun élément ajouté"), juce::dontSendNotification);
    emptyLabel.setJustificationType(juce::Justification::centred);
    emptyLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withAlpha(0.7f));
    
    // Application de la police via FontManager
    juce::SharedResourcePointer<FontManager> fontManager;
    auto fontOptions = fontManager->getSFProText(14.0f, FontManager::FontWeight::Regular);
    emptyLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(emptyLabel);
}

void Zone4ContentArea::updateVisibility()
{
    bool hasContentNow = hasContent();
    
    // Affichage conditionnel : viewport si contenu, label si vide
    viewport.setVisible(hasContentNow);
    emptyLabel.setVisible(!hasContentNow);
}

juce::Colour Zone4ContentArea::getNextColour()
{
    // Rotation des couleurs disponibles
    auto colour = availableColours[(nextRectangleId - 1) % availableColours.size()];
    nextRectangleId++;
    return colour;
}

void Zone4ContentArea::populateInfoColoredPanel(InfoColoredPanel* panel)
{
    if (!panel)
        return;
    
    // Préparer les listes de noms pour chaque combobox
    
    // 1. ChordDegree (degré d'accord) - Zone gauche
    juce::StringArray degreeNames;
    for (const auto& degree : chordDegrees)
    {
        degreeNames.add(DiatonyText::getChordDegreeName(degree));
    }
    panel->populateLeftCombo(degreeNames);
    
    // 2. ChordState (état d'inversion) - Zone top-right
    juce::StringArray stateNames;
    for (const auto& state : chordStates)
    {
        stateNames.add(DiatonyText::getChordStateName(state));
    }
    panel->populateTopRightCombo(stateNames);
    
    // 3. ChordQuality (qualité d'accord) - Zone bottom-right
    juce::StringArray qualityNames;
    for (const auto& quality : chordQualities)
    {
        qualityNames.add(DiatonyText::getChordQualityName(quality));
    }
    panel->populateBottomRightCombo(qualityNames);
}