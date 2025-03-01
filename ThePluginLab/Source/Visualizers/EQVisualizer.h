#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

class EQVisualizer : public juce::Component,
                    public juce::Timer
{
public:
    EQVisualizer();
    ~EQVisualizer() override;

    void paint(juce::Graphics& g) override;
    void setType(EQType type);
    void setFrequency(float freq);
    void setGain(float gainDb);
    void setQ(float qValue);
    void setInputLevel(float level);
    void setOutputLevel(float level);
    
    // Add this method to handle audio sample visualization
    void pushSample(float inputSample, float outputSample);

private:
    void timerCallback() override;
    void drawFrequencyScale(juce::Graphics& g);
    void drawFrequencyResponse(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void drawLevelMeters(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    float xToFreq(float x, float width) const;
    float freqToX(float freq, float width) const;
    float dbToY(float db, float height) const;
    float yToDb(float y, float height) const;

    EQType currentType = EQType::Lowpass;
    float frequency = 1000.0f;
    float gain = 0.0f;
    float q = 0.707f;
    float inputLevel = -60.0f;
    float outputLevel = -60.0f;
    
    // Buffers to store samples for visualization
    juce::AudioBuffer<float> inputBuffer;
    juce::AudioBuffer<float> outputBuffer;
    int bufferPosition = 0;
    const int bufferSize = 1024;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQVisualizer)
};
