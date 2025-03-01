#pragma once
#include <JuceHeader.h>
#include "../Components/PluginNodeComponent.h"

class GuiNode : public PluginNodeComponent
{
public:
    GuiNode(ComponentType type);
    ~GuiNode() override = default;
    
    NodeType getType() const override { return NodeType::GUI; }
    juce::AudioProcessor* getProcessor() override { return nullptr; }
    
    void resized() override;
    void paint(juce::Graphics& g) override;
    
private:
    void setupForType();
    void addKnob();
    void addSlider();
    void addButton();
    void addLabel();
    
    ComponentType componentType;
    std::unique_ptr<juce::Component> control;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuiNode)
};
