#include "ResponseCurveComponent.h"
#include "../Nodes/EqualizerNode.h"

ResponseCurveComponent::ResponseCurveComponent(EqualizerNode& node)
    : owner(node)
{
    startTimerHz(30);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    stopTimer();
}

void ResponseCurveComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    
    auto bounds = getLocalBounds().toFloat();
    g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
    
    // Frequency grid
    g.setColour(juce::Colours::grey);
    const float frequencies[] = { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    
    for (auto freq : frequencies)
    {
        float x = bounds.getX() + std::log10(freq / 20.0f) * bounds.getWidth() / 3.0f;
        g.drawVerticalLine(int(x), bounds.getY(), bounds.getBottom());
    }
}

void ResponseCurveComponent::timerCallback()
{
    auto bounds = getLocalBounds().toFloat();
    responseCurve.clear();
    
    const float startFreq = 20.0f;
    const float endFreq = 20000.0f;
    
    responseCurve.startNewSubPath(bounds.getX(), bounds.getCentreY());
    
    for (float freq = startFreq; freq <= endFreq; freq *= 1.01f)
    {
        float magnitude = 1.0f; 
        
        float x = bounds.getX() + std::log10(freq / startFreq) * bounds.getWidth() / 3.0f;
        float y = bounds.getCentreY() - magnitude * bounds.getHeight() / 2.0f;
        
        responseCurve.lineTo(x, y);
    }
    
    repaint();
} 
