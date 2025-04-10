#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

enum class PortDirection {
    Input,
    Output
};

class AudioConnectionPoint : public juce::Component
{
public:
    AudioConnectionPoint(juce::Component* owner, PortDirection dir);
    ~AudioConnectionPoint() override;

    void paint(juce::Graphics& g) override;

    // Add any other public methods here
    bool isConnected() const { return connected; }
    void setConnected(bool isConnected) { connected = isConnected; repaint(); }
    
    void setHighlighted(bool isHighlighted) { highlighted = isHighlighted; repaint(); }
    
    PortDirection getDirection() const { return direction; }
    
    juce::Component* getOwnerNode() const { return ownerNode; }

private:
    juce::Component* ownerNode;
    PortDirection direction;
    bool connected;
    bool highlighted;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConnectionPoint)
};
