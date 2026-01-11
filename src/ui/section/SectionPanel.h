#pragma once

#include <JuceHeader.h>
#include "components/OverviewZone/Overview/OverviewActionArea.h"
#include "components/EditZone/ProgressionArea.h"

/** @brief Panneau principal de section contenant l'aperçu (Overview) et la zone d'édition (soit l'éditeur de Progression (section) ou l'éditeur de Modulation). */
class SectionPanel : public juce::Component
{
public:
    SectionPanel();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    OverviewActionArea  overviewActionArea;
    ProgressionArea     progressionArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionPanel)
};
