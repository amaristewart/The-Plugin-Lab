#include "AudioVisualizer.h"

AudioVisualizer::AudioVisualizer()
    : fft(fftOrder),
      fftData(fftSize * 2),
      fftWindow(fftSize)
{
    setOpaque(true);
    startTimerHz(60); // 60 fps refresh rate
    
    for (int i = 0; i < fftSize; ++i)
        fftWindow[i] = 0.5f - 0.5f * std::cos(2.0f * juce::MathConstants<float>::pi * i / (fftSize - 1));
}

AudioVisualizer::~AudioVisualizer()
{
    stopTimer();
}

void AudioVisualizer::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    switch (visualizationType)
    {
        case VisualizationType::Waveform:
            drawWaveform(g);
            break;
        case VisualizationType::Spectrum:
            drawSpectrum(g);
            break;
        case VisualizationType::EQResponse:
            drawEQResponse(g);
            break;
    }
}

void AudioVisualizer::pushBuffer(const juce::AudioBuffer<float>& buffer)
{
    auto* channelData = buffer.getReadPointer(0);
    int numSamples = buffer.getNumSamples();
    
    for (int i = 0; i < numSamples; ++i)
    {
        float absLevel = std::abs(channelData[i]);
        maxLevel = juce::jmax(maxLevel * 0.99f, absLevel);
    }
    
    for (int i = 0; i < numSamples; ++i)
    {
        audioData[writePosition] = channelData[i];
        writePosition = (writePosition + 1) % bufferSize;
    }
    
    if (visualizationType == VisualizationType::Spectrum)
        calculateFFT();
}

void AudioVisualizer::drawWaveform(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float centerY = bounds.getCentreY();
    float width = bounds.getWidth();
    
    g.setColour(juce::Colours::white);
    
    juce::Path waveformPath;
    waveformPath.startNewSubPath(0, centerY);
    
    for (int i = 0; i < bufferSize; ++i)
    {
        float x = (float)i / bufferSize * width;
        float y = centerY + audioData[(writePosition + i) % bufferSize] * bounds.getHeight() * 0.5f;
        waveformPath.lineTo(x, y);
    }
    
    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
}

void AudioVisualizer::drawSpectrum(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    
    // Create gradient for spectrum
    juce::ColourGradient gradient;
    gradient.addColour(0.0, gradientColours[0]);
    gradient.addColour(0.3, gradientColours[1]);
    gradient.addColour(0.6, gradientColours[2]);
    gradient.addColour(1.0, gradientColours[3]);
    
    juce::Path spectrumPath;
    spectrumPath.startNewSubPath(0, height);
    
    for (int i = 0; i < fftSize / 2; ++i)
    {
        float freq = (float)i / (fftSize / 2) * 20000.0f;
        float x = std::log10(freq / 20.0f) * width / 3.0f;
        
        if (x >= 0 && x <= width)
        {
            float level = juce::jlimit(0.0f, 1.0f, fftData[i] / maxLevel);
            float y = height - level * height;
            spectrumPath.lineTo(x, y);
        }
    }
    
    spectrumPath.lineTo(width, height);
    spectrumPath.closeSubPath();
    
    g.setGradientFill(gradient);
    g.fillPath(spectrumPath);
}

void AudioVisualizer::calculateFFT()
{
    for (int i = 0; i < fftSize; ++i)
    {
        fftData[i] = audioData[(writePosition + i) % bufferSize] * fftWindow[i];
        fftData[i + fftSize] = 0.0f; // Clear imaginary part
    }
    
    fft.performRealOnlyForwardTransform(fftData.data(), true);
    
    // Calculate magnitudes
    for (int i = 0; i < fftSize / 2; ++i)
    {
        float real = fftData[i];
        float imag = fftData[i + fftSize];
        fftData[i] = std::sqrt(real * real + imag * imag);
    }
}

void AudioVisualizer::drawEQResponse(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    float centerY = height * 0.5f;
    
    // Frequency grid
    g.setColour(juce::Colours::darkgrey);
    float frequencies[] = { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    for (float freq : frequencies)
    {
        float x = std::log10(freq / minFreq) * width / std::log10(maxFreq / minFreq);
        g.drawVerticalLine(int(x), 0.0f, height);
        
        // Frequency labels
        g.setFont(12.0f);
        juce::String label = (freq >= 1000) ? juce::String(freq / 1000) + "k" : juce::String(freq);
        g.drawText(label, int(x) - 20, int(height) - 20, 40, 20, juce::Justification::centred);
    }
    
    // 0dB line
    g.setColour(juce::Colours::darkgrey);
    g.drawHorizontalLine(int(centerY), 0.0f, width);
    
    // EQ response curve
    if (getFrequencyResponse)
    {
        g.setColour(juce::Colours::orange);
        juce::Path responsePath;
        bool pathStarted = false;
        
        for (float x = 0; x < width; x += 1.0f)
        {
            float freq = minFreq * std::pow(maxFreq / minFreq, x / width);
            float response = getFrequencyResponse(freq);
            float y = centerY - response * height * 0.25f; // Scale by 0.25 to show ±12dB range
            
            if (!pathStarted)
            {
                responsePath.startNewSubPath(x, y);
                pathStarted = true;
            }
            else
            {
                responsePath.lineTo(x, y);
            }
        }
        
        g.strokePath(responsePath, juce::PathStrokeType(2.0f));
    }
}

void AudioVisualizer::timerCallback()
{
    repaint();
}

void AudioVisualizer::resized()
{
    
} 
