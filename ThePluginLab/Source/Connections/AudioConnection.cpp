#include "AudioConnection.h"

AudioConnection::AudioConnection(AudioConnectionPoint* source, AudioConnectionPoint* destination)
    : sourcePoint(source), destPoint(destination)
{
    setInterceptsMouseClicks(false, false); // Pass through mouse events
    updatePosition();
}

AudioConnection::~AudioConnection()
{
    // Clean up any resources if needed
}

void AudioConnection::paint(juce::Graphics& g)
{
    if (sourcePoint == nullptr || destPoint == nullptr)
        return;
        
    // Get the component-relative positions of the connection points
    auto* sourceComponent = sourcePoint->getParentComponent();
    auto* destComponent = destPoint->getParentComponent();
    
    if (sourceComponent == nullptr || destComponent == nullptr)
        return;
        
    // Calculate relative positions
    auto sourcePos = getLocalPoint(sourceComponent, sourcePoint->getBoundsInParent().getCentre());
    auto destPos = getLocalPoint(destComponent, destPoint->getBoundsInParent().getCentre());
    
    // Draw the connection as a curved line
    g.setColour(juce::Colours::white);
    
    // Draw a bezier curve
    juce::Path path;
    path.startNewSubPath(sourcePos.toFloat());
    
    float midX = (sourcePos.x + destPos.x) * 0.5f;
    juce::Point<float> control1(midX, sourcePos.y);
    juce::Point<float> control2(midX, destPos.y);
    
    path.cubicTo(control1, control2, destPos.toFloat());
    
    g.strokePath(path, juce::PathStrokeType(2.0f));
    
    // Draw arrow to indicate flow direction
    float arrowSize = 8.0f;
    float distanceAlongPath = 0.7f; // 70% along the path
    
    juce::Point<float> arrowPos = path.getPointAlongPath(path.getLength() * distanceAlongPath);
    juce::Point<float> direction = path.getPointAlongPath(path.getLength() * (distanceAlongPath + 0.01f)) - arrowPos;
    
    // Normalize the direction vector
    float mag = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (mag > 0)
        direction = direction / mag;
        
    juce::Point<float> normal(-direction.y, direction.x);
    
    juce::Path arrow;
    arrow.addTriangle(
        arrowPos, 
        arrowPos - direction * arrowSize + normal * arrowSize * 0.5f,
        arrowPos - direction * arrowSize - normal * arrowSize * 0.5f
    );
    
    g.fillPath(arrow);
}

void AudioConnection::resized()
{
    // Nothing specific needed here
}

void AudioConnection::updatePosition()
{
    if (sourcePoint == nullptr || destPoint == nullptr)
        return;
    
    // Find the parent component that contains both connection points
    auto* sourceParent = sourcePoint->getParentComponent();
    auto* destParent = destPoint->getParentComponent();
    
    if (sourceParent == nullptr || destParent == nullptr)
        return;
        
    // Calculate the bounds that encompasses both connection points
    auto sourceBounds = sourceParent->getParentComponent()->getLocalArea(sourceParent, sourcePoint->getBounds());
    auto destBounds = destParent->getParentComponent()->getLocalArea(destParent, destPoint->getBounds());
    
    auto totalBounds = sourceBounds.getUnion(destBounds).expanded(10);
    
    // Set the bounds of the connection component
    setBounds(totalBounds);
}
