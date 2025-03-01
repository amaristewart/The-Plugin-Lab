#pragma once
#include <JuceHeader.h>

class VisualizerComponent : public juce::Component
{
public:
    VisualizerComponent()
    {
        setOpaque(true);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        drawGrid(g);
    }

protected:
    virtual void drawGrid(juce::Graphics& g)
    {
        g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        
        // Draw vertical grid lines
        for (float x = 0; x < getWidth(); x += getWidth() / 10.0f)
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(getHeight()));
            
        // Draw horizontal grid lines
        for (float y = 0; y < getHeight(); y += getHeight() / 10.0f)
            g.drawHorizontalLine(static_cast<int>(y), 0.0f, static_cast<float>(getWidth()));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VisualizerComponent)
};
