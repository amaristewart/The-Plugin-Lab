#pragma once
#include <JuceHeader.h>
#include "../Components/PluginNodeComponent.h"
#include "../Connections/AudioConnectionPoint.h"

// Forward declarations
class AudioConnectionPoint;

/**
 * GUI node component
 */
class GuiNode : public PluginNodeComponent
{
public:
    // Default constructor without ComponentType parameter
    GuiNode();
    ~GuiNode();
    
    // Type and processor implementations
    NodeType getType() const override;
    juce::AudioProcessor* getProcessor() override;
    
    // Connection point implementations
    void addInputPort(const juce::String& name) override;
    void addOutputPort(const juce::String& name) override;
    
    // Port accessor implementations
    AudioConnectionPoint* getInputPort(int index) const override;
    AudioConnectionPoint* getOutputPort(int index) const override;
    
    // GUI specific methods
    void setControlType(int controlType);
    int getControlType() const;
    
    // Value handling
    void setValue(float value);
    float getValue() const;
    
    // Setup and component methods
    void setupNode();
    
    // Override Component methods
    void resized() override;
    void paint(juce::Graphics& g) override;
    
private:
    // GUI node specific data
    std::unique_ptr<juce::AudioProcessor> processor;
    juce::OwnedArray<AudioConnectionPoint> inputPorts;
    juce::OwnedArray<AudioConnectionPoint> outputPorts;
    
    // GUI components
    juce::Slider valueSlider;
    
    // Control parameters
    int controlType;
    float value;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuiNode)
};
