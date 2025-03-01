#include "PluginNodeComponent.h"
#include "../Connections/AudioConnectionPoint.h"
#include "../GUI/PluginEditorCanvas.h"
#include "ConnectionComponent.h"

juce::Array<AudioConnectionPoint*> inputPorts;
juce::Array<AudioConnectionPoint*> outputPorts;

PluginNodeComponent::PluginNodeComponent(const juce::String& name, const juce::Colour& color)
    : nodeName(name)
    , nodeColor(color)
{
    setSize(200, 150);
}

void PluginNodeComponent::createConnectionPoints()
{
    // Input point on left
    auto* input = new AudioConnectionPoint(*this, ConnectionPointType::Input, 0);
    addAndMakeVisible(input);
    inputPorts.add(input);
    
    // Output point on right
    auto* output = new AudioConnectionPoint(*this, ConnectionPointType::Output, 0);
    addAndMakeVisible(output);
    outputPorts.add(output);
}

void PluginNodeComponent::paint(juce::Graphics& g)
{
    // Node background
    g.setColour(nodeColor);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
    
    // Border
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 10.0f, 1.0f);
    
    // Title
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText(nodeName, getLocalBounds().reduced(5),
               juce::Justification::centredTop, true);
}

void PluginNodeComponent::resized()
{
    // Connection points
    const int pointSize = 10;
    
    for (auto* point : inputPorts)
    {
        point->setBounds(0, getHeight() / 2 - pointSize / 2, 
                        pointSize, pointSize);
    }
    
    for (auto* point : outputPorts)
    {
        point->setBounds(getWidth() - pointSize, getHeight() / 2 - pointSize / 2,
                        pointSize, pointSize);
    }
    
    // Parameter sliders
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    
    for (auto* slider : parameterSliders)
    {
        slider->setBounds(bounds.removeFromTop(30));
        bounds.removeFromTop(5); // Spacing
    }
}

void PluginNodeComponent::mouseDown(const juce::MouseEvent& e)
{
    dragger.startDraggingComponent(this, e);
}

void PluginNodeComponent::mouseDrag(const juce::MouseEvent& e)
{
    dragger.dragComponent(this, e, &constrainer);
    repaint();
}

void PluginNodeComponent::mouseUp(const juce::MouseEvent& /*e*/)
{
    isDragging = false;
}

void PluginNodeComponent::addConnectionPoint(AudioConnectionPoint* point)
{
    if (point != nullptr)
        connectionPoints.add(point);
}

void PluginNodeComponent::removeConnectionPoint(AudioConnectionPoint* point)
{
    connectionPoints.remove(connectionPoints.indexOf(point));
}

void PluginNodeComponent::layoutParameters()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    
    for (auto* slider : parameterSliders)
    {
        slider->setBounds(bounds.removeFromTop(30));
        bounds.removeFromTop(5); // Spacing
    }
}

juce::Point<float> PluginNodeComponent::getInputPosition(int index) const
{
    if (index < inputPorts.size())
        return inputPorts[index]->getBounds().getCentre().toFloat();
    return {};
}

juce::Point<float> PluginNodeComponent::getOutputPosition(int index) const
{
    if (index < outputPorts.size())
        return outputPorts[index]->getBounds().getCentre().toFloat();
    return {};
}

bool PluginNodeComponent::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& details)
{
    // Accept drops from other nodes or connection points
    auto* sourceComponent = dynamic_cast<PluginNodeComponent*>(details.sourceComponent.get());
    auto* connectionPoint = dynamic_cast<AudioConnectionPoint*>(details.sourceComponent.get());
    
    return sourceComponent != nullptr || connectionPoint != nullptr;
}

void PluginNodeComponent::itemDropped(const juce::DragAndDropTarget::SourceDetails& details)
{
    // Handle connection creation
    if (auto* connectionPoint = dynamic_cast<AudioConnectionPoint*>(details.sourceComponent.get()))
    {
        // Create connection if compatible
        if (auto* canvas = dynamic_cast<PluginEditorCanvas*>(getParentComponent()))
        {
            if (connectionPoint->getType() == ConnectionPointType::Output)
            {
                // Connect output to our input
                canvas->addConnection(connectionPoint, inputPorts[0]);
            }
            else
            {
                // Connect our output to their input
                canvas->addConnection(outputPorts[0], connectionPoint);
            }
        }
    }
}

void PluginNodeComponent::addInputPort(const juce::String& name)
{
    auto* port = new AudioConnectionPoint(*this, ConnectionPointType::Input, inputPorts.size());
    addAndMakeVisible(port);
    inputPorts.add(port);
    resized();
}

void PluginNodeComponent::addOutputPort(const juce::String& name)
{
    auto* port = new AudioConnectionPoint(*this, ConnectionPointType::Output, outputPorts.size());
    addAndMakeVisible(port);
    outputPorts.add(port);
    resized();
}

void PluginNodeComponent::addParameter(const juce::String& name, float min, float max, float defaultValue)
{
    auto* slider = new juce::Slider(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight);
    slider->setName(name);
    slider->setRange(min, max);
    slider->setValue(defaultValue, juce::dontSendNotification);
    addAndMakeVisible(slider);
    parameterSliders.add(slider);
    resized();
}

void PluginNodeComponent::layoutControls()
{
    // Default implementation - derived classes can override
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for title
    
    for (auto* param : parameterSliders)
    {
        param->setBounds(bounds.removeFromTop(30));
        bounds.removeFromTop(5); // Spacing
    }
}
