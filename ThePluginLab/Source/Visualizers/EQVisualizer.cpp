#include "EQVisualizer.h"

EQVisualizer::EQVisualizer()
{
    startTimerHz(30);
    setOpaque(true);
}

EQVisualizer::~EQVisualizer()
{
    stopTimer();
}

void EQVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Background
    g.setColour(juce::Colours::black);
    g.fillRect(bounds);
    
    // Draw elements
    drawFrequencyScale(g);
    drawFrequencyResponse(g, bounds.toNearestInt());
    drawLevelMeters(g, bounds.toNearestInt());
}

void EQVisualizer::drawFrequencyResponse(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(juce::Colours::white);
    juce::Path responseCurve;
    
    const float width = bounds.getWidth();
    
    // Start path
    responseCurve.startNewSubPath(bounds.getX(), bounds.getCentreY());
    
    // Draw response curve
    for (float x = 1; x < width; x += 1.0f)
    {
        float freq = xToFreq(x / width, width);
        float response = 0.0f;
        
        switch (currentType)
        {
            case EQType::Lowpass:
            case EQType::Highpass:
                response = std::pow(10.0f, gain / 20.0f) * frequency / (frequency + q);
                break;
                
            case EQType::Bandpass:
            case EQType::Notch:
            case EQType::Peaking:
                response = std::pow(10.0f, gain / 20.0f) * q * frequency /
                          (std::sqrt(std::pow(frequency, 2.0f) + std::pow(q, 2.0f)));
                break;
                
            default:
                break;
        }
        
        float y = dbToY(20.0f * std::log10(std::abs(response)), bounds.getHeight());
        responseCurve.lineTo(x, y);
    }
    
    g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
}

void EQVisualizer::drawLevelMeters(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    const float meterWidth = 20.0f;
    auto workingBounds = bounds; // Create non-const copy
    
    // Input meter
    auto inputMeterBounds = workingBounds.removeFromLeft(meterWidth).toFloat();
    g.setColour(juce::Colours::green);
    float inputHeight = juce::mapFromLog10(inputLevel, -60.0f, 6.0f) * bounds.getHeight();
    g.fillRect(inputMeterBounds.removeFromBottom(inputHeight));
    
    // Output meter
    auto outputMeterBounds = workingBounds.removeFromRight(meterWidth).toFloat();
    g.setColour(juce::Colours::blue);
    float outputHeight = juce::mapFromLog10(outputLevel, -60.0f, 6.0f) * bounds.getHeight();
    g.fillRect(outputMeterBounds.removeFromBottom(outputHeight));
}

void EQVisualizer::drawFrequencyScale(juce::Graphics& g)
{
    g.setColour(juce::Colours::grey);
    g.setFont(12.0f);
    
    // Draw frequency markers
    const float frequencies[] = { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    const float dbMarkers[] = { -24, -18, -12, -6, 0, 6, 12, 18, 24 };
    
    auto bounds = getLocalBounds();
    const float width = bounds.getWidth();
    const float height = bounds.getHeight();
    
    // Draw vertical frequency lines and labels
    for (float freq : frequencies)
    {
        const float x = freqToX(freq, width) * width;
        if (x >= 0 && x <= width)
        {
            // Draw vertical line
            g.setColour(juce::Colours::darkgrey);
            g.drawVerticalLine(static_cast<int>(x), 0.0f, height);
            
            // Draw frequency label
            g.setColour(juce::Colours::grey);
            juce::String label = (freq >= 1000) ? juce::String(freq/1000) + "k" : juce::String(freq);
            g.drawText(label, static_cast<int>(x - 15), height - 20, 30, 20,
                      juce::Justification::centred);
        }
    }
    
    // Draw horizontal dB lines and labels
    for (float db : dbMarkers)
    {
        const float y = dbToY(db, height);
        if (y >= 0 && y <= height)
        {
            // Draw horizontal line
            g.setColour(juce::Colours::darkgrey);
            g.drawHorizontalLine(static_cast<int>(y), 0.0f, width);
            
            // Draw dB label
            g.setColour(juce::Colours::grey);
            g.drawText(juce::String(db), 0, static_cast<int>(y - 10), 30, 20,
                      juce::Justification::centred);
        }
    }
}

void EQVisualizer::timerCallback()
{
    repaint();
}

// Parameter setters
void EQVisualizer::setType(EQType type)
{
    currentType = type;
    repaint();
}

void EQVisualizer::setFrequency(float freq)
{
    frequency = freq;
    repaint();
}

void EQVisualizer::setGain(float gainDb)
{
    gain = gainDb;
    repaint();
}

void EQVisualizer::setQ(float qFactor)
{
    q = qFactor;
    repaint();
}

void EQVisualizer::setInputLevel(float level)
{
    inputLevel = level;
    repaint();
}

void EQVisualizer::setOutputLevel(float level)
{
    outputLevel = level;
    repaint();
}

void EQVisualizer::pushSample(float inputSample, float outputSample)
{
    // Create buffers if they don't exist
    if (inputBuffer.getNumSamples() < bufferSize)
    {
        inputBuffer.setSize(1, bufferSize);
        inputBuffer.clear();
    }
    
    if (outputBuffer.getNumSamples() < bufferSize)
    {
        outputBuffer.setSize(1, bufferSize);
        outputBuffer.clear();
    }
    
    // Write samples to buffer
    inputBuffer.setSample(0, bufferPosition, inputSample);
    outputBuffer.setSample(0, bufferPosition, outputSample);
    
    // Update buffer position
    bufferPosition = (bufferPosition + 1) % bufferSize;
    
    // Update level meters using RMS values
    float inputSum = 0.0f, outputSum = 0.0f;
    int numSamples = juce::jmin(128, bufferSize);
    
    for (int i = 0; i < numSamples; ++i)
    {
        int pos = (bufferPosition - i + bufferSize) % bufferSize;
        inputSum += inputBuffer.getSample(0, pos) * inputBuffer.getSample(0, pos);
        outputSum += outputBuffer.getSample(0, pos) * outputBuffer.getSample(0, pos);
    }
    
    // Convert to dB
    inputLevel = inputSum > 0.0f ? 
        juce::jlimit(-60.0f, 6.0f, 20.0f * std::log10(std::sqrt(inputSum / numSamples))) : -60.0f;
    
    outputLevel = outputSum > 0.0f ? 
        juce::jlimit(-60.0f, 6.0f, 20.0f * std::log10(std::sqrt(outputSum / numSamples))) : -60.0f;
}

float EQVisualizer::xToFreq(float x, float width) const
{
    return 20.0f * std::pow(10.0f, (x / width) * 3.0f);
}

float EQVisualizer::freqToX(float freq, float width) const
{
    return width * (std::log10(freq) - std::log10(20.0f)) / 3.0f;
}

float EQVisualizer::dbToY(float db, float height) const
{
    return height * (0.5f - db / 48.0f);
}

float EQVisualizer::yToDb(float y, float height) const
{
    return 48.0f * (0.5f - y / height);
}
