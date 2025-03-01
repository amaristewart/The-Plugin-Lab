#pragma once
#include <JuceHeader.h>
#include "../Common/Forward.h"

class ConnectionComponent : public juce::Component
{
public:
    ConnectionComponent(AudioConnectionPoint* source, AudioConnectionPoint* dest);
    ~ConnectionComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void updatePosition();
    
    PluginNodeComponent* getSourceNode() const;
    int getSourceChannel() const;
    PluginNodeComponent* getDestinationNode() const;
    int getDestinationChannel() const;
    
    AudioConnectionPoint* getSourcePoint() const { return sourcePoint; }
    AudioConnectionPoint* getDestinationPoint() const { return destPoint; }

private:
    AudioConnectionPoint* sourcePoint;
    AudioConnectionPoint* destPoint;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionComponent)
};
