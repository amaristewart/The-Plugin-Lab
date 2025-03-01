#include "ConnectionComponent.h"
#include "../Connections/AudioConnectionPoint.h"

ConnectionComponent::ConnectionComponent(AudioConnectionPoint* source, AudioConnectionPoint* dest)
    : sourcePoint(source),
      destPoint(dest)
{
    setAlwaysOnTop(true);
    updatePosition();
}

ConnectionComponent::~ConnectionComponent()
{
    
}

void ConnectionComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    
    auto startPoint = getLocalPoint(sourcePoint, sourcePoint->getBounds().getCentre().toFloat());
    auto endPoint = getLocalPoint(destPoint, destPoint->getBounds().getCentre().toFloat());
    
    g.drawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, 2.0f);
}

void ConnectionComponent::resized()
{
    updatePosition();
}

void ConnectionComponent::updatePosition()
{
    if (sourcePoint == nullptr || destPoint == nullptr)
        return;
        
    auto startPoint = sourcePoint->getBounds().getCentre();
    auto endPoint = destPoint->getBounds().getCentre();
    
    auto parentComponent = getParentComponent();
    if (parentComponent != nullptr)
    {
        startPoint = sourcePoint->localPointToGlobal(startPoint);
        endPoint = destPoint->localPointToGlobal(endPoint);
        startPoint = parentComponent->getLocalPoint(nullptr, startPoint);
        endPoint = parentComponent->getLocalPoint(nullptr, endPoint);
    }
    
    auto bounds = juce::Rectangle<int>::leftTopRightBottom(
        juce::jmin(startPoint.x, endPoint.x) - 5,
        juce::jmin(startPoint.y, endPoint.y) - 5,
        juce::jmax(startPoint.x, endPoint.x) + 5,
        juce::jmax(startPoint.y, endPoint.y) + 5);
        
    setBounds(bounds);
    repaint();
}

PluginNodeComponent* ConnectionComponent::getSourceNode() const
{
    return &sourcePoint->getOwner();
}

int ConnectionComponent::getSourceChannel() const
{
    return sourcePoint->getPortIndex();
}

PluginNodeComponent* ConnectionComponent::getDestinationNode() const
{
    return &destPoint->getOwner();
}

int ConnectionComponent::getDestinationChannel() const
{
    return destPoint->getPortIndex();
}
