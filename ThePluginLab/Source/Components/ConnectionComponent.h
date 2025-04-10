#pragma once
#include <JuceHeader.h>
#include "../Connections/AudioConnectionPoint.h"

class ConnectionComponent : public juce::Component
{
public:
    ConnectionComponent(AudioConnectionPoint* source, AudioConnectionPoint* destination);
    ConnectionComponent(AudioConnectionPoint* source, const juce::Point<float>& endPoint);
    ~ConnectionComponent() override;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    
    AudioConnectionPoint* getSource() const { return sourcePoint; }
    AudioConnectionPoint* getDestination() const { return destinationPoint; }
    
    void updatePosition();
    void setEndPoint(const juce::Point<float>& newEndPoint);
    bool hitTest(int x, int y) override;

private:
    AudioConnectionPoint* sourcePoint;
    AudioConnectionPoint* destinationPoint;
    juce::Point<float> dragEndPoint;
    bool isDraggingEndPoint;
    float lineThickness = 2.5f;
    
    juce::Path calculateConnectionPath() const;
    juce::Point<float> getGlobalPoint(const juce::Point<float>& point) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionComponent)
};
