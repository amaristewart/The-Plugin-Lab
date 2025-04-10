#pragma once

// Forward include to the canonical location
#include "../Connections/AudioConnectionPoint.h"

#include <JuceHeader.h>
#include "../Common/Forward.h"
#include "../Common/Types.h"

class AudioConnectionPoint : public juce::Component
{
public:
    AudioConnectionPoint(PortType type, const juce::String& name);
    ~AudioConnectionPoint() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    PortType getPortType() const;
    juce::String getName() const;
    
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    
private:
    PortType portType;
    juce::String portName;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConnectionPoint)
}; 