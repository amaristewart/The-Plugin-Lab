#pragma once
#include <JuceHeader.h>
#include "PluginAudioProcessor.h"
#include "../../Common/Types.h"

class EqualizerBand
{
public:
    EqualizerBand() : frequency(1000.0f), gain(1.0f), q(0.707f), currentSampleRate(44100.0) {}
    
    void prepare(double sampleRate);
    void processBlock(float* data, int numSamples);
    
    void setFrequency(float newFreq);
    void setGain(float newGain);
    void setQ(float newQ);
    
    float getFrequencyResponse(float frequency) const;
    
private:
    float frequency;
    float gain;
    float q;
    double currentSampleRate;
    
    float b0, b1, b2, a1, a2;
    float x1, x2, y1, y2;
    
    void updateCoefficients();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizerBand)
};

class EqualizerProcessor : public PluginAudioProcessor
{
public:
    EqualizerProcessor();

    // AudioProcessor overrides
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    // Plugin info
    const juce::String getName() const override { return "Equalizer"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    // State management
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter handling
    void setFilterType(EQType type);
    void setFrequency(float freq);
    void setGain(float gainDb);
    void setQ(float q);

private:
    EQType filterType = EQType::Peaking;
    float frequency = 1000.0f;
    float gain = 0.0f;
    float quality = 1.0f;
    
    double sampleRate = 44100.0;
    
    void updateCoefficients();
    std::array<float, 5> coefficients;  // b0, b1, b2, a1, a2
    std::array<float, 4> state;         // x1, x2, y1, y2

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizerProcessor)
};
