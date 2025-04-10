#include "CompressorNode.h"
#include "../Common/Features.h"
#include "../Connections/AudioConnectionPoint.h"

// Fix for duplicate symbol: Use forward declaration
class CompressorVisualizer;

CompressorNode::CompressorNode()
    : PluginNodeComponent("Compressor", Features::compColor)
{
    // Add ports
    addInputPort("Input L");
    addInputPort("Input R");
    addOutputPort("Output L");
    addOutputPort("Output R");
    
    // Set up sliders
    addAndMakeVisible(thresholdSlider);
    thresholdSlider.setRange(-60.0, 0.0, 0.1);
    thresholdSlider.setValue(threshold);
    thresholdSlider.setTextValueSuffix(" dB");
    thresholdSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(ratioSlider);
    ratioSlider.setRange(1.0, 20.0, 0.1);
    ratioSlider.setValue(ratio);
    ratioSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    ratioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(attackSlider);
    attackSlider.setRange(0.1, 100.0, 0.1);
    attackSlider.setValue(attack);
    attackSlider.setTextValueSuffix(" ms");
    attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(releaseSlider);
    releaseSlider.setRange(10.0, 1000.0, 1.0);
    releaseSlider.setValue(release);
    releaseSlider.setTextValueSuffix(" ms");
    releaseSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    setSize(250, 200);
}

CompressorNode::~CompressorNode()
{
    // Clean up resources
}

NodeType CompressorNode::getType() const
{
    return NodeType::Compressor;
}

juce::AudioProcessor* CompressorNode::getProcessor()
{
    return processor.get();
}

void CompressorNode::addInputPort(const juce::String& name)
{
    auto port = new AudioConnectionPoint(*this, AudioConnectionPoint::PortDirection::Input, inputPorts.size());
    port->setName(name);
    inputPorts.add(port);
    addAndMakeVisible(port);
    resized();
}

void CompressorNode::addOutputPort(const juce::String& name)
{
    auto port = new AudioConnectionPoint(*this, AudioConnectionPoint::PortDirection::Output, outputPorts.size());
    port->setName(name);
    outputPorts.add(port);
    addAndMakeVisible(port);
    resized();
}

AudioConnectionPoint* CompressorNode::getInputPort(int index) const
{
    if (index >= 0 && index < inputPorts.size())
        return inputPorts[index];
    return nullptr;
}

AudioConnectionPoint* CompressorNode::getOutputPort(int index) const
{
    if (index >= 0 && index < outputPorts.size())
        return outputPorts[index];
    return nullptr;
}

void CompressorNode::paint(juce::Graphics& g)
{
    // Base painting
    PluginNodeComponent::paint(g);
    
    // Add compressor-specific UI
    auto bounds = getLocalBounds().reduced(25, 40).withY(50);
    
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    
    auto rowHeight = 60;
    auto row1 = bounds.removeFromTop(rowHeight);
    auto row2 = bounds.removeFromTop(rowHeight);
    
    auto left1 = row1.removeFromLeft(row1.getWidth() / 2);
    auto right1 = row1;
    auto left2 = row2.removeFromLeft(row2.getWidth() / 2);
    auto right2 = row2;
    
    g.drawText("Threshold", left1.withHeight(20), juce::Justification::centred);
    g.drawText("Ratio", right1.withHeight(20), juce::Justification::centred);
    g.drawText("Attack", left2.withHeight(20), juce::Justification::centred);
    g.drawText("Release", right2.withHeight(20), juce::Justification::centred);
}

void CompressorNode::resized()
{
    // Position ports
    int portY = 40;
    for (auto* port : inputPorts)
    {
        port->setBounds(0, portY, 20, 20);
        portY += 25;
    }
    
    portY = 40;
    for (auto* port : outputPorts)
    {
        port->setBounds(getWidth() - 20, portY, 20, 20);
        portY += 25;
    }
    
    // Position controls
    auto bounds = getLocalBounds().reduced(25, 40).withY(70);
    
    auto rowHeight = 60;
    auto row1 = bounds.removeFromTop(rowHeight);
    auto row2 = bounds.removeFromTop(rowHeight);
    
    auto left1 = row1.removeFromLeft(row1.getWidth() / 2);
    auto right1 = row1;
    auto left2 = row2.removeFromLeft(row2.getWidth() / 2);
    auto right2 = row2;
    
    thresholdSlider.setBounds(left1);
    ratioSlider.setBounds(right1);
    attackSlider.setBounds(left2);
    releaseSlider.setBounds(right2);
}

void CompressorNode::setThreshold(float newThreshold)
{
    threshold = newThreshold;
    thresholdSlider.setValue(threshold, juce::dontSendNotification);
}

float CompressorNode::getThreshold() const
{
    return threshold;
}

void CompressorNode::setRatio(float newRatio)
{
    ratio = newRatio;
    ratioSlider.setValue(ratio, juce::dontSendNotification);
}

float CompressorNode::getRatio() const
{
    return ratio;
}

void CompressorNode::setAttack(float newAttack)
{
    attack = newAttack;
    attackSlider.setValue(attack, juce::dontSendNotification);
}

float CompressorNode::getAttack() const
{
    return attack;
}

void CompressorNode::setRelease(float newRelease)
{
    release = newRelease;
    releaseSlider.setValue(release, juce::dontSendNotification);
}

float CompressorNode::getRelease() const
{
    return release;
}
