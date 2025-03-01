#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

// Forward declaration
class LevelMeter;

class CompressorVisualizer : public juce::Component,
                            public juce::Timer
{
public:
    CompressorVisualizer();
    ~CompressorVisualizer() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    // Public setters
    void setType(DynamicType t);
    void setThreshold(float t);
    void setRatio(float r);
    void setLevels(float inLevel, float outLevel);
    void setGainReduction(float reduction);

private:
    // UI drawing methods
    void drawDbScale(juce::Graphics& g);
    void drawTransferFunction(juce::Graphics& g);
    void drawGainReductionMeter(juce::Graphics& g);
    void drawLevelMeters(juce::Graphics& g);

    // Utility methods
    float processGain(float input);
    float inputToX(float db);
    float dbToY(float db);
    
    // State
    DynamicType type = DynamicType::Compressor;
    float threshold = -20.0f;
    float ratio = 4.0f;
    
    // Smoothed values
    juce::SmoothedValue<float> inputLevel;
    juce::SmoothedValue<float> outputLevel;
    juce::SmoothedValue<float> smoothedGainReduction;
    
    // Meters
    std::unique_ptr<LevelMeter> inputMeter;
    std::unique_ptr<LevelMeter> outputMeter;
    std::unique_ptr<LevelMeter> reductionMeter;
    
    // Waveform displays
    std::unique_ptr<juce::Component> inputWaveform;
    std::unique_ptr<juce::Component> outputWaveform;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorVisualizer)
};

// Simple level meter class
class LevelMeter : public juce::Component
{
public:
    LevelMeter(juce::Colour color) : meterColor(color) {}
    
    void paint(juce::Graphics& g) override 
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::darkgrey);
        g.fillRect(bounds);
        
        g.setColour(meterColor);
        g.fillRect(bounds.withHeight(bounds.getHeight() * level));
    }
    
    void setLevel(float newLevel) { level = juce::jlimit(0.0f, 1.0f, newLevel); repaint(); }
    
private:
    float level = 0.0f;
    juce::Colour meterColor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
