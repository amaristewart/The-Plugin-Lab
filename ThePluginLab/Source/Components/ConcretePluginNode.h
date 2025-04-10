#pragma once
#include <JuceHeader.h>
#include "PluginNodeComponent.h"
#include "../Common/Types.h"

class AudioConnectionPoint;

/**
 * A concrete implementation of the PluginNodeComponent
 */
class ConcretePluginNode : public PluginNodeComponent
{
public:
    ConcretePluginNode(const juce::String& name, juce::Colour color);
    ~ConcretePluginNode() override;
    
    // Type and processor implementations
    NodeType getType() const override;
    juce::AudioProcessor* getProcessor() override;
    
    // Connection point implementations
    void addInputPort(const juce::String& name) override;
    void addOutputPort(const juce::String& name) override;
    
    // Port accessor implementations
    AudioConnectionPoint* getInputPort(int index) const override;
    AudioConnectionPoint* getOutputPort(int index) const override;
    
    // Layout methods
    void resized() override;
    void paint(juce::Graphics& g) override;
    
protected:
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    
private:
    // Internal data for the concrete node
    std::unique_ptr<juce::AudioProcessor> processor;
    juce::OwnedArray<AudioConnectionPoint> inputPorts;
    juce::OwnedArray<AudioConnectionPoint> outputPorts;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConcretePluginNode)
};
