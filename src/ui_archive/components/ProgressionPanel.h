#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../model/Progression.h"

class ProgressionPanel : public juce::Component {
public:
    ProgressionPanel() {
        // Initialisation des composants
        progressionLabel = std::make_unique<juce::Label>();
        progressionInput = std::make_unique<juce::TextEditor>();
        progressionStateLabel = std::make_unique<juce::Label>();
        progressionStateInput = std::make_unique<juce::TextEditor>();
        chordQualitiesLabel = std::make_unique<juce::Label>();
        chordQualitiesInput = std::make_unique<juce::TextEditor>();
        
        // Configuration du label et input pour la progression
        addAndMakeVisible(*progressionLabel);
        addAndMakeVisible(*progressionInput);
        
        progressionLabel->setText("Progression:", juce::dontSendNotification);
        progressionLabel->setJustificationType(juce::Justification::right);
        progressionInput->setMultiLine(false);
        progressionInput->setTextToShowWhenEmpty(juce::String::fromUTF8("Entrez la progression (ex: I VI V/V Vda V I) [progression séparée par des espaces ou \"-\"]"), juce::Colours::grey);
        
        // Tooltip pour la progression
        juce::String progressionTooltip = juce::String::fromUTF8(
            "Les degrés possibles sont : \n"
            "\tI   : Premier degré\n"
            "\tII  : Deuxième degré\n"
            "\tIII : Troisième degré\n"
            "\tIV  : Quatrième degré\n"
            "\tV   : Cinquième degré\n"
            "\tVI  : Sixième degré\n"
            "\tVII : Septième degré\n"
            "\tVda : Quinte diminuée avec appoggiature\n"
            "\tV/II    : Dominante du deuxième degré\n"
            "\tV/III   : Dominante du troisième degré\n"
            "\tV/IV    : Dominante du quatrième degré\n"
            "\tV/V     : Dominante du cinquième degré\n"
            "\tV/VI    : Dominante du sixième degré\n"
            "\tV/VII   : Dominante du septième degré\n"
            "\tbII     : Deuxième degré abaissé\n"
            "\t6te_aug : Sixte augmentée\n"
            "\nExemple : I VI V/V Vda V I"
        );
        
        progressionLabel->setTooltip(progressionTooltip);
        progressionInput->setTooltip(progressionTooltip);
        
        // Configuration du label et input pour les états
        addAndMakeVisible(*progressionStateLabel);
        addAndMakeVisible(*progressionStateInput);
        
        progressionStateLabel->setText(juce::String::fromUTF8("États:"), juce::dontSendNotification);
        progressionStateLabel->setJustificationType(juce::Justification::right);
        progressionStateInput->setMultiLine(false);
        progressionStateInput->setTextToShowWhenEmpty(juce::String::fromUTF8("Entrez les états (ex: Fund 1st 2nd Fund Fund Fund) [états séparés par des espaces]"), juce::Colours::grey);
        
        // Message commun pour les deux composants
        juce::String stateTooltip = juce::String::fromUTF8("Les états possibles sont : \n\tFund / 0 (fondamental), \n\t1st / 1 (premier renversement), "
                                 "\n\t2nd / 2 (deuxième renversement), \n\t3rd / 3 (troisième renversement). \n\nExemple : Fund 1st 2nd Fund Fund Fund");
        
        progressionStateLabel->setTooltip(stateTooltip);
        progressionStateInput->setTooltip(stateTooltip);
        
        // Configuration du label et input pour les qualités
        addAndMakeVisible(*chordQualitiesLabel);
        addAndMakeVisible(*chordQualitiesInput);
        
        chordQualitiesLabel->setText(juce::String::fromUTF8("Qualités:"), juce::dontSendNotification);
        chordQualitiesLabel->setJustificationType(juce::Justification::right);
        chordQualitiesInput->setMultiLine(false);
        chordQualitiesInput->setTextToShowWhenEmpty(juce::String::fromUTF8("Entrez les qualités (ex: - m7 9b - dim7 -) [qualités séparées par des espaces]"), juce::Colours::grey);
        
        // Tooltip pour les qualités d'accords
        juce::String qualityTooltip = juce::String::fromUTF8(
            "Les qualités possibles sont : \n\n"
            "- : Qualité par défaut\n"
            "maj : Majeur\n"
            "min : Mineur\n"
            "dim : Diminué\n"
            "aug : Augmenté\n"
            "6+ : Sixte augmentée\n"
            "7 : Septième de dominante\n"
            "maj7 : Septième majeure\n"
            "min7 : Septième mineure\n"
            "dim7 : Septième diminuée\n"
            "ø : Semi-diminué\n"
            "minmaj7 : Mineur-majeur septième\n"
            "9 : Neuvième majeure dominante\n"
            "9b (9♭) : Neuvième mineure dominante\n"
            "\nExemple : - m7 9 - dim7 -"
        );
        
        chordQualitiesLabel->setTooltip(qualityTooltip);
        chordQualitiesInput->setTooltip(qualityTooltip);
        
        // Ajouter les callbacks
        progressionInput->onTextChange = [this]() { updateProgressionData(); };
        progressionStateInput->onTextChange = [this]() { updateProgressionData(); };
        chordQualitiesInput->onTextChange = [this]() { updateProgressionData(); };
    }
    
    ~ProgressionPanel() override = default;
    
    void paint(juce::Graphics& g) override {
        // Optionnel : dessin spécifique pour ce panneau
    }
    
    void resized() override {
        auto area = getLocalBounds().reduced(15);
        
        // Zone de progression
        auto progressionArea = area.removeFromTop(35);
        progressionLabel->setBounds(progressionArea.removeFromLeft(90));
        progressionInput->setBounds(progressionArea);

        area.removeFromTop(15);  // Espace entre les composants

        // Zone des états de progression
        auto progressionStateArea = area.removeFromTop(35);
        progressionStateLabel->setBounds(progressionStateArea.removeFromLeft(90));
        progressionStateInput->setBounds(progressionStateArea);

        area.removeFromTop(15);  // Espace entre les composants

        // Zone des qualités d'accords
        auto qualitiesArea = area.removeFromTop(35);
        chordQualitiesLabel->setBounds(qualitiesArea.removeFromLeft(90));
        chordQualitiesInput->setBounds(qualitiesArea);
    }
    
    Progression getProgressionData() const {
        return currentProgression;
    }
    
    // Réinitialisation des champs
    void clearInputs() {
        progressionInput->setText("", false);
        progressionStateInput->setText("", false);
        chordQualitiesInput->setText("", false);
    }
    
    // Callback pour les changements dans la progression
    std::function<void(const Progression&)> onProgressionChanged;
    
private:
    void updateProgressionData() {
        // Stocker les entrées sans validation croisée
        Progression tempProgression;
        
        // Analyser la progression si présente
        if (!progressionInput->getText().isEmpty()) {
            tempProgression.parseProgressionString(progressionInput->getText());
        }
        
        // Analyser les états si présents
        if (!progressionStateInput->getText().isEmpty()) {
            tempProgression.parseStatesString(progressionStateInput->getText());
        }
        
        // Analyser les qualités si présentes
        if (!chordQualitiesInput->getText().isEmpty()) {
            tempProgression.parseQualitiesString(chordQualitiesInput->getText());
        }
        
        // Mettre à jour la progression courante sans validation
        currentProgression = tempProgression;
        
        // Notifier du changement
        if (onProgressionChanged) {
            onProgressionChanged(currentProgression);
        }
    }
    
    std::unique_ptr<juce::Label> progressionLabel;
    std::unique_ptr<juce::TextEditor> progressionInput;
    std::unique_ptr<juce::Label> progressionStateLabel;
    std::unique_ptr<juce::TextEditor> progressionStateInput;
    std::unique_ptr<juce::Label> chordQualitiesLabel;
    std::unique_ptr<juce::TextEditor> chordQualitiesInput;
    
    Progression currentProgression;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionPanel)
}; 