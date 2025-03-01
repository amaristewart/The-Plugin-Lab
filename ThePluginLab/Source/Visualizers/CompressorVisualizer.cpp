#include "CompressorVisualizer.h"

CompressorVisualizer::CompressorVisualizer()
{
    // Initialize level smoothing
    inputLevel.reset(100);  // 100ms smoothing
    outputLevel.reset(100);
    smoothedGainReduction.reset(50);

    // Set up meters with colors
    inputMeter = std::make_unique<LevelMeter>(juce::Colours::lightblue);
    outputMeter = std::make_unique<LevelMeter>(juce::Colours::lightgreen);
    reductionMeter = std::make_unique<LevelMeter>(juce::Colours::orange);

    startTimerHz(30); // Update at 30fps
}

void CompressorVisualizer::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::black);
    
    // Draw visualization elements
    drawDbScale(g);
    drawTransferFunction(g);
    drawGainReductionMeter(g);
    drawLevelMeters(g);

    // Draw the level indicator
    const float rmsAlpha = 0.7f;
    g.setColour(juce::Colours::white.withAlpha(rmsAlpha));
    
    float inputRMS = inputLevel.getCurrentValue();
    float outputRMS = outputLevel.getCurrentValue();
    
    const float x1 = inputToX(inputRMS);
    const float x2 = inputToX(outputRMS);
    const float y = dbToY(outputRMS);
    
    g.fillEllipse(x1 - 3, y - 3, 6, 6);
    g.fillEllipse(x2 - 3, y - 3, 6, 6);
    g.drawLine(x1, y, x2, y, 1.0f);
}

void CompressorVisualizer::resized()
{
    auto bounds = getLocalBounds();
    
    // Position any meters or components
    if (inputMeter && outputMeter && reductionMeter)
    {
        int meterWidth = 15;
        inputMeter->setBounds(bounds.removeFromLeft(meterWidth));
        outputMeter->setBounds(bounds.removeFromRight(meterWidth));
        reductionMeter->setBounds(bounds.removeFromRight(meterWidth));
    }
    
    // Position waveform displays if they exist
    auto waveformBounds = bounds.reduced(4);
    if (inputWaveform && outputWaveform)
    {
        inputWaveform->setBounds(waveformBounds.removeFromTop(waveformBounds.getHeight() / 2).reduced(0, 2));
        outputWaveform->setBounds(waveformBounds.reduced(0, 2));
    }
}

void CompressorVisualizer::drawTransferFunction(juce::Graphics& g)
{
    g.setColour(juce::Colours::purple);
    
    juce::Path transferCurve;
    const float margin = 20.0f;
    const float width = getWidth() - 2 * margin;
    const float height = getHeight() - 2 * margin;
    
    transferCurve.startNewSubPath(margin, getHeight() - margin);
    
    for (float x = 0; x <= width; x += 1.0f)
    {
        const float input = (x / width) * 60.0f - 60.0f;
        const float output = processGain(input);
        const float y = getHeight() - margin - (output + 60.0f) * height / 60.0f;
        
        transferCurve.lineTo(x + margin, y);
    }
    
    g.strokePath(transferCurve, juce::PathStrokeType(2.0f));
    
    // Draw threshold line
    const float thresholdX = margin + (threshold + 60.0f) * width / 60.0f;
    g.setColour(juce::Colours::red);
    g.drawVerticalLine(static_cast<int>(thresholdX), margin, getHeight() - margin);
}

void CompressorVisualizer::drawGainReductionMeter(juce::Graphics& g)
{
    const float margin = 20.0f;
    const float meterWidth = 15.0f;
    auto bounds = getLocalBounds().toFloat();
    auto meterBounds = bounds.removeFromRight(meterWidth).reduced(0, margin);
    
    // Draw meter background
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(meterBounds);
    
    // Draw gain reduction
    const float reduction = smoothedGainReduction.getCurrentValue();
    const float height = meterBounds.getHeight() * (reduction / 60.0f);
    
    g.setGradientFill(juce::ColourGradient(
        juce::Colours::red, meterBounds.getBottomLeft(),
        juce::Colours::green, meterBounds.getTopLeft(),
        false));
    g.fillRect(meterBounds.withHeight(height).withBottomY(meterBounds.getBottom()));
}

void CompressorVisualizer::drawLevelMeters(juce::Graphics& g)
{
    // Just repainting the level meters is enough since they handle their own drawing
    if (inputMeter) inputMeter->repaint();
    if (outputMeter) outputMeter->repaint();
    if (reductionMeter) reductionMeter->repaint();
}

void CompressorVisualizer::drawDbScale(juce::Graphics& g)
{
    g.setColour(juce::Colours::grey);
    g.setFont(12.0f);
    
    const float margin = 20.0f;
    const float width = getWidth() - 2 * margin;
    const float height = getHeight() - 2 * margin;
    
    // Draw dB markers
    for (int db = -60; db <= 0; db += 6)
    {
        const float x = margin + (db + 60.0f) * width / 60.0f;
        const float y = margin + (1.0f - (db + 60.0f) / 60.0f) * height;
        
        // Horizontal lines (input)
        g.setColour(juce::Colours::darkgrey);
        g.drawHorizontalLine(static_cast<int>(y), margin, getWidth() - margin);
        
        // Vertical lines (output)
        g.drawVerticalLine(static_cast<int>(x), margin, getHeight() - margin);
        
        // Labels
        g.setColour(juce::Colours::white);
        g.drawText(juce::String(db), static_cast<int>(x - 15), getHeight() - static_cast<int>(margin) + 5, 30, 20, juce::Justification::centred);
        g.drawText(juce::String(db), static_cast<int>(margin) - 30, static_cast<int>(y - 10), 25, 20, juce::Justification::centredRight);
    }
}

float CompressorVisualizer::processGain(float input)
{
    switch (type)
    {
        case DynamicType::Compressor:
            if (input <= threshold)
                return input;
            return threshold + (input - threshold) / ratio;
            
        case DynamicType::Limiter:
            return std::min(input, threshold);
            
        case DynamicType::Expander:
            if (input >= threshold)
                return input;
            return threshold + (input - threshold) * ratio;
            
        case DynamicType::Gate:
            return input >= threshold ? input : -60.0f;
            
        default:
            return input;
    }
}

void CompressorVisualizer::timerCallback()
{
    // Update any animations
    repaint();
}

void CompressorVisualizer::setLevels(float inLevel, float outLevel)
{
    inputLevel.setTargetValue(inLevel);
    outputLevel.setTargetValue(outLevel);
    
    if (inputMeter)
        inputMeter->setLevel(juce::jmap(inLevel, -60.0f, 0.0f, 0.0f, 1.0f));
    
    if (outputMeter)
        outputMeter->setLevel(juce::jmap(outLevel, -60.0f, 0.0f, 0.0f, 1.0f));
    
    repaint();
}

void CompressorVisualizer::setGainReduction(float reduction)
{
    smoothedGainReduction.setTargetValue(reduction);
    
    if (reductionMeter)
        reductionMeter->setLevel(juce::jmap(reduction, 0.0f, 20.0f, 0.0f, 1.0f));
    
    repaint();
}

void CompressorVisualizer::setType(DynamicType t)
{
    type = t;
    repaint();
}

void CompressorVisualizer::setThreshold(float t)
{
    threshold = t;
    repaint();
}

void CompressorVisualizer::setRatio(float r)
{
    ratio = r;
    repaint();
}

float CompressorVisualizer::inputToX(float db)
{
    const float margin = 20.0f;
    const float width = getWidth() - 2 * margin;
    return margin + ((db + 60.0f) / 60.0f) * width;
}

float CompressorVisualizer::dbToY(float db)
{
    const float margin = 20.0f;
    const float height = getHeight() - 2 * margin;
    return margin + (1.0f - (db + 60.0f) / 60.0f) * height;
}
