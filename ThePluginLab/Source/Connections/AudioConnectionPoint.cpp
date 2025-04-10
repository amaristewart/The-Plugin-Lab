#include "AudioConnectionPoint.h"
#include "../Components/PluginNodeComponent.h"
#include "../GUI/PluginEditorCanvas.h"

namespace ThePluginLab {
namespace Connections {

AudioConnectionPoint::AudioConnectionPoint(Component* owner, PortDirection dir)
    : ownerNode(owner)
    , direction(dir)
    , connected(false)
    , highlighted(false)
{
    setSize(10, 10);
    setInterceptsMouseClicks(true, false);
}

AudioConnectionPoint::~AudioConnectionPoint()
{
    // Cleanup if needed
}

void AudioConnectionPoint::paint(juce::Graphics& g)
{
    float thickness = 1.5f;
    juce::Colour outlineColour = juce::Colours::white;
    juce::Colour fillColour;

    if (connected)
    {
        fillColour = juce::Colours::green;
    }
    else if (isMouseOver() || highlighted)
    {
        fillColour = juce::Colours::lightgrey;
    }
    else
    {
        fillColour = juce::Colours::darkgrey;
    }

    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    if (direction == PortDirection::Input)
    {
        // Triangle pointing right for input
        juce::Path trianglePath;
        trianglePath.addTriangle(bounds.getX(), bounds.getY(),
                                bounds.getX(), bounds.getBottom(),
                                bounds.getRight(), bounds.getCentreY());

        g.setColour(fillColour);
        g.fillPath(trianglePath);
        g.setColour(outlineColour);
        g.strokePath(trianglePath, juce::PathStrokeType(thickness));
    }
    else
    {
        // Circle for output
        g.setColour(fillColour);
        g.fillEllipse(bounds);
        g.setColour(outlineColour);
        g.drawEllipse(bounds, thickness);
    }
}

void AudioConnectionPoint::mouseEnter(const juce::MouseEvent& e)
{
    isMouseOver = true;
    repaint();
}

void AudioConnectionPoint::mouseExit(const juce::MouseEvent& e)
{
    isMouseOver = false;
    repaint();
}

void AudioConnectionPoint::mouseDown(const juce::MouseEvent& e)
{
    // Start connection drag if this is an output port or disconnect if connections exist
    if (e.mods.isRightButtonDown() && connected)
    {
        disconnect();
    }
    else if (auto* canvas = findCanvas())
    {
        // Start dragging a connection
        canvas->startConnectionDrag(this);
        isConnecting = true;
    }
}

void AudioConnectionPoint::mouseDrag(const juce::MouseEvent& e)
{
    if (isConnecting)
    {
        // Update the connection drag
        if (auto* canvas = findCanvas())
        {
            canvas->dragConnection(this, e.getEventRelativeTo(canvas).position.toFloat());
        }
    }
}

void AudioConnectionPoint::mouseUp(const juce::MouseEvent& e)
{
    if (isConnecting)
    {
        // Complete the connection
        if (auto* canvas = findCanvas())
        {
            canvas->endConnectionDrag();
        }
        
        isConnecting = false;
    }
}

bool AudioConnectionPoint::canConnect(const AudioConnectionPoint* otherPoint) const
{
    // Ensure we have a valid connection point
    if (otherPoint == nullptr || otherPoint == this)
        return false;
        
    // Can't connect two inputs or two outputs
    if (direction == otherPoint->getDirection())
        return false;
        
    // Can't connect points that belong to the same node
    if (&otherPoint->getOwner() == &owner)
        return false;
        
    // All checks passed, connection is allowed
    return true;
}

void AudioConnectionPoint::setConnected(bool shouldBeConnected)
{
    if (connected != shouldBeConnected)
    {
        connected = shouldBeConnected;
        repaint();
    }
}

void AudioConnectionPoint::setHighlighted(bool shouldHighlight)
{
    if (highlighted != shouldHighlight)
    {
        highlighted = shouldHighlight;
        repaint();
    }
}

void AudioConnectionPoint::disconnect()
{
    if (auto* canvas = findCanvas())
    {
        canvas->disconnectPort(this);
    }
}

juce::Rectangle<int> AudioConnectionPoint::getScreenBounds() const
{
    return getLocalArea(nullptr, getLocalBounds());
}

void AudioConnectionPoint::updateConnectionPosition()
{
    if (auto* canvas = findCanvas())
    {
        canvas->updateConnectionsForPort(this);
    }
}

PluginEditorCanvas* AudioConnectionPoint::findCanvas() const
{
    // Find the canvas by traversing the component hierarchy
    for (auto* p = getParentComponent(); p != nullptr; p = p->getParentComponent())
    {
        if (auto* canvas = dynamic_cast<PluginEditorCanvas*>(p))
        {
            return canvas;
        }
    }
    
    return nullptr;
}

// Implement DragAndDropTarget methods
bool AudioConnectionPoint::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& details)
{
    // Check if this is a connection drag operation
    // We're only interested if the source is another AudioConnectionPoint
    if (auto* sourceAudioPoint = dynamic_cast<AudioConnectionPoint*>(details.sourceComponent.get()))
    {
        // Only accept connections between compatible ports (input to output or vice versa)
        return canConnect(sourceAudioPoint);
    }
    
    return false;
}

void AudioConnectionPoint::itemDragEnter(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    // Highlight this connection point when a compatible drag enters
    setHighlighted(true);
}

void AudioConnectionPoint::itemDragMove(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    // Nothing needed here for the prototype
}

void AudioConnectionPoint::itemDragExit(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    // Remove highlight when drag exits
    setHighlighted(false);
}

void AudioConnectionPoint::itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    // Check if the source is another AudioConnectionPoint
    if (auto* sourceAudioPoint = dynamic_cast<AudioConnectionPoint*>(dragSourceDetails.sourceComponent.get()))
    {
        // Create a connection between the points
        createConnection(sourceAudioPoint);
    }
    
    // Reset highlight state
    setHighlighted(false);
}

void AudioConnectionPoint::createConnection(AudioConnectionPoint* otherPoint)
{
    // Find the canvas
    auto* canvas = findCanvas();
    if (canvas == nullptr || otherPoint == nullptr)
        return;

    // Determine which point is the source and which is the destination
    AudioConnectionPoint* source = nullptr;
    AudioConnectionPoint* destination = nullptr;
    
    // The output point should be the source, the input point should be the destination
    if (direction == PortDirection::Output && otherPoint->direction == PortDirection::Input)
    {
        source = this;
        destination = otherPoint;
    }
    else if (direction == PortDirection::Input && otherPoint->direction == PortDirection::Output)
    {
        source = otherPoint;
        destination = this;
    }
    else
    {
        // Cannot connect two points of the same type
        return;
    }
    
    // Add the connection to the canvas
    canvas->addConnection(source, destination);
}

AudioConnectionPoint* AudioConnectionPoint::findConnectionTarget(const juce::Point<int>& screenPosition) const
{
    // Find the canvas
    auto* canvas = findCanvas();
    if (canvas == nullptr)
        return nullptr;
        
    // Use the canvas to find a connection point at the screen position
    return canvas->findPortAt(screenPosition, this);
}

} // namespace Connections
} // namespace ThePluginLab
