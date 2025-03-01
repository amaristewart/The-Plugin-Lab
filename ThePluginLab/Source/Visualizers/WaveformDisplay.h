#pragma once
#include <JuceHeader.h>

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay()
    {
        ringBuffer.resize(bufferSize);
        setOpaque(true);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        
        // Draw grid
        drawTimeMarkers(g);
        drawAmplitudeScale(g);
        
        // Draw waveform
        g.setColour(waveformColor);
        auto bounds = getLocalBounds().toFloat();
        juce::Path waveformPath;
        
        float x = 0;
        const float xScale = bounds.getWidth() / static_cast<float>(bufferSize);
        const float yScale = bounds.getHeight() / 2.0f;
        const float centerY = bounds.getCentreY();
        
        waveformPath.startNewSubPath(0, centerY);
        
        for (int i = writePos; i < bufferSize; ++i)
        {
            float y = centerY - (ringBuffer[i] * yScale);
            waveformPath.lineTo(x, y);
            x += xScale;
        }
        
        for (int i = 0; i < writePos; ++i)
        {
            float y = centerY - (ringBuffer[i] * yScale);
            waveformPath.lineTo(x, y);
            x += xScale;
        }
        
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
    }
    
    void pushSample(float sample)
    {
        ringBuffer[writePos] = sample;
        writePos = (writePos + 1) % bufferSize;
        repaint();
    }
    
    void setWaveformColor(juce::Colour color)
    {
        waveformColor = color;
        repaint();
    }

    void drawTimeMarkers(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.setFont(10.0f);
        
        // Draw time markers (assuming 44.1kHz sample rate)
        const float msPerSample = 1000.0f / 44100.0f;
        const float totalTimeMs = msPerSample * bufferSize;
        
        for (int i = 0; i < 5; ++i)
        {
            float x = bounds.getWidth() * (i / 4.0f);
            float timeMs = (totalTimeMs * i) / 4.0f;
            
            g.drawVerticalLine(static_cast<int>(x), 0.0f, bounds.getHeight());
            g.drawText(juce::String(timeMs, 1) + "ms", 
                      static_cast<int>(x), bounds.getHeight() - 15, 
                      30, 10, juce::Justification::centred);
        }
    }
    
    void drawAmplitudeScale(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.setFont(10.0f);
        
        // Draw amplitude markers (-1.0 to 1.0)
        const float levels[] = { -1.0f, -0.5f, 0.0f, 0.5f, 1.0f };
        
        for (float level : levels)
        {
            float y = bounds.getCentreY() - (level * bounds.getHeight() / 2.0f);
            g.drawHorizontalLine(static_cast<int>(y), 0.0f, bounds.getWidth());
            g.drawText(juce::String(level), 0, static_cast<int>(y) - 5, 
                      25, 10, juce::Justification::right);
        }
    }

private:
    static constexpr int bufferSize = 1024;
    std::vector<float> ringBuffer;
    int writePos = 0;
    juce::Colour waveformColor = juce::Colours::lime;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
