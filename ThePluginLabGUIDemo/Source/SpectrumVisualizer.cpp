#include "SpectrumVisualizer.h"
#include "BlockComponent.h"

//==============================================================================
SpectrumVisualizer::SpectrumVisualizer()
{
    // Set default size
    setSize(300, 200);
    
    // Start the timer to update the animation
    startTimerHz(30); // 30 fps
    
    // Set default filter band
    FilterBand defaultBand;
    defaultBand.frequency = frequency;
    defaultBand.q = qFactor;
    defaultBand.gain = gain;
    filterBands.add(defaultBand);
    
    // Initialize animation state
    previousBand = defaultBand;
    targetBand = defaultBand;
}

SpectrumVisualizer::~SpectrumVisualizer()
{
    stopTimer();
}

void SpectrumVisualizer::paint(juce::Graphics& g)
{
    // Get bounds and create a slightly smaller area for the graph
    auto bounds = getLocalBounds().toFloat().reduced(5.0f);
    
    // Create hexagon path for the border
    juce::Path hexagon;
    float w = bounds.getWidth();
    float h = bounds.getHeight();
    float x = bounds.getX();
    float y = bounds.getY();
    
    hexagon.startNewSubPath(x + w * 0.25f, y);
    hexagon.lineTo(x + w * 0.75f, y);
    hexagon.lineTo(x + w, y + h * 0.5f);
    hexagon.lineTo(x + w * 0.75f, y + h);
    hexagon.lineTo(x + w * 0.25f, y + h);
    hexagon.lineTo(x, y + h * 0.5f);
    hexagon.closeSubPath();
    
    // Fill the hexagon with a dark background
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillPath(hexagon);
    
    // Create a slightly inset bounds for the grid and curves
    auto innerBounds = bounds.reduced(10.0f);
    
    // Draw grid
    drawBackgroundGrid(g, innerBounds);
    
    // Draw scales
    drawFrequencyScale(g, innerBounds);
    drawGainScale(g, innerBounds);
    
    // Draw the frequency response curve
    drawResponseCurve(g, innerBounds);
    
    // Draw title at top
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("EQ Spectrum", bounds.withHeight(20.0f), juce::Justification::centred, false);
    
    // Draw the purple hexagon border
    g.setColour(juce::Colours::purple.withAlpha(0.8f));
    g.strokePath(hexagon, juce::PathStrokeType(2.0f));
}

void SpectrumVisualizer::resized()
{
    // The component size has changed
    repaint();
}

void SpectrumVisualizer::timerCallback()
{
    if (isAnimating)
    {
        // Update animation progress
        animationProgress += 0.1f; // Controls animation speed
        
        if (animationProgress >= 1.0f)
        {
            animationProgress = 0.0f;
            isAnimating = false;
            
            // Animation complete, set the current values to the target
            for (auto& band : filterBands)
            {
                if (band.isActive)
                {
                    band = targetBand;
                }
            }
        }
        
        // Repaint to show animation frame
        repaint();
    }
}

void SpectrumVisualizer::setFrequency(float newFrequency)
{
    // Limit to valid range
    newFrequency = juce::jlimit(minFrequency, maxFrequency, newFrequency);
    
    if (frequency != newFrequency)
    {
        // Set up animation
        previousBand = filterBands.size() > 0 ? filterBands[0] : FilterBand();
        targetBand = previousBand;
        targetBand.frequency = newFrequency;
        
        frequency = newFrequency;
        animationProgress = 0.0f;
        isAnimating = true;
        
        repaint();
    }
}

void SpectrumVisualizer::setQFactor(float newQ)
{
    // Limit to valid range (Q shouldn't be negative or too extreme)
    newQ = juce::jlimit(0.1f, 10.0f, newQ);
    
    if (qFactor != newQ)
    {
        previousBand = filterBands.size() > 0 ? filterBands[0] : FilterBand();
        targetBand = previousBand;
        targetBand.q = newQ;
        
        qFactor = newQ;
        animationProgress = 0.0f;
        isAnimating = true;
        
        repaint();
    }
}

void SpectrumVisualizer::setGain(float newGain)
{
    // Limit to valid range
    newGain = juce::jlimit(minGain, maxGain, newGain);
    
    if (gain != newGain)
    {
        previousBand = filterBands.size() > 0 ? filterBands[0] : FilterBand();
        targetBand = previousBand;
        targetBand.gain = newGain;
        
        gain = newGain;
        animationProgress = 0.0f;
        isAnimating = true;
        
        repaint();
    }
}

void SpectrumVisualizer::addFilterBand(float freq, float q, float gain)
{
    // Create a new filter band
    FilterBand newBand;
    newBand.frequency = juce::jlimit(minFrequency, maxFrequency, freq);
    newBand.q = juce::jlimit(0.1f, 10.0f, q);
    newBand.gain = juce::jlimit(minGain, maxGain, gain);
    
    // Set up animation
    previousBand = filterBands.size() > 0 ? filterBands[0] : FilterBand();
    targetBand = newBand;
    
    // Update the first band or add a new one if empty
    if (filterBands.size() > 0)
        filterBands.set(0, previousBand); // Keep the previous band for animation
    else
        filterBands.add(previousBand);
    
    // Update class member values
    frequency = newBand.frequency;
    qFactor = newBand.q;
    gain = newBand.gain;
    
    // Start animation
    animationProgress = 0.0f;
    isAnimating = true;
    
    repaint();
}

void SpectrumVisualizer::clearFilterBands()
{
    filterBands.clear();
    
    // Add default band
    FilterBand defaultBand;
    defaultBand.frequency = 1000.0f;
    defaultBand.q = 1.0f;
    defaultBand.gain = 0.0f;
    filterBands.add(defaultBand);
    
    // Reset animation
    previousBand = defaultBand;
    targetBand = defaultBand;
    
    // Update class member values
    frequency = defaultBand.frequency;
    qFactor = defaultBand.q;
    gain = defaultBand.gain;
    
    repaint();
}

float SpectrumVisualizer::freqToX(float freq, float width) const
{
    // Convert frequency to x position using logarithmic scale
    float logMin = std::log10(minFrequency);
    float logMax = std::log10(maxFrequency);
    float logFreq = std::log10(freq);
    
    // Map log(freq) from [logMin, logMax] to [0, width]
    return width * (logFreq - logMin) / (logMax - logMin);
}

float SpectrumVisualizer::gainToY(float gainDB, float height) const
{
    // Convert gain in dB to y position (inverted because y grows downward in graphics)
    // Map gain from [minGain, maxGain] to [height, 0]
    return height * (1.0f - (gainDB - minGain) / (maxGain - minGain));
}

float SpectrumVisualizer::calculateResponse(float freq)
{
    // Calculate the combined response of all filter bands at the given frequency
    float response = 0.0f;
    
    for (int i = 0; i < filterBands.size(); ++i)
    {
        const auto& band = filterBands.getReference(i);
        
        if (!band.isActive)
            continue;
        
        // Use current band values for the animating band, or interpolate during animation
        float bandFreq, bandQ, bandGain;
        
        if (isAnimating && i == 0) // Use index comparison instead of address comparison
        {
            // Interpolate between previous and target values
            bandFreq = previousBand.frequency + animationProgress * (targetBand.frequency - previousBand.frequency);
            bandQ = previousBand.q + animationProgress * (targetBand.q - previousBand.q);
            bandGain = previousBand.gain + animationProgress * (targetBand.gain - previousBand.gain);
        }
        else
        {
            bandFreq = band.frequency;
            bandQ = band.q;
            bandGain = band.gain;
        }
        
        // Calculate the standard EQ formula for a peak filter (simplified)
        float omega = (freq / bandFreq);
        float denominator = 1.0f + (1.0f / bandQ) * omega;
        
        // Simplified EQ response curve formula
        if (omega > 0.0f)
        {
            float responseFactor;
            // Different equations for boost vs cut for better visualization
            if (bandGain >= 0)
            {
                // Boost
                responseFactor = bandGain * (1.0f - 1.0f / (1.0f + std::pow(omega, 4) * std::pow(bandQ, 2)));
            }
            else
            {
                // Cut
                responseFactor = bandGain * (1.0f / (1.0f + std::pow(omega, 2) / bandQ));
            }
            response += responseFactor;
        }
    }
    
    return response;
}

void SpectrumVisualizer::drawResponseCurve(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    
    // Calculate 0 dB line position
    float zeroDB = gainToY(0.0f, height);
    
    // Create a path for the response curve
    juce::Path responsePath;
    
    // Start at the left edge at zero dB
    responsePath.startNewSubPath(bounds.getX(), bounds.getY() + zeroDB);
    
    // Add points to the path
    const int numPoints = 200;  // More points for smoother curve
    
    for (int i = 0; i <= numPoints; ++i)
    {
        float ratio = static_cast<float>(i) / numPoints;
        // Calculate frequency using logarithmic scale
        float freq = minFrequency * std::pow(maxFrequency / minFrequency, ratio);
        
        // Calculate the response at this frequency
        float response = calculateResponse(freq);
        
        // Convert to coordinates
        float x = bounds.getX() + freqToX(freq, width);
        float y = bounds.getY() + gainToY(response, height);
        
        responsePath.lineTo(x, y);
    }
    
    // Complete the path to form a filled shape
    responsePath.lineTo(bounds.getRight(), bounds.getY() + zeroDB);
    responsePath.closeSubPath();
    
    // Fill the path with a gradient
    juce::ColourGradient gradient(
        juce::Colours::blue.withAlpha(0.6f), bounds.getX(), bounds.getY() + zeroDB,
        juce::Colours::red.withAlpha(0.6f), bounds.getRight(), bounds.getY(), false
    );
    
    g.setGradientFill(gradient);
    g.fillPath(responsePath);
    
    // Draw the response curve with a solid line
    g.setColour(juce::Colours::white);
    g.strokePath(responsePath, juce::PathStrokeType(2.0f));
}

void SpectrumVisualizer::drawFrequencyScale(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    g.setColour(juce::Colours::grey);
    g.setFont(12.0f);
    
    // Draw frequency markers
    float freqMarkers[] = { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    
    for (float freq : freqMarkers)
    {
        float x = bounds.getX() + freqToX(freq, bounds.getWidth());
        
        // Draw tick mark
        g.drawVerticalLine(static_cast<int>(x), bounds.getBottom() - 5, bounds.getBottom());
        
        // Format the frequency label
        juce::String freqLabel;
        if (freq >= 1000)
            freqLabel = juce::String(freq / 1000) + "k";
        else
            freqLabel = juce::String(static_cast<int>(freq));
        
        // Draw label below the tick mark
        g.drawText(freqLabel, static_cast<int>(x - 10), static_cast<int>(bounds.getBottom() + 5),
                  20, 15, juce::Justification::centred, false);
    }
}

void SpectrumVisualizer::drawGainScale(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    g.setColour(juce::Colours::grey);
    g.setFont(12.0f);
    
    // Draw gain markers
    for (float db = minGain; db <= maxGain; db += 6.0f)  // Every 6 dB
    {
        float y = bounds.getY() + gainToY(db, bounds.getHeight());
        
        // Draw tick mark
        g.drawHorizontalLine(static_cast<int>(y), bounds.getX(), bounds.getX() + 5);
        
        // Draw gain label
        g.drawText(juce::String(static_cast<int>(db)), 
                  static_cast<int>(bounds.getX() + 5), 
                  static_cast<int>(y - 6), 
                  25, 12, juce::Justification::left, false);
    }
}

juce::Path SpectrumVisualizer::createRoundedTop(juce::Rectangle<float> bounds, float cornerSize)
{
    juce::Path path;
    
    path.startNewSubPath(bounds.getX(), bounds.getBottom());
    path.lineTo(bounds.getX(), bounds.getY() + cornerSize);
    path.addArc(bounds.getX(), bounds.getY(), cornerSize * 2.0f, cornerSize * 2.0f, juce::MathConstants<float>::pi, juce::MathConstants<float>::pi * 1.5f);
    path.lineTo(bounds.getRight() - cornerSize, bounds.getY());
    path.addArc(bounds.getRight() - cornerSize * 2.0f, bounds.getY(), cornerSize * 2.0f, cornerSize * 2.0f, juce::MathConstants<float>::pi * 1.5f, juce::MathConstants<float>::pi * 2.0f);
    path.lineTo(bounds.getRight(), bounds.getBottom());
    path.closeSubPath();
    
    return path;
}

void SpectrumVisualizer::drawBackgroundGrid(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    g.setColour(juce::Colours::grey.withAlpha(0.2f));
    
    // Draw horizontal grid lines (gain)
    for (float db = minGain; db <= maxGain; db += 6.0f)
    {
        float y = bounds.getY() + gainToY(db, bounds.getHeight());
        g.drawHorizontalLine(static_cast<int>(y), bounds.getX(), bounds.getRight());
    }
    
    // Draw vertical grid lines (frequency)
    float freqMarkers[] = { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    
    for (float freq : freqMarkers)
    {
        float x = bounds.getX() + freqToX(freq, bounds.getWidth());
        g.drawVerticalLine(static_cast<int>(x), bounds.getY(), bounds.getBottom());
    }
    
    // Draw 0dB line slightly bolder
    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    float zeroDB = bounds.getY() + gainToY(0.0f, bounds.getHeight());
    g.drawHorizontalLine(static_cast<int>(zeroDB), bounds.getX(), bounds.getRight());
}
