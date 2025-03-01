#pragma once
#include <JuceHeader.h>

class PluginAudioProcessor : public juce::AudioProcessor
{
public:
    PluginAudioProcessor()
    {
        // Initialize processor state
    }
    
    virtual ~PluginAudioProcessor() = default;
    
    // Create a default implementation for AudioProcessor methods
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return "Default"; }
    void changeProgramName(int index, const juce::String& newName) override {}
    
    bool isMidiEffect() const override { return false; }
    
    // Add common methods for our plugin processors
    void setBypassed(bool shouldBeBypassed) { bypassed = shouldBeBypassed; }
    bool isBypassed() const { return bypassed; }
    
    // Add level tracking methods
    float getInputLevel() const { return inputLevel.load(); }
    float getOutputLevel() const { return outputLevel.load(); }
    
protected:
    bool bypassed = false;
    std::atomic<float> inputLevel{-60.0f};
    std::atomic<float> outputLevel{-60.0f};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};
