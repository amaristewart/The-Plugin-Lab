#include "GuiNode.h"
#include "../Common/Features.h"
#include "../Connections/AudioConnectionPoint.h"
#include "../Audio/Processors/GuiControlAudioProcessor.h"

GuiNode::GuiNode()
    : PluginNodeComponent("GUI Control", Features::guiColor),
      controlType(0),
      value(0.0f)
{
    setupNode();
}

GuiNode::~GuiNode()
{
    // Clean up resources
    inputPorts.clear();
    outputPorts.clear();
}

void GuiNode::setupNode()
{
    // Create a processor for this node
    processor = std::make_unique<GuiControlAudioProcessor>();
    
    // Add standard ports
    addInputPort("Input");
    addOutputPort("Output");
    
    // Set up any GUI components here
    // For example:
    addAndMakeVisible(valueSlider);
    valueSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    valueSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    valueSlider.setRange(0.0, 1.0);
    valueSlider.setValue(0.5);
    
    // Update layout
    resized();
}

NodeType GuiNode::getType() const
{
    return NodeType::GuiControl;
}

juce::AudioProcessor* GuiNode::getProcessor()
{
    return processor.get();
}

void GuiNode::addInputPort(const juce::String& name)
{
    // Use proper enum values for direction
    auto port = new AudioConnectionPoint(*this, AudioConnectionPoint::PortDirection::Input, inputPorts.size());
    port->setName(name);
    inputPorts.add(port);
    addAndMakeVisible(port);
    resized();
}

void GuiNode::addOutputPort(const juce::String& name)
{
    // Use proper enum values for direction
    auto port = new AudioConnectionPoint(*this, AudioConnectionPoint::PortDirection::Output, outputPorts.size());
    port->setName(name);
    outputPorts.add(port);
    addAndMakeVisible(port);
    resized();
}

AudioConnectionPoint* GuiNode::getInputPort(int index) const
{
    if (index >= 0 && index < inputPorts.size())
        return inputPorts[index];
    return nullptr;
}

AudioConnectionPoint* GuiNode::getOutputPort(int index) const
{
    if (index >= 0 && index < outputPorts.size())
        return outputPorts[index];
    return nullptr;
}

void GuiNode::setControlType(int newControlType)
{
    controlType = newControlType;
    // Update the UI based on control type
    resized();
}

int GuiNode::getControlType() const
{
    return controlType;
}

void GuiNode::setValue(float newValue)
{
    value = newValue;
    // Update the processor with the new value
    if (auto* guiProcessor = dynamic_cast<GuiControlAudioProcessor*>(processor.get()))
    {
        guiProcessor->setValue(value);
    }
}

float GuiNode::getValue() const
{
    return value;
}

void GuiNode::resized()
{
    // Layout the node
    auto bounds = getLocalBounds().reduced(4);
    
    // Position the input ports
    int portY = 40;
    for (auto* port : inputPorts)
    {
        port->setBounds(0, portY, 20, 20);
        portY += 25;
    }
    
    // Position the output ports
    portY = 40;
    for (auto* port : outputPorts)
    {
        port->setBounds(getWidth() - 20, portY, 20, 20);
        portY += 25;
    }
    
    // Position the GUI controls
    auto controlBounds = bounds.reduced(25, 40);
    valueSlider.setBounds(controlBounds);
}

void GuiNode::paint(juce::Graphics& g)
{
    // Use the base class paint method for the basic node look
    PluginNodeComponent::paint(g);
    
    // Add any additional GUI painting here
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Control Type: " + juce::String(controlType), 
               getLocalBounds().reduced(8), 
               juce::Justification::topLeft, true);
}
