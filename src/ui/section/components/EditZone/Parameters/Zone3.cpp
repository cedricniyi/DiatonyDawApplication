#include "Zone3.h"

Zone3::Zone3()
{
    setupStyledButtons();
}

Zone3::~Zone3()
{
}

void Zone3::paint(juce::Graphics& g)
{
    // Dessiner le titre avec FontManager
    g.setColour(juce::Colours::black);
    auto titleFont = juce::Font(fontManager->getSFProDisplay(16.0f, FontManager::FontWeight::Bold));
    g.setFont(titleFont);
    g.drawText("Mode", titleArea, juce::Justification::centred);
}

void Zone3::resized()
{
    auto bounds = getLocalBounds();
    
    // Diviser en 2 parties : 30% titre, 70% contenu
    int titleHeight = static_cast<int>(bounds.getHeight() * 0.3f);
    
    titleArea = bounds.removeFromTop(titleHeight);
    contentArea = bounds; // Le reste pour le contenu
    
    // Layout des 2 boutons StyledButton avec FlexBox
    layoutStyledButtons();
}

void Zone3::setupStyledButtons()
{
    // Créer les 2 boutons SelectableStyledButton basés sur les modes
    for (int i = 0; i < 2; ++i)
    {
        // Couleurs cohérentes : gris normal, bleu pour sélection
        juce::Colour normalColour = juce::Colours::lightgrey;
        juce::Colour selectedColour = juce::Colours::blue;
        
        // Obtenir le nom du mode pour le label
        auto modeName = DiatonyText::getModeName(modes[i]);
        
        styledButtons[i] = std::make_unique<SelectableStyledButton>(
            modeName,
            normalColour,
            selectedColour,
            20.0f, // Taille de police légèrement plus petite pour les rectangles
            FontManager::FontWeight::Medium
        );
        
        // Ajouter le callback pour les clics avec gestion de sélection
        styledButtons[i]->onClick = [i, this]() {
            auto selectedModeValue = modes[i];
            auto modeName = DiatonyText::getModeName(selectedModeValue);
            DBG("Mode " << modeName.toStdString() << " sélectionné!");
            
            // Mettre à jour le mode sélectionné
            selectedMode = selectedModeValue;
            
            // Désélectionner tous les autres boutons et sélectionner le bon
            for (int j = 0; j < 2; ++j)
            {
                styledButtons[j]->setSelected(j == i);
            }
            
            // Notifier l'aval si un binding est présent
            if (onModeChanged)
                onModeChanged(selectedMode);
        };
        
        // Pas de sélection par défaut hardcodée
        // La valeur réelle sera appliquée par setSelectedMode() depuis le modèle
        
        addAndMakeVisible(*styledButtons[i]);
    }
}

void Zone3::setSelectedMode(Diatony::Mode mode)
{
    selectedMode = mode;
    for (size_t j = 0; j < styledButtons.size(); ++j)
    {
        bool isSelected = (modes[j] == selectedMode);
        styledButtons[j]->setSelected(isSelected);
    }
    repaint();
}

void Zone3::layoutStyledButtons()
{
    if (contentArea.isEmpty())
        return;
    
    // Utiliser FlexBox pour disposer les 2 boutons sur une rangée
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::center; // Centrer les boutons
    flexBox.alignItems = juce::FlexBox::AlignItems::center;
    
    // FlexBox full-auto : laisser FlexBox gérer automatiquement la répartition
    for (auto& button : styledButtons)
    {
        juce::FlexItem item(*button);
        
        // Chaque bouton aura la même part d'espace disponible
        item = item.withFlex(1.0f);
        
        // Contraintes pour des boutons rectangulaires (largeur > hauteur)
        item = item.withMinWidth(80.0f);   // Minimum 80px de largeur
        item = item.withMinHeight(40.0f);  // Minimum 40px de hauteur (rectangulaire)
        
        // Contraintes de taille maximale (format rectangulaire)
        item = item.withMaxWidth(160.0f);  // Maximum 160px de largeur
        item = item.withMaxHeight(70.0f);  // Maximum 70px de hauteur (rectangulaire)
        
        // Espacement entre les boutons
        item = item.withMargin(juce::FlexItem::Margin(8.0f));
        
        flexBox.items.add(item);
    }
    
    // Appliquer le layout automatique
    flexBox.performLayout(contentArea.toFloat());
}