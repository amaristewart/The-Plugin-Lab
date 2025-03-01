#pragma once

#include <JuceHeader.h>
#include "PluginAudioProcessor.h"
#include "../../Common/Types.h"

class CompressorProcessor : public PluginAudioProcessor
{
public:
    CompressorProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    // Plugin info
    const juce::String getName() const override { return "Compressor"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    // State management
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Dynamics parameters
    void setDynamicsType(DynamicType type);
    void setThreshold(float thresholdDb);
    void setRatio(float ratio);
    void setAttackTime(float attackMs);
    void setReleaseTime(float releaseMs);
    void setHoldTime(float holdMs);
    void setBypassed(bool shouldBeBypassed);

    float getGainReduction() const;

private:
    // Different dynamics processing methods
    void processCompressor(juce::AudioBuffer<float>& buffer);
    void processLimiter(juce::AudioBuffer<float>& buffer);
    void processExpander(juce::AudioBuffer<float>& buffer);
    void processGate(juce::AudioBuffer<float>& buffer);

    // Parameters
    DynamicType currentType = DynamicType::Compressor;
    float threshold = -20.0f;         // dB
    float ratio = 4.0f;               // compression ratio
    float attackTime = 10.0f;         // ms
    float releaseTime = 100.0f;       // ms
    float holdTime = 0.0f;            // ms (for gate)

    // Initial state
    double currentSampleRate = 44100.0;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float envelopeLevel = 0.0f;
    float currentGainReduction = 1.0f;  // linear gain factor
    int holdCounter = 0;
    int maxHoldSamples = 0;
    std::atomic<float> gainReduction{0.0f};  // dB for metering

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorProcessor)
};
