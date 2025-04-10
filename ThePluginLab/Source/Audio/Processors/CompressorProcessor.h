#pragma once

#include <JuceHeader.h>
#include "../../Common/Types.h"

/**
 * Audio processor for compressor effects
 */
class CompressorProcessor : public juce::AudioProcessor
{
public:
    CompressorProcessor();
    ~CompressorProcessor() override;
    
    // AudioProcessor overrides
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    // MIDI handling
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    // Editor
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    // Program handling
    const juce::String getName() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    
    // State handling
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
    // Parameter handling
    void setThreshold(float thresholdDb);
    void setRatio(float ratio);
    void setAttack(float attackMs);
    void setRelease(float releaseMs);
    void setMakeupGain(float gainDb);
    
    // Metering
    float getGainReduction() const;
    
private:
    // Compressor parameters
    float threshold = 0.0f;
    float ratio = 1.0f;
    float attack = 50.0f;
    float release = 200.0f;
    float makeupGain = 0.0f;
    
    // Compressor state
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Gain<float> gain;
    
    // Metering
    float currentGainReduction = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorProcessor)
};
