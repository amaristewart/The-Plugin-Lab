#pragma once
#include <JuceHeader.h>
#include "../Components/PluginNodeComponent.h"

// Forward declarations
class AudioConnectionPoint;

/**
 * Compressor node component 
 */
class CompressorNode : public PluginNodeComponent
{
public:
    CompressorNode();
    ~CompressorNode();
    
    // Type and processor implementations
    NodeType getType() const override;
    juce::AudioProcessor* getProcessor() override;
    
    // Connection point implementations
    void addInputPort(const juce::String& name) override;
    void addOutputPort(const juce::String& name) override;
    
    // Port accessor implementations
    AudioConnectionPoint* getInputPort(int index) const override;
    AudioConnectionPoint* getOutputPort(int index) const override;
    
    // Compressor-specific methods
    void setThreshold(float threshold);
    float getThreshold() const;
    
    void setRatio(float ratio);
    float getRatio() const;
    
    void setAttack(float attack);
    float getAttack() const;
    
    void setRelease(float release);
    float getRelease() const;
    
    // Component methods
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    // Compressor-specific data
    std::unique_ptr<juce::AudioProcessor> processor;
    juce::OwnedArray<AudioConnectionPoint> inputPorts;
    juce::OwnedArray<AudioConnectionPoint> outputPorts;
    
    // Controls
    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    
    // Parameters
    float threshold = -12.0f;
    float ratio = 4.0f;
    float attack = 10.0f;
    float release = 100.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorNode)
};
