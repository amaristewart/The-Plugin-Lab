#pragma once
#include <JuceHeader.h>

/**
 * A very simple audio processor that just routes audio through
 * Used for non-processing GUI nodes
 */
class PluginAudioProcessor : public juce::AudioProcessor
{
public:
    PluginAudioProcessor()
        : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
    }
    
    ~PluginAudioProcessor() override {}
    
    // AudioProcessor overrides - minimal implementation
    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override 
    { 
        // Nothing to do - this processor just passes audio through
    }
    
    void releaseResources() override {}
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        // Pass audio through unchanged
        // The buffer already contains the input, so we do nothing
        juce::ScopedNoDenormals noDenormals;
    }
    
    // Editor and name
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    const juce::String getName() const override { return "Plugin Audio Processor"; }
    
    // MIDI handling
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    
    // Program handling
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    
    // State handling
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};
