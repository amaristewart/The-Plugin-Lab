#include "CompressorVisualizer.h"

CompressorVisualizer::CompressorVisualizer()
{
    setSize(200, 150);
}

CompressorVisualizer::~CompressorVisualizer()
{
    // Cleanup if needed
}

void CompressorVisualizer::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(juce::Colours::black.withAlpha(0.8f));
    
    // Draw border
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds(), 1);
    
    // Draw a representation of the compressor curve
    auto bounds = getLocalBounds().reduced(4);
    juce::Path curve;
    
    // Starting point (bottom left)
    const float width = static_cast<float>(bounds.getWidth());
    const float height = static_cast<float>(bounds.getHeight());
    const float startX = static_cast<float>(bounds.getX());
    const float startY = static_cast<float>(bounds.getBottom());
    
    // Normalize threshold to curve position (0-1)
    const float thresholdNormalized = juce::jmap(threshold, -60.0f, 0.0f, 0.0f, 1.0f);
    const float thresholdX = startX + thresholdNormalized * width;
    
    // Calculate curve
    curve.startNewSubPath(startX, startY);
    curve.lineTo(thresholdX, startY - thresholdNormalized * height);
    
    // Above threshold, apply ratio
    float endX = width + startX;
    float thresholdY = startY - thresholdNormalized * height;
    float endY = thresholdY - (1.0f - thresholdNormalized) * height / ratio;
    curve.lineTo(endX, endY);
    
    // Draw the curve
    g.setColour(juce::Colours::white);
    g.strokePath(curve, juce::PathStrokeType(2.0f));
    
    // Draw threshold line
    g.setColour(juce::Colours::yellow.withAlpha(0.5f));
    g.drawVerticalLine(static_cast<int>(thresholdX), static_cast<float>(bounds.getY()), 
                   static_cast<float>(bounds.getBottom()));
    
    // Draw current gain reduction meter
    const float reductionHeight = juce::jmap(currentGainReduction, 0.0f, 20.0f, 0.0f, height * 0.5f);
    const juce::Rectangle<float> reductionMeter(
        static_cast<float>(bounds.getX()), 
        static_cast<float>(bounds.getBottom() - reductionHeight),
        10.0f, reductionHeight);
    
    g.setColour(juce::Colours::red.withAlpha(0.7f));
    g.fillRect(reductionMeter);
    
    // Draw info text
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("Threshold: " + juce::String(threshold, 1) + " dB",
              bounds.removeFromTop(15), juce::Justification::topLeft, false);
    g.drawText("Ratio: " + juce::String(ratio, 1) + ":1",
              bounds.removeFromTop(15), juce::Justification::topLeft, false);
    g.drawText("GR: " + juce::String(currentGainReduction, 1) + " dB",
              bounds.removeFromTop(15), juce::Justification::topLeft, false);
}

void CompressorVisualizer::resized()
{
    // Nothing special needed here
}

void CompressorVisualizer::updateSettings(float newThreshold, float newRatio, 
                                        float newAttack, float newRelease)
{
    threshold = newThreshold;
    ratio = newRatio;
    attack = newAttack;
    release = newRelease;
    repaint();
}

void CompressorVisualizer::updateGainReduction(float gainReductionDb)
{
    currentGainReduction = gainReductionDb;
    repaint();
}
