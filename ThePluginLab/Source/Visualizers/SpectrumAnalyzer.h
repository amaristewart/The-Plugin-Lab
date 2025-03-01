#pragma once
#include <JuceHeader.h>

class SpectrumAnalyzer : public juce::Component,
                        public juce::Timer
{
public:
    SpectrumAnalyzer()
    {
        fft = std::make_unique<juce::dsp::FFT>(fftOrder);
        window = std::make_unique<juce::dsp::WindowingFunction<float>>(
            fftSize, juce::dsp::WindowingFunction<float>::hann);
            
        startTimerHz(30);
    }
    
    ~SpectrumAnalyzer() override
    {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        
        juce::Path spectrumPath;
        const auto bounds = getLocalBounds().toFloat();
        
        spectrumPath.startNewSubPath(bounds.getX(), bounds.getBottom());
        
        for (int i = 0; i < scopeSize; ++i)
        {
            const float freq = (sampleRate * i) / fftSize;
            const float x = bounds.getX() + bounds.getWidth() * 
                std::log10(freq / 20.0f) / std::log10(20000.0f / 20.0f);
            const float y = bounds.getY() + bounds.getHeight() * 
                (1.0f - (scopeData[i] + 100.0f) / 100.0f);
                
            spectrumPath.lineTo(x, y);
        }
        
        spectrumPath.lineTo(bounds.getRight(), bounds.getBottom());
        spectrumPath.closeSubPath();
        
        g.setGradientFill(juce::ColourGradient(
            juce::Colours::blue.withAlpha(0.7f), 0, bounds.getBottom(),
            juce::Colours::blue.withAlpha(0.2f), 0, bounds.getY(),
            false));
        g.fillPath(spectrumPath);
        
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.strokePath(spectrumPath, juce::PathStrokeType(1.0f));
    }
    
    void pushNextSampleIntoFifo(float sample) noexcept
    {
        if (fifoIndex == fftSize)
        {
            if (!nextFFTBlockReady)
            {
                juce::zeromem(fftData, sizeof(fftData));
                memcpy(fftData, fifo, sizeof(fifo));
                nextFFTBlockReady = true;
            }
            fifoIndex = 0;
        }
        fifo[fifoIndex++] = sample;
    }
    
    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            window->multiplyWithWindowingTable(fftData, fftSize);
            fft->performFrequencyOnlyForwardTransform(fftData);
            
            auto mindB = -100.0f;
            auto maxdB = 0.0f;
            
            for (int i = 0; i < scopeSize; ++i)
            {
                auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
                auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
                auto level = juce::jmap(juce::jlimit(mindB, maxdB, 
                    juce::Decibels::gainToDecibels(fftData[fftDataIndex]) 
                        - juce::Decibels::gainToDecibels((float)fftSize)),
                    mindB, maxdB, 0.0f, 1.0f);
                        
                scopeData[i] = level * 100.0f - 100.0f;
            }
            
            nextFFTBlockReady = false;
            repaint();
        }
    }
    
    void setSampleRate(double rate) { sampleRate = rate; }

private:
    static constexpr auto fftOrder = 11;
    static constexpr auto fftSize = 1 << fftOrder;
    static constexpr auto scopeSize = 512;
    
    std::unique_ptr<juce::dsp::FFT> fft;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
    
    float fifo[fftSize];
    float fftData[2 * fftSize];
    float scopeData[scopeSize];
    
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
    double sampleRate = 44100.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzer)
};
