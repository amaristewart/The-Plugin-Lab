#include "ConnectionComponent.h"
#include "AudioConnectionPoint.h"
#include "../GUI/PluginEditorCanvas.h"

namespace Connections {

ConnectionComponent::ConnectionComponent()
{
    setInterceptsMouseClicks(true, false);
}

ConnectionComponent::~ConnectionComponent()
{
}

void ConnectionComponent::setSourcePoint(AudioConnectionPoint* source)
{
    sourcePoint = source;
    updatePosition();
}

void ConnectionComponent::setDestinationPoint(AudioConnectionPoint* destination)
{
    destinationPoint = destination;
    updatePosition();
}

void ConnectionComponent::updatePosition()
{
    if (sourcePoint == nullptr || destinationPoint == nullptr)
        return;
        
    // Get the bounds of the source and destination points in global coordinates
    juce::Rectangle<int> sourceBounds = sourcePoint->getScreenBounds();
    juce::Rectangle<int> destBounds = destinationPoint->getScreenBounds();
    
    // Convert to local coordinates of our parent
    juce::Point<int> sourceCenter = getParentComponent()->getLocalPoint(nullptr, sourceBounds.getCentre());
    juce::Point<int> destCenter = getParentComponent()->getLocalPoint(nullptr, destBounds.getCentre());
    
    // Create a line between them with some padding for the bounds
    juce::Point<int> topLeft(
        std::min(sourceCenter.x, destCenter.x) - 5,
        std::min(sourceCenter.y, destCenter.y) - 5
    );
    
    juce::Point<int> bottomRight(
        std::max(sourceCenter.x, destCenter.x) + 5,
        std::max(sourceCenter.y, destCenter.y) + 5
    );
    
    // Set our bounds to encompass the connection
    setBounds(juce::Rectangle<int>(topLeft, bottomRight));
    
    // Update the connection path
    connectionPath.clear();
    juce::Point<float> start(sourceCenter.x - getX(), sourceCenter.y - getY());
    juce::Point<float> end(destCenter.x - getX(), destCenter.y - getY());
    
    // Calculate control points for a bezier curve
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    
    // We want the curve to bend based on the horizontal distance
    float bezierOffset = juce::jmin(50.0f, std::abs(dx) * 0.5f);
    
    // Create the bezier path
    connectionPath.startNewSubPath(start);
    connectionPath.cubicTo(start.x + bezierOffset, start.y,
                       end.x - bezierOffset, end.y,
                       end.x, end.y);
                       
    repaint();
}

void ConnectionComponent::paint(juce::Graphics& g)
{
    // Draw the connection line
    g.setColour(juce::Colours::white);
    g.strokePath(connectionPath, juce::PathStrokeType(2.0f));
}

void ConnectionComponent::mouseDown(const juce::MouseEvent& e)
{
    // Right-click to remove the connection
    if (e.mods.isRightButtonDown() && getParentComponent() != nullptr)
    {
        if (auto* canvas = dynamic_cast<PluginEditorCanvas*>(getParentComponent()))
        {
            canvas->removeConnection(this);
        }
    }
}

} // namespace Connections
