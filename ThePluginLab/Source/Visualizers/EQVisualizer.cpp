#include "EQVisualizer.h"

EQVisualizer::EQVisualizer()
{
    setSize(200, 100);
}

EQVisualizer::~EQVisualizer()
{
    // Cleanup if needed
}

void EQVisualizer::paint(juce::Graphics& g)
{
    // Draw background
    g.fillAll(juce::Colours::black.withAlpha(0.8f));
    
    // Draw frequency response curve
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds(), 1);
    
    // Draw a dummy curve
    auto bounds = getLocalBounds().reduced(2);
    juce::Path path;
    path.startNewSubPath(bounds.getX(), bounds.getCentreY());
    
    for (int x = bounds.getX(); x <= bounds.getRight(); x += 2)
    {
        float y = bounds.getCentreY() + std::sin((x - bounds.getX()) * 0.05f) * 20.0f;
        path.lineTo(x, y);
    }
    
    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void EQVisualizer::resized()
{
    // Nothing to do here
}

void EQVisualizer::updateCurve(const juce::Array<float>& response)
{
    // Store the response data and repaint
    frequencyResponse = response;
    repaint();
}
