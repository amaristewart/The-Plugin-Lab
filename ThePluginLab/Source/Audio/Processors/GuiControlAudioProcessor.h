#pragma once
#include <JuceHeader.h>

/**
 * Simple audio processor for GUI control nodes
 */
class GuiControlAudioProcessor : public juce::AudioProcessor
{
public:
    GuiControlAudioProcessor();
    ~GuiControlAudioProcessor() override;
    
    // AudioProcessor overrides
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    
    const juce::String getName() const override { return "GUI Control"; }
    
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
    // Set/get control value
    void setValue(float newValue);
    float getValue() const;
    
private:
    // Control parameters
    float controlValue;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuiControlAudioProcessor)
};
