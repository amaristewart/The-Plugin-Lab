#include "ConcretePluginNode.h"
#include "../Connections/AudioConnectionPoint.h"
#include "../Audio/Processors/BasicAudioProcessor.h"

// Fix constructor to match declaration in header (name -> nodeName, color -> nodeColour)
ConcretePluginNode::ConcretePluginNode(const juce::String& name, juce::Colour color)
    : PluginNodeComponent(name, color)
{
    // Just set processor to nullptr for the GUI prototype
    processor = nullptr;
}

ConcretePluginNode::~ConcretePluginNode()
{
    // Make sure all connection points are removed first
    inputPorts.clear();
    outputPorts.clear();
    processor.reset(); // Explicitly reset the processor
}

NodeType ConcretePluginNode::getType() const
{
    return NodeType::Generic;
}

juce::AudioProcessor* ConcretePluginNode::getProcessor()
{
    // Since we're using BasicAudioProcessor which isn't a juce::AudioProcessor in the GUI prototype,
    // we'll return nullptr here
    return nullptr;
}

void ConcretePluginNode::addInputPort(const juce::String& name)
{
    auto port = new AudioConnectionPoint(*this, AudioConnectionPoint::PortDirection::Input, inputPorts.size());
    port->setName(name);
    inputPorts.add(port);
    addAndMakeVisible(port);
    resized();
}

void ConcretePluginNode::addOutputPort(const juce::String& name)
{
    auto port = new AudioConnectionPoint(*this, AudioConnectionPoint::PortDirection::Output, outputPorts.size());
    port->setName(name);
    outputPorts.add(port);
    addAndMakeVisible(port);
    resized();
}

AudioConnectionPoint* ConcretePluginNode::getInputPort(int index) const
{
    if (index >= 0 && index < inputPorts.size())
        return inputPorts[index];
    return nullptr;
}

AudioConnectionPoint* ConcretePluginNode::getOutputPort(int index) const
{
    if (index >= 0 && index < outputPorts.size())
        return outputPorts[index];
    return nullptr;
}

void ConcretePluginNode::resized()
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
}

void ConcretePluginNode::paint(juce::Graphics& g)
{
    // Draw background with rounded corners and gradient
    auto bounds = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(nodeColor.brighter(0.2f), bounds.getTopLeft(),
                                          nodeColor.darker(0.2f), bounds.getBottomRight(), false));
    g.fillRoundedRectangle(bounds.reduced(1.0f), 10.0f);

    // Draw border
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 10.0f, 2.0f);

    // Draw title bar with gradient
    juce::Rectangle<float> titleBar = bounds.withHeight(30.0f).reduced(1.0f);
    g.setGradientFill(juce::ColourGradient(nodeColor.brighter(0.3f), titleBar.getTopLeft(),
                                          nodeColor, titleBar.getBottomLeft(), false));
    g.fillRoundedRectangle(titleBar, 10.0f);
    
    // Draw title text with shadow
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.setFont(juce::Font(16.0f).boldened());
    g.drawText(nodeName, titleBar.translated(1.0f, 1.0f), juce::Justification::centred);
    
    g.setColour(juce::Colours::white);
    g.drawText(nodeName, titleBar, juce::Justification::centred);

    // Draw node type indicator
    juce::String typeText;
    switch (getType()) {
        case NodeType::Equalizer: typeText = "EQ"; break;
        case NodeType::Compressor: typeText = "COMP"; break;
        case NodeType::GuiControl: typeText = "GUI"; break;
        default: typeText = "GEN"; break;
    }

    g.setFont(10.0f);
    g.drawText(typeText, bounds.reduced(5).removeFromBottom(20.0f), juce::Justification::right);
    
    // Draw connection port labels
    g.setFont(12.0f);
    for (int i = 0; i < inputPorts.size(); ++i) {
        auto* port = inputPorts[i];
        if (port != nullptr) {
            g.setColour(juce::Colours::white);
            g.drawText(port->getName(), 
                      port->getRight() + 5, port->getY(), 
                      getWidth() - port->getRight() - 10, port->getHeight(), 
                      juce::Justification::left);
        }
    }
    
    for (int i = 0; i < outputPorts.size(); ++i) {
        auto* port = outputPorts[i];
        if (port != nullptr) {
            g.setColour(juce::Colours::white);
            auto textWidth = g.getCurrentFont().getStringWidth(port->getName());
            g.drawText(port->getName(), 
                      port->getX() - textWidth - 10, port->getY(), 
                      textWidth + 5, port->getHeight(), 
                      juce::Justification::right);
        }
    }
}

void ConcretePluginNode::mouseDown(const juce::MouseEvent& e)
{
    PluginNodeComponent::mouseDown(e);
}

void ConcretePluginNode::mouseDrag(const juce::MouseEvent& e)
{
    PluginNodeComponent::mouseDrag(e);
}

void ConcretePluginNode::mouseUp(const juce::MouseEvent& e)
{
    PluginNodeComponent::mouseUp(e);
}
