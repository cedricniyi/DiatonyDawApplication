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
    // Dessiner le fond du titre
    g.setColour(titleBackgroundColour);
    g.fillRect(titleArea);
    
    // Dessiner le fond du contenu (plus foncé)
    g.setColour(contentBackgroundColour);
    g.fillRect(contentArea);
    
    // Dessiner la bordure globale
    g.setColour(borderColour);
    g.drawRect(getLocalBounds(), 1);
    
    // Dessiner une ligne de séparation entre titre et contenu
    g.setColour(borderColour.withAlpha(0.3f));
    g.drawHorizontalLine(titleArea.getBottom(), static_cast<float>(titleArea.getX()), static_cast<float>(titleArea.getRight()));
    
    // Dessiner le titre avec FontManager
    g.setColour(juce::Colours::black);
    auto titleFont = juce::Font(fontManager->getSFProDisplay(16.0f, FontManager::FontWeight::Bold));
    g.setFont(titleFont);
    g.drawText("Zone 2", titleArea, juce::Justification::centred);
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
    // Créer les 3 boutons StyledButton basés sur les altérations
    for (int i = 0; i < 3; ++i)
    {
        // Couleurs différentes pour chaque bouton
        juce::Colour normalColour = juce::Colours::darkgreen.withRotatedHue(i * 0.2f); // Rotation de teinte
        juce::Colour highlightColour = normalColour.brighter(0.3f);
        
        // Obtenir le symbole de l'altération pour le label
        auto alterationSymbol = DiatonyText::getAlterationSymbol(alterations[i]);
        
        styledButtons[i] = std::make_unique<StyledButton>(
            alterationSymbol,
            normalColour,
            highlightColour,
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
            
            // Désélectionner tous les autres boutons
            for (int j = 0; j < 3; ++j)
            {
                styledButtons[j]->setToggleState(j == i, juce::dontSendNotification);
            }
            
            // TODO: Ajouter la logique pour communiquer la sélection au modèle
        };
        
        // Rendre les boutons "toggleable"
        styledButtons[i]->setClickingTogglesState(true);
        
        // Définir "Natural" comme sélectionné par défaut
        if (alterations[i] == Diatony::Alteration::Natural)
        {
            styledButtons[i]->setToggleState(true, juce::dontSendNotification);
        }
        
        addAndMakeVisible(*styledButtons[i]);
    }
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