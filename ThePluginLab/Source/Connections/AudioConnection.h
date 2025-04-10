#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"
#include "../Common/Forward.h"
#include "AudioConnectionPoint.h"
#include "../Components/PluginNodeComponent.h"

// Forward declarations to avoid circular dependencies
class AudioConnectionPoint;

/**
 * Represents an audio connection between two AudioConnectionPoints
 */
class AudioConnection : public juce::Component
{
public:
    AudioConnection(AudioConnectionPoint* source, AudioConnectionPoint* destination);
    ~AudioConnection() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void updatePosition();
    
    AudioConnectionPoint* getSourcePoint() const { return sourcePoint; }
    AudioConnectionPoint* getDestinationPoint() const { return destPoint; }
    
private:
    AudioConnectionPoint* sourcePoint;
    AudioConnectionPoint* destPoint;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConnection)
};
