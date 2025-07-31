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
    // Créer les 7 boutons circulaires basés sur les BaseNote
    for (int i = 0; i < 7; ++i)
    {
        // Couleurs différentes pour chaque bouton
        juce::Colour baseColour = juce::Colours::lightblue.withRotatedHue(i * 0.14f); // Rotation de teinte
        
        // Obtenir le nom de la note de base pour le label
        auto noteName = DiatonyText::getBaseNoteName(baseNotes[i]);
        
        circularButtons[i] = std::make_unique<CircularButton>(
            noteName,
            baseColour,
            juce::Colours::black,
            12.0f, // Taille de police plus petite pour les cercles
            FontManager::FontWeight::Medium
        );
        
        // Ajouter le callback pour les clics avec gestion de sélection
        circularButtons[i]->onClick = [i, this]() {
            auto selectedNote = baseNotes[i];
            auto noteName = DiatonyText::getBaseNoteName(selectedNote);
            DBG("Note de base " << noteName.toStdString() << " sélectionnée!");
            
            // Mettre à jour la note sélectionnée
            selectedBaseNote = selectedNote;
            
            // Désélectionner tous les autres boutons
            for (int j = 0; j < 7; ++j)
            {
                circularButtons[j]->setSelected(j == i);
            }
            
            // TODO: Ajouter la logique pour communiquer la sélection au modèle
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
    
    // FlexBox avec flex-basis défini pour une taille de base
    for (auto& button : circularButtons)
    {
        juce::FlexItem item(*button);
        
        // Définir flex-grow=1, flex-shrink=1, flex-basis=70px pour un bon équilibre
        item = item.withFlex(1.0f,    // flex-grow: grandit s'il y a de la place
                            1.0f,    // flex-shrink: rétréci proportionnellement si nécessaire
                            70.0f);  // flex-basis: taille de base de 70px
        
        // Contraintes de taille minimale pour l'accessibilité
        item = item.withMinWidth(40.0f);   // Minimum 40px si vraiment très petit écran
        item = item.withMinHeight(40.0f);  // Minimum 40px de hauteur
        
        // Contraintes de taille maximale pour éviter des boutons énormes
        item = item.withMaxWidth(120.0f);   // Maximum 120px de largeur
        item = item.withMaxHeight(120.0f);  // Maximum 120px de hauteur
        
        flexBox.items.add(item);
    }
    
    // Appliquer le layout automatique
    flexBox.performLayout(contentArea.toFloat());
}