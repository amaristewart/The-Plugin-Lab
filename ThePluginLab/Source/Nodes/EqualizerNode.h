#pragma once
#include "../Components/PluginNodeComponent.h"
#include "../Visualizers/EQVisualizer.h"
#include "../Audio/Processors/PluginAudioProcessor.h"

class EqualizerNode : public PluginNodeComponent,
                     private juce::ComboBox::Listener,
                     private juce::Slider::Listener,
                     private juce::Button::Listener
{
public:
    EqualizerNode();
    ~EqualizerNode() override;
    
    NodeType getType() const override { return NodeType::Equalizer; }
    juce::AudioProcessor* getProcessor() override { return processor.get(); }

private:
    // Change processBlock to match JUCE's AudioProcessor interface
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

    std::unique_ptr<juce::ComboBox> typeSelector;
    std::unique_ptr<juce::ToggleButton> bypassButton;
    std::unique_ptr<EQVisualizer> visualizer;
    std::unique_ptr<PluginAudioProcessor> processor;

    // Listener interface implementations
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    // Internal methods
    void typeChanged();
    void updateParametersForType(EQType type);
    void updateProcessor();
    void setBypassState(bool shouldBeBypassed);
    void updateParameters();
    void resized() override;
    void updateMeters() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizerNode)
};
