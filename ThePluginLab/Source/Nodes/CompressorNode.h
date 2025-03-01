#pragma once

#include <JuceHeader.h>
#include "../Audio/AudioPort.h"
#include "../Visualizers/CompressorVisualizer.h"
#include "../Components/PluginNodeComponent.h"
#include "../Audio/Processors/CompressorProcessor.h"
#include "../Common/Types.h"

class CompressorNode : public PluginNodeComponent,
                      private juce::Slider::Listener,
                      private juce::Button::Listener,
                      private juce::ComboBox::Listener
{
public:
    CompressorNode();
    ~CompressorNode() override;

    // Component overrides
    void paint(juce::Graphics&) override;
    void resized() override;

    // PluginNodeComponent pure virtual implementation
    NodeType getType() const override { return NodeType::Compressor; }
    juce::AudioProcessor* getProcessor() override { return processor.get(); }

    // Override virtual methods with matching signatures
    void addInputPort(const juce::String& name) override;
    void addOutputPort(const juce::String& name) override;
    void addParameter(const juce::String& name, float min, float max, float defaultValue) override;

protected:
    // This is protected in the base class, so should be protected here too
    void updateMeters() override;

private:
    // JUCE Listener implementations
    void comboBoxChanged(juce::ComboBox*) override;
    void sliderValueChanged(juce::Slider*) override;
    void buttonClicked(juce::Button*) override;

    // Helper methods
    void typeChanged();
    void updateParametersForType(DynamicType type);
    void updateProcessor();
    void updateParameters();
    void setBypassState(bool shouldBeBypassed);
    void setupParameters();

    // Member variables
    std::unique_ptr<CompressorProcessor> processor{ new CompressorProcessor() };
    std::unique_ptr<juce::ComboBox> typeSelector{ new juce::ComboBox() };
    std::unique_ptr<juce::ToggleButton> bypassButton{ new juce::ToggleButton("Bypass") };
    std::unique_ptr<CompressorVisualizer> visualizer{ new CompressorVisualizer() };
    juce::OwnedArray<AudioPort> inputPorts;
    juce::OwnedArray<AudioPort> outputPorts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorNode)
};
