#pragma once
#include <JuceHeader.h>
#include "AudioConnectionPoint.h"

class AudioConnection : public juce::Component
{
public:
    AudioConnection(AudioConnectionPoint* source, AudioConnectionPoint* destination);
    ~AudioConnection() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void updatePosition();
    
private:
    AudioConnectionPoint* sourcePoint;
    AudioConnectionPoint* destPoint;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConnection)
};
