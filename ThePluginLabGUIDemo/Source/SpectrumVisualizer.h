#pragma once

#include <JuceHeader.h>

// Forward declaration only, no impl details needed in this header
class BlockComponent;

// Forward declare the needed structure for connections
namespace CenterCanvasTypes {
    struct BlockConnection {
        BlockComponent* sourceBlock = nullptr;
        BlockComponent* destBlock = nullptr;
    };
}

//==============================================================================
/**
 * A spectrum visualizer component that displays an EQ curve based on
 * frequency, Q, and gain values received from connected blocks.
 */
class SpectrumVisualizer : public juce::Component,
                          public juce::Timer
{
public:
    //==============================================================================
    SpectrumVisualizer();
    ~SpectrumVisualizer() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Timer callback to animate the display
    void timerCallback() override;
    
    // Methods to update parameters from connected blocks
    void setFrequency(float newFrequency);
    void setQFactor(float newQ);
    void setGain(float newGain);
    
    // Method to add filter bands
    void addFilterBand(float freq, float q, float gain);
    void clearFilterBands();
    
private:
    //==============================================================================
    struct FilterBand
    {
        float frequency = 1000.0f;  // Hz
        float q = 1.0f;
        float gain = 0.0f;          // dB
        bool isActive = true;
    };
    
    // Container for multiple filter bands
    juce::Array<FilterBand> filterBands;
    
    // Default parameters if none connected
    float frequency = 1000.0f;     // Default: 1kHz
    float qFactor = 1.0f;          // Default: Q of 1.0
    float gain = 0.0f;             // Default: 0 dB gain
    
    // Frequency range for display
    const float minFrequency = 20.0f;    // 20 Hz
    const float maxFrequency = 20000.0f; // 20 kHz
    
    // Gain range for display
    const float minGain = -24.0f;  // -24 dB
    const float maxGain = 24.0f;   // +24 dB
    
    // Convert frequency to x-coordinate
    float freqToX(float freq, float width) const;
    
    // Convert gain to y-coordinate
    float gainToY(float gainDB, float height) const;
    
    // Calculate frequency response at a specific frequency
    float calculateResponse(float freq);
    
    // Draw the frequency response curve
    void drawResponseCurve(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    
    // Draw the frequency scale
    void drawFrequencyScale(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    
    // Draw the gain scale
    void drawGainScale(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    
    // Utilities for drawing
    juce::Path createRoundedTop(juce::Rectangle<float> bounds, float cornerSize);
    void drawBackgroundGrid(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    
    // Animation
    float animationProgress = 0.0f;
    FilterBand previousBand;
    FilterBand targetBand;
    bool isAnimating = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumVisualizer)
};
