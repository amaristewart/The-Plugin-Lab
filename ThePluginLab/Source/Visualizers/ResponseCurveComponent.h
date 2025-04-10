#pragma once
#include <JuceHeader.h>
#include "../Nodes/EqualizerNode.h"

class EqualizerNode;

class ResponseCurveComponent : public juce::Component,
                             public juce::Timer
{
public:
    explicit ResponseCurveComponent(EqualizerNode& node);
    ~ResponseCurveComponent() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    EqualizerNode& owner;
    juce::Path responseCurve;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponseCurveComponent)
}; 
