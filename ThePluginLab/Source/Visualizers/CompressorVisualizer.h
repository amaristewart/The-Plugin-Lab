#pragma once
#include <JuceHeader.h>

/**
 * Visualizer for the compressor node showing compression curve and gain reduction
 */
class CompressorVisualizer : public juce::Component
{
public:
    CompressorVisualizer();
    ~CompressorVisualizer() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Update the visualizer with current compressor settings
    void updateSettings(float threshold, float ratio, float attackTime, float releaseTime);
    
    // Update with current gain reduction amount
    void updateGainReduction(float gainReductionDb);
    
private:
    // Compressor parameters
    float threshold = -20.0f;    // dB
    float ratio = 4.0f;          // ratio
    float attack = 10.0f;        // ms
    float release = 100.0f;      // ms
    float currentGainReduction = 0.0f;  // dB
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorVisualizer)
};
