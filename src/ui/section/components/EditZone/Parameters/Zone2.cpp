#include "Zone2.h"

Zone2::Zone2()
{
    setupStyledButtons();
}

Zone2::~Zone2()
{
}

void Zone2::paint(juce::Graphics& g)
{    
    // Dessiner le titre avec FontManager
    g.setColour(juce::Colours::black);
    auto titleFont = juce::Font(fontManager->getSFProDisplay(16.0f, FontManager::FontWeight::Bold));
    g.setFont(titleFont);
    g.drawText("Alteration", titleArea, juce::Justification::centred);
}

void Zone2::resized()
{
    auto bounds = getLocalBounds();
    
    // Diviser en 2 parties : 30% titre, 70% contenu
    int titleHeight = static_cast<int>(bounds.getHeight() * 0.3f);
    
    titleArea = bounds.removeFromTop(titleHeight);
    contentArea = bounds; // Le reste pour le contenu
    
    // Layout des 3 boutons StyledButton avec FlexBox
    layoutStyledButtons();
}

void Zone2::setupStyledButtons()
{
    // Créer les 3 boutons SelectableStyledButton basés sur les altérations
    for (int i = 0; i < 3; ++i)
    {
        // Couleurs cohérentes : gris normal, bleu pour sélection
        juce::Colour normalColour = juce::Colours::lightgrey;
        juce::Colour selectedColour = juce::Colours::blue;
        
        // Obtenir le symbole de l'altération pour le label
        auto alterationSymbol = DiatonyText::getAlterationSymbol(alterations[i]);
        
        styledButtons[i] = std::make_unique<SelectableStyledButton>(
            alterationSymbol,
            normalColour,
            selectedColour,
            24.0f, // Taille de police plus grande pour les symboles d'altération
            FontManager::FontWeight::Semibold
        );
        
        // Ajouter le callback pour les clics avec gestion de sélection
        styledButtons[i]->onClick = [i, this]() {
            auto selectedAlt = alterations[i];
            auto altSymbol = DiatonyText::getAlterationSymbol(selectedAlt);
            DBG("Altération " << altSymbol.toStdString() << " sélectionnée!");
            
            // Mettre à jour l'altération sélectionnée
            selectedAlteration = selectedAlt;
            
            // Désélectionner tous les autres boutons et sélectionner le bon
            for (int j = 0; j < 3; ++j)
            {
                styledButtons[j]->setSelected(j == i);
            }
            
            // Notifier l'aval si un binding est présent
            if (onAlterationChanged)
                onAlterationChanged(selectedAlteration);
        };
        
        // Pas de sélection par défaut hardcodée
        // La valeur réelle sera appliquée par setSelectedAlteration() depuis le modèle
        
        addAndMakeVisible(*styledButtons[i]);
    }
}

void Zone2::setSelectedAlteration(Diatony::Alteration alt)
{
    selectedAlteration = alt;

    for (size_t j = 0; j < styledButtons.size(); ++j)
    {
        bool isSelected = (alterations[j] == selectedAlteration);
        styledButtons[j]->setSelected(isSelected);
    }

    repaint();
}

void Zone2::layoutStyledButtons()
{
    if (contentArea.isEmpty())
        return;
    
    // Utiliser FlexBox pour disposer les 3 boutons sur une rangée
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround; // Plus d'espace automatique entre boutons
    flexBox.alignItems = juce::FlexBox::AlignItems::center;
    
    // FlexBox full-auto : laisser FlexBox gérer automatiquement la répartition
    for (auto& button : styledButtons)
    {
        juce::FlexItem item(*button);
        
        // Chaque bouton aura la même part d'espace disponible
        item = item.withFlex(1.0f);
        
        // Contraintes pour des boutons carrés (corrigées)
        item = item.withMinWidth(60.0f);   // Minimum 60px de largeur
        item = item.withMinHeight(40.0f);  // Minimum 60px de hauteur (vraiment carré maintenant)
        
        // Contraintes de taille maximale
        item = item.withMaxWidth(120.0f);  // Maximum 120px de largeur
        item = item.withMaxHeight(120.0f); // Maximum 120px de hauteur (carré)
        
        // Espacement entre les boutons (plus prononcé)
        item = item.withMargin(juce::FlexItem::Margin(12.0f)); // 12px au lieu de 5px
        
        flexBox.items.add(item);
    }
    
    // Appliquer le layout automatique
    flexBox.performLayout(contentArea.toFloat());
}