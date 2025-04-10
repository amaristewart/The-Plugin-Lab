#include "AudioConnectionPoint.h"

namespace Components {

AudioConnectionPoint::AudioConnectionPoint(PortType type, const juce::String& name)
    : portType(type), portName(name)
{
    // Constructor implementation
}

AudioConnectionPoint::~AudioConnectionPoint()
{
    // Destructor implementation
}

void AudioConnectionPoint::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(2);
    
    juce::Colour portColour = juce::Colours::white;
    if (portType == PortType::AudioInput)
        portColour = juce::Colours::green;
    else if (portType == PortType::AudioOutput)
        portColour = juce::Colours::red;
    
    g.setColour(portColour);
    g.fillEllipse(bounds.toFloat());
    
    g.setColour(juce::Colours::black);
    g.drawEllipse(bounds.toFloat(), 1.0f);
}

void AudioConnectionPoint::resized()
{
    // Resized implementation
}

PortType AudioConnectionPoint::getPortType() const
{
    return portType;
}

juce::String AudioConnectionPoint::getName() const
{
    return portName;
}

void AudioConnectionPoint::mouseDown(const juce::MouseEvent& e)
{
    // Implementation for mouse down
}

void AudioConnectionPoint::mouseDrag(const juce::MouseEvent& e)
{
    // Implementation for mouse drag
}

void AudioConnectionPoint::mouseUp(const juce::MouseEvent& e)
{
    // Implementation for mouse up
}

} // namespace Components