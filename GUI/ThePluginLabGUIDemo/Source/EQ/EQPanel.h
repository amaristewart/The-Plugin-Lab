#pragma once
#include <JuceHeader.h>
#include "EQComponents.h"

class EQPanel : public juce::Component
{
public:
    EQPanel();
    ~EQPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Public methods to show specific components
    void showFrequencyComponent() { showComponent(&frequencyComponent); }
    void showGainComponent() { showComponent(&gainComponent); }
    void showQComponent() { showComponent(&qComponent); }
    void showHighpassComponent() { showComponent(&highpassComponent); }
    void showLowpassComponent() { showComponent(&lowpassComponent); }
    void showPeakingComponent() { showComponent(&peakingComponent); }

private:
    // EQ component instances
    FrequencyComponent frequencyComponent;
    GainComponent gainComponent;
    QComponent qComponent;
    HighpassComponent highpassComponent;
    LowpassComponent lowpassComponent;
    PeakingComponent peakingComponent;

    // Current selected component
    juce::Component* currentComponent = nullptr;

    // Helper methods to switch between components
    void showComponent(juce::Component* componentToShow);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQPanel)
};
