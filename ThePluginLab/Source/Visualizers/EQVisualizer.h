#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

// Forward declaration
class EqualizerProcessor;

/**
 * Visualizer component for equalizer nodes
 */
class EQVisualizer : public juce::Component
{
public:
    EQVisualizer();
    ~EQVisualizer() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Update the frequency response curve
    void updateCurve(const juce::Array<float>& response);
    
private:
    juce::Array<float> frequencyResponse;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQVisualizer)
};

// Define EqualizerVisualizer as EQVisualizer for compatibility
typedef EQVisualizer EqualizerVisualizer;
