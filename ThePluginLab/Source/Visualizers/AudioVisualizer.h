#pragma once
#include <JuceHeader.h>
#include <array>

class AudioVisualizer : public juce::Component,
                       public juce::Timer
{
public:
    AudioVisualizer();
    ~AudioVisualizer() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // To update the visualization
    void pushBuffer(const juce::AudioBuffer<float>& buffer);
    
    // Timer callback for animation
    void timerCallback() override;
    
    enum class VisualizationType
    {
        Waveform,
        Spectrum,
        EQResponse
    };
    
    void setVisualizationType(VisualizationType type) { visualizationType = type; }
    
    void setFrequencyResponseCallback(std::function<float(float)> callback) { getFrequencyResponse = callback; }
    
private:
    void drawWaveform(juce::Graphics& g);
    void drawSpectrum(juce::Graphics& g);
    void calculateFFT();
    void drawEQResponse(juce::Graphics& g);
    
    VisualizationType visualizationType = VisualizationType::Waveform;
    
    static constexpr int bufferSize = 1024;
    std::array<float, bufferSize> audioData{};
    int writePosition = 0;
    
    static constexpr int fftOrder = 11;  // 2048 points
    static constexpr int fftSize = 1 << fftOrder;
    
    juce::dsp::FFT fft;
    std::vector<float> fftData;
    std::vector<float> fftWindow;
    
    float maxLevel = 0.0f;
    juce::Colour gradientColours[4] = {
        juce::Colours::blue,
        juce::Colours::green,
        juce::Colours::yellow,
        juce::Colours::red
    };
    
    std::function<float(float)> getFrequencyResponse;
    
    // Frequency range for EQ visualization
    static constexpr float minFreq = 20.0f;
    static constexpr float maxFreq = 20000.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioVisualizer)
}; 
