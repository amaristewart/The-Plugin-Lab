#pragma once
#include <JuceHeader.h>

class AudioPort : public juce::Component
{
public:
    enum class Type { Input, Output };

    explicit AudioPort(Type portType) : type(portType)
    {
        setSize(20, 20);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(type == Type::Input ? juce::Colours::green : juce::Colours::red);
        g.fillEllipse(getLocalBounds().toFloat());
        g.setColour(juce::Colours::white);
        g.drawEllipse(getLocalBounds().toFloat(), 1.0f);
    }

private:
    Type type;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPort)
};
