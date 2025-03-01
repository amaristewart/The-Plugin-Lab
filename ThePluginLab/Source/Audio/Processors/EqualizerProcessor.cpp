#include "EqualizerProcessor.h"
#include <cmath>

//==============================================================================
// EqualizerBand implementation
void EqualizerBand::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    updateCoefficients();
    x1 = x2 = y1 = y2 = 0.0f;
}

void EqualizerBand::processBlock(float* data, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        float x0 = data[i];
        float y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        
        x2 = x1;
        x1 = x0;
        y2 = y1;
        y1 = y0;
        
        data[i] = y0;
    }
}

void EqualizerBand::setFrequency(float newFreq)
{
    frequency = newFreq;
    updateCoefficients();
}

void EqualizerBand::setGain(float newGain)
{
    gain = newGain;
    updateCoefficients();
}

void EqualizerBand::setQ(float newQ)
{
    q = newQ;
    updateCoefficients();
}

float EqualizerBand::getFrequencyResponse(float freq) const
{
    // Simple implementation - would need a proper calculation for accuracy
    return gain * q * frequency / (std::sqrt(std::pow(frequency, 2.0f) + std::pow(q, 2.0f)));
}

void EqualizerBand::updateCoefficients()
{
    const float omega = 2.0f * juce::MathConstants<float>::pi * frequency / static_cast<float>(currentSampleRate);
    const float alpha = std::sin(omega) / (2.0f * q);
    const float A = std::pow(10.0f, gain / 40.0f);
    const float cosOmega = std::cos(omega);
    
    const float alpha_A = alpha * A;
    const float alpha_div_A = alpha / A;
    
    const float b0_temp = 1.0f + alpha_A;
    const float b1_temp = -2.0f * cosOmega;
    const float b2_temp = 1.0f - alpha_A;
    const float a0 = 1.0f + alpha_div_A;
    
    b0 = b0_temp / a0;
    b1 = b1_temp / a0;
    b2 = b2_temp / a0;
    a1 = b1_temp / a0;
    a2 = (1.0f - alpha_div_A) / a0;
}

//==============================================================================
// EqualizerProcessor implementation
EqualizerProcessor::EqualizerProcessor()
    : PluginAudioProcessor()  // Call base class constructor
{
    updateCoefficients();
}

void EqualizerProcessor::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    updateCoefficients();
    state.fill(0.0f);
}

void EqualizerProcessor::releaseResources()
{
    state.fill(0.0f);
}

void EqualizerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    if (bypassed)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Store input level
    float inputSum = 0.0f;
    for (int channel = 0; channel < numChannels; ++channel)
        inputSum += buffer.getRMSLevel(channel, 0, numSamples);
    inputLevel.store(juce::Decibels::gainToDecibels(inputSum / numChannels));

    // Process each channel
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float x0 = channelData[i];
            float y0 = coefficients[0] * x0 + coefficients[1] * state[0] + coefficients[2] * state[1]
                      - coefficients[3] * state[2] - coefficients[4] * state[3];
            
            state[1] = state[0];
            state[0] = x0;
            state[3] = state[2];
            state[2] = y0;
            
            channelData[i] = y0;
        }
    }

    // Store output level
    float outputSum = 0.0f;
    for (int channel = 0; channel < numChannels; ++channel)
        outputSum += buffer.getRMSLevel(channel, 0, numSamples);
    outputLevel.store(juce::Decibels::gainToDecibels(outputSum / numChannels));
}

void EqualizerProcessor::updateCoefficients()
{
    const double omega = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
    const double sinOmega = std::sin(omega);
    const double cosOmega = std::cos(omega);
    const double alpha = sinOmega / (2.0 * quality);
    const double A = std::pow(10.0, gain / 40.0);
    
    double b0, b1, b2, a0, a1, a2;
    
    switch (filterType)
    {
        case EQType::Lowpass:
            b0 = (1.0 - cosOmega) / 2.0;
            b1 = 1.0 - cosOmega;
            b2 = (1.0 - cosOmega) / 2.0;
            a0 = 1.0 + alpha;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - alpha;
            break;
        case EQType::Highpass:
            b0 = (1.0 + cosOmega) / 2.0;
            b1 = -(1.0 + cosOmega);
            b2 = (1.0 + cosOmega) / 2.0;
            a0 = 1.0 + alpha;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - alpha;
            break;
        case EQType::Bandpass:
            b0 = sinOmega / 2.0;
            b1 = 0.0;
            b2 = -sinOmega / 2.0;
            a0 = 1.0 + alpha;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - alpha;
            break;
        case EQType::Notch:
            b0 = 1.0;
            b1 = -2.0 * cosOmega;
            b2 = 1.0;
            a0 = 1.0 + alpha;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - alpha;
            break;
        case EQType::Peaking:
            b0 = 1.0 + alpha * A;
            b1 = -2.0 * cosOmega;
            b2 = 1.0 - alpha * A;
            a0 = 1.0 + alpha / A;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - alpha / A;
            break;
            
        default:
            return;
    }
    
    // Normalize coefficients
    const double oneOverA0 = 1.0 / a0;
    coefficients[0] = static_cast<float>(b0 * oneOverA0);
    coefficients[1] = static_cast<float>(b1 * oneOverA0);
    coefficients[2] = static_cast<float>(b2 * oneOverA0);
    coefficients[3] = static_cast<float>(a1 * oneOverA0);
    coefficients[4] = static_cast<float>(a2 * oneOverA0);
}

void EqualizerProcessor::setFilterType(EQType type)
{
    filterType = type;
    updateCoefficients();
}

void EqualizerProcessor::setFrequency(float freq)
{
    frequency = freq;
    updateCoefficients();
}

void EqualizerProcessor::setGain(float gainDb)
{
    gain = gainDb;
    updateCoefficients();
}

void EqualizerProcessor::setQ(float q)
{
    quality = q;
    updateCoefficients();
}

void EqualizerProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(frequency);
    stream.writeFloat(gain);
    stream.writeFloat(quality);
    stream.writeInt(static_cast<int>(filterType));
    stream.writeBool(bypassed);
}

void EqualizerProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    frequency = stream.readFloat();
    gain = stream.readFloat();
    quality = stream.readFloat();
    filterType = static_cast<EQType>(stream.readInt());
    bypassed = stream.readBool();
    
    updateCoefficients();
}
