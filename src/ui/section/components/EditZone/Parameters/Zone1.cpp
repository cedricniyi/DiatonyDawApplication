#include "Zone1.h"

Zone1::Zone1()
{
    setupCircularButtons();
}

Zone1::~Zone1()
{
}

void Zone1::paint(juce::Graphics& g)
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
    g.drawText("Zone 1", titleArea, juce::Justification::centred);
}

void Zone1::resized()
{
    auto bounds = getLocalBounds();
    
    // Diviser en 2 parties : 30% titre, 70% contenu
    int titleHeight = static_cast<int>(bounds.getHeight() * 0.3f);
    
    titleArea = bounds.removeFromTop(titleHeight);
    contentArea = bounds; // Le reste pour le contenu
    
    // Layout des 7 boutons circulaires avec FlexBox
    layoutCircularButtons();
}

void Zone1::setupCircularButtons()
{
    // Créer les 7 boutons circulaires
    for (int i = 0; i < 7; ++i)
    {
        // Couleurs différentes pour chaque bouton
        juce::Colour baseColour = juce::Colours::lightblue.withRotatedHue(i * 0.14f); // Rotation de teinte
        
        circularButtons[i] = std::make_unique<CircularButton>(
            juce::String(i + 1),
            baseColour,
            juce::Colours::black,
            12.0f, // Taille de police plus petite pour les cercles
            FontManager::FontWeight::Medium
        );
        
        // Ajouter le callback pour les clics avec gestion de sélection
        circularButtons[i]->onClick = [i, this]() {
            DBG("Bouton circulaire " << (i + 1) << " cliqué!");
            
            // Désélectionner tous les autres boutons
            for (int j = 0; j < 7; ++j)
            {
                circularButtons[j]->setSelected(j == i);
            }
            
            // TODO: Ajouter la logique spécifique pour chaque bouton
        };
        
        addAndMakeVisible(*circularButtons[i]);
    }
}

void Zone1::layoutCircularButtons()
{
    if (contentArea.isEmpty())
        return;
    
    // Utiliser FlexBox pour disposer les 7 boutons sur une rangée
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    flexBox.alignItems = juce::FlexBox::AlignItems::center;
    
    // FlexBox full-auto : laisser FlexBox gérer automatiquement la répartition
    for (auto& button : circularButtons)
    {
        juce::FlexItem item(*button);
        
        // Chaque bouton aura la même part d'espace disponible
        item = item.withFlex(1.0f);
        
        // Contraintes de taille minimale pour rester lisible
        item = item.withMinWidth(50.0f);   // Minimum 30px de largeur
        item = item.withMinHeight(50.0f);  // Minimum 30px de hauteur
        
        // Contraintes de taille maximale pour éviter des boutons trop grands
        item = item.withMaxWidth(150.0f);   // Maximum 80px de largeur
        item = item.withMaxHeight(150.0f);  // Maximum 80px de hauteur
        
        flexBox.items.add(item);
    }
    
    // Appliquer le layout automatique
    flexBox.performLayout(contentArea.toFloat());
}