#include "CompressorProcessor.h"

CompressorProcessor::CompressorProcessor()
    : PluginAudioProcessor() // Call base class constructor
{
    // Initialize default compressor state
}

void CompressorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    // Set up attack and release time constants based on sample rate
    attackCoeff = std::exp(-1.0 / (attackTime * 0.001 * sampleRate));
    releaseCoeff = std::exp(-1.0 / (releaseTime * 0.001 * sampleRate));

    // Clear any previous state
    envelopeLevel = 0.0f;
    currentGainReduction = 0.0f;
    holdCounter = 0;
    maxHoldSamples = static_cast<int>(holdTime * 0.001 * sampleRate);
}

void CompressorProcessor::releaseResources()
{
    // Free resources when audio device is stopped
}

void CompressorProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassed)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Calculate input level for metering
    float inputSum = 0.0f;
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        for (int i = 0; i < numSamples; ++i)
        {
            inputSum += channelData[i] * channelData[i];
        }
    }
    
    if (numSamples > 0 && numChannels > 0)
    {
        float rmsLevel = std::sqrt(inputSum / (numSamples * numChannels));
        inputLevel.store(juce::Decibels::gainToDecibels(rmsLevel, -60.0f));
    }
    
    // Process dynamic range processing
    switch (currentType)
    {
        case DynamicType::Compressor:
            processCompressor(buffer);
            break;
            
        case DynamicType::Limiter:
            processLimiter(buffer);
            break;
            
        case DynamicType::Expander:
            processExpander(buffer);
            break;
            
        case DynamicType::Gate:
            processGate(buffer);
            break;
    }
    
    // Calculate output level for metering
    float outputSum = 0.0f;
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        for (int i = 0; i < numSamples; ++i)
        {
            outputSum += channelData[i] * channelData[i];
        }
    }
    
    if (numSamples > 0 && numChannels > 0)
    {
        float rmsLevel = std::sqrt(outputSum / (numSamples * numChannels));
        outputLevel.store(juce::Decibels::gainToDecibels(rmsLevel, -60.0f));
    }
}

void CompressorProcessor::processCompressor(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    const float thresholdLinear = juce::Decibels::decibelsToGain(threshold);
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Find the maximum sample for all channels for this sample position
        float inputSample = 0.0f;
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = std::abs(buffer.getSample(channel, i));
            inputSample = std::max(inputSample, sample);
        }
        
        // Calculate envelope (smoothed abs value for detection)
        if (inputSample > envelopeLevel)
            envelopeLevel = attackCoeff * envelopeLevel + (1.0f - attackCoeff) * inputSample;
        else
            envelopeLevel = releaseCoeff * envelopeLevel + (1.0f - releaseCoeff) * inputSample;
        
        // Calculate gain reduction
        float gainReduction = 1.0f;
        if (envelopeLevel > thresholdLinear)
        {
            // Compressor formula: gain = threshold * (level/threshold)^(1/ratio - 1)
            gainReduction = thresholdLinear * std::pow(envelopeLevel / thresholdLinear, 1.0f/ratio - 1.0f) / envelopeLevel;
        }
        
        // Smooth gain reduction to avoid artifacts
        const float alphaAttack = 0.9f;  // Smooth coefficient
        currentGainReduction = alphaAttack * currentGainReduction + (1.0f - alphaAttack) * gainReduction;
        
        // Apply gain reduction to all channels
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = buffer.getSample(channel, i);
            buffer.setSample(channel, i, sample * currentGainReduction);
        }
    }
    
    // Store gain reduction for metering (in dB)
    gainReduction.store(juce::Decibels::gainToDecibels(currentGainReduction));
}

void CompressorProcessor::processLimiter(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    const float thresholdLinear = juce::Decibels::decibelsToGain(threshold);
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Find maximum sample across all channels
        float maxSample = 0.0f;
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = std::abs(buffer.getSample(channel, i));
            maxSample = std::max(maxSample, sample);
        }
        
        // Calculate envelope for peak detection
        envelopeLevel = std::max(maxSample, releaseCoeff * envelopeLevel);
        
        // Calculate gain reduction for limiter (hard limiting)
        float gainReduction = 1.0f;
        if (envelopeLevel > thresholdLinear)
            gainReduction = thresholdLinear / envelopeLevel;
            
        // Apply gain reduction to all channels
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = buffer.getSample(channel, i);
            buffer.setSample(channel, i, sample * gainReduction);
        }
        
        // Update gain reduction for metering
        currentGainReduction = gainReduction;
    }
    
    // Store gain reduction for metering (in dB)
    gainReduction.store(juce::Decibels::gainToDecibels(currentGainReduction));
}

void CompressorProcessor::processExpander(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    const float thresholdLinear = juce::Decibels::decibelsToGain(threshold);
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Find the maximum sample for all channels
        float inputSample = 0.0f;
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = std::abs(buffer.getSample(channel, i));
            inputSample = std::max(inputSample, sample);
        }
        
        // Calculate envelope (smoothed abs value)
        if (inputSample > envelopeLevel)
            envelopeLevel = attackCoeff * envelopeLevel + (1.0f - attackCoeff) * inputSample;
        else
            envelopeLevel = releaseCoeff * envelopeLevel + (1.0f - releaseCoeff) * inputSample;
        
        // Calculate gain reduction for expander
        float gainReduction = 1.0f;
        if (envelopeLevel < thresholdLinear)
        {
            // Expander formula inverts the compressor ratio relationship
            gainReduction = std::pow(envelopeLevel / thresholdLinear, ratio - 1.0f);
        }
        
        // Apply gain reduction to all channels
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = buffer.getSample(channel, i);
            buffer.setSample(channel, i, sample * gainReduction);
        }
        
        // Update gain reduction for metering
        currentGainReduction = gainReduction;
    }
    
    // Store gain reduction for metering (in dB)
    gainReduction.store(juce::Decibels::gainToDecibels(currentGainReduction));
}

void CompressorProcessor::processGate(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    const float thresholdLinear = juce::Decibels::decibelsToGain(threshold);
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Find the maximum sample for all channels
        float inputSample = 0.0f;
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = std::abs(buffer.getSample(channel, i));
            inputSample = std::max(inputSample, sample);
        }
        
        // Calculate envelope (smoothed abs value)
        if (inputSample > envelopeLevel)
            envelopeLevel = attackCoeff * envelopeLevel + (1.0f - attackCoeff) * inputSample;
        else
            envelopeLevel = releaseCoeff * envelopeLevel + (1.0f - releaseCoeff) * inputSample;
        
        // Calculate gate state
        float gainReduction;
        if (envelopeLevel >= thresholdLinear)
        {
            // Reset hold counter when signal is above threshold
            holdCounter = maxHoldSamples;
            gainReduction = 1.0f;  // Gate open
        }
        else
        {
            // Count down hold time
            if (holdCounter > 0)
            {
                holdCounter--;
                gainReduction = 1.0f;  // Gate still open during hold
            }
            else
            {
                gainReduction = 0.0f;  // Gate closed
            }
        }
        
        // Smooth gain changes to avoid clicks
        currentGainReduction = 0.9f * currentGainReduction + 0.1f * gainReduction;
        
        // Apply gain reduction to all channels
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = buffer.getSample(channel, i);
            buffer.setSample(channel, i, sample * currentGainReduction);
        }
    }
    
    // Store gain reduction for metering (in dB)
    gainReduction.store(juce::Decibels::gainToDecibels(currentGainReduction));
}

void CompressorProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    
    stream.writeFloat(threshold);
    stream.writeFloat(ratio);
    stream.writeFloat(attackTime);
    stream.writeFloat(releaseTime);
    stream.writeFloat(holdTime);
    stream.writeInt(static_cast<int>(currentType));
    stream.writeBool(bypassed);
}

void CompressorProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    
    threshold = stream.readFloat();
    ratio = stream.readFloat();
    attackTime = stream.readFloat();
    releaseTime = stream.readFloat();
    holdTime = stream.readFloat();
    currentType = static_cast<DynamicType>(stream.readInt());
    bypassed = stream.readBool();
    
    // Update derived parameters
    if (currentSampleRate > 0)
    {
        attackCoeff = std::exp(-1.0 / (attackTime * 0.001 * currentSampleRate));
        releaseCoeff = std::exp(-1.0 / (releaseTime * 0.001 * currentSampleRate));
        maxHoldSamples = static_cast<int>(holdTime * 0.001 * currentSampleRate);
    }
}

// Parameter setters
void CompressorProcessor::setDynamicsType(DynamicType type)
{
    currentType = type;
}

void CompressorProcessor::setThreshold(float thresholdDb)
{
    threshold = thresholdDb;
}

void CompressorProcessor::setRatio(float r)
{
    ratio = r;
}

void CompressorProcessor::setAttackTime(float attackMs)
{
    attackTime = attackMs;
    if (currentSampleRate > 0)
        attackCoeff = std::exp(-1.0 / (attackTime * 0.001 * currentSampleRate));
}

void CompressorProcessor::setReleaseTime(float releaseMs)
{
    releaseTime = releaseMs;
    if (currentSampleRate > 0)
        releaseCoeff = std::exp(-1.0 / (releaseTime * 0.001 * currentSampleRate));
}

void CompressorProcessor::setHoldTime(float holdMs)
{
    holdTime = holdMs;
    if (currentSampleRate > 0)
        maxHoldSamples = static_cast<int>(holdTime * 0.001 * currentSampleRate);
}

void CompressorProcessor::setBypassed(bool shouldBeBypassed)
{
    bypassed = shouldBeBypassed;
}

float CompressorProcessor::getGainReduction() const
{
    return -gainReduction.load();  // Return positive dB value for reduction
}
