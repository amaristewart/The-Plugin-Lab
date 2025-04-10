#pragma once
#include <JuceHeader.h>

class AudioConnectionPoint;

/**
 * Visual representation of a connection between two AudioConnectionPoints
 */
class ConnectionComponent : public juce::Component
{
public:
    ConnectionComponent();
    ~ConnectionComponent() override;
    
    // Set connection endpoints
    void setSourcePoint(AudioConnectionPoint* source);
    void setDestinationPoint(AudioConnectionPoint* destination);
    
    // Get connection endpoints
    AudioConnectionPoint* getSourcePoint() const { return sourcePoint; }
    AudioConnectionPoint* getDestinationPoint() const { return destinationPoint; }
    
    // Update the position of the connection line to match the connection points
    void updatePosition();
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    
private:
    // Connection endpoints
    AudioConnectionPoint* sourcePoint = nullptr;
    AudioConnectionPoint* destinationPoint = nullptr;
    
    // Line path
    juce::Path connectionPath;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionComponent)
};
