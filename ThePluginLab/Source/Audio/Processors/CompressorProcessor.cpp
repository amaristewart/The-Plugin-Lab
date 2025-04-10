#include "CompressorProcessor.h"

CompressorProcessor::CompressorProcessor()
{
    // Initialize the processor
}

CompressorProcessor::~CompressorProcessor()
{
    // Clean up resources
}

void CompressorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare DSP chain
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    compressor.prepare(spec);
    gain.prepare(spec);
    
    // Set initial parameters
    compressor.setThreshold(threshold);
    compressor.setRatio(ratio);
    compressor.setAttack(attack);
    compressor.setRelease(release);
    gain.setGainDecibels(makeupGain);
}

void CompressorProcessor::releaseResources()
{
    // Release resources when no longer playing
}

void CompressorProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Make a copy of the buffer before compression to calculate gain reduction
    juce::AudioBuffer<float> inputCopy;
    inputCopy.makeCopyOf(buffer);
    
    // Process with the compressor and makeup gain
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    compressor.process(context);
    gain.process(context);
    
    // Manually calculate gain reduction by comparing before/after levels
    float maxInputLevel = 0.0f;
    float maxOutputLevel = 0.0f;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            maxInputLevel = std::max(maxInputLevel, std::abs(inputCopy.getSample(channel, sample)));
            maxOutputLevel = std::max(maxOutputLevel, std::abs(buffer.getSample(channel, sample)));
        }
    }
    
    // Convert to dB
    float inputLevelDb = maxInputLevel > 0.0f ? 20.0f * std::log10(maxInputLevel) : -100.0f;
    float outputLevelDb = maxOutputLevel > 0.0f ? 20.0f * std::log10(maxOutputLevel) : -100.0f;
    
    // Calculate gain reduction (without makeup gain)
    currentGainReduction = inputLevelDb - outputLevelDb + makeupGain;
    
    // Ensure gain reduction is positive (it's a reduction)
    currentGainReduction = std::max(0.0f, currentGainReduction);
}

juce::AudioProcessorEditor* CompressorProcessor::createEditor()
{
    return nullptr; // No custom editor for now
}

bool CompressorProcessor::hasEditor() const
{
    return false;
}

const juce::String CompressorProcessor::getName() const
{
    return "Compressor";
}

int CompressorProcessor::getNumPrograms()
{
    return 1;
}

int CompressorProcessor::getCurrentProgram()
{
    return 0;
}

void CompressorProcessor::setCurrentProgram(int index)
{
    // We only have one program
}

const juce::String CompressorProcessor::getProgramName(int index)
{
    return "Default";
}

void CompressorProcessor::changeProgramName(int index, const juce::String& newName)
{
    // Not implemented
}

void CompressorProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Store compressor parameters
    juce::MemoryOutputStream stream(destData, true);
    
    stream.writeFloat(threshold);
    stream.writeFloat(ratio);
    stream.writeFloat(attack);
    stream.writeFloat(release);
    stream.writeFloat(makeupGain);
}

void CompressorProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore compressor parameters
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    
    threshold = stream.readFloat();
    ratio = stream.readFloat();
    attack = stream.readFloat();
    release = stream.readFloat();
    makeupGain = stream.readFloat();
    
    // Update DSP parameters
    compressor.setThreshold(threshold);
    compressor.setRatio(ratio);
    compressor.setAttack(attack);
    compressor.setRelease(release);
    gain.setGainDecibels(makeupGain);
}

void CompressorProcessor::setThreshold(float thresholdDb)
{
    threshold = thresholdDb;
    compressor.setThreshold(threshold);
}

void CompressorProcessor::setRatio(float newRatio)
{
    ratio = newRatio;
    compressor.setRatio(ratio);
}

void CompressorProcessor::setAttack(float attackMs)
{
    attack = attackMs;
    compressor.setAttack(attack);
}

void CompressorProcessor::setRelease(float releaseMs)
{
    release = releaseMs;
    compressor.setRelease(release);
}

void CompressorProcessor::setMakeupGain(float gainDb)
{
    makeupGain = gainDb;
    gain.setGainDecibels(makeupGain);
}

float CompressorProcessor::getGainReduction() const
{
    return currentGainReduction;
}

// MIDI handling methods
bool CompressorProcessor::acceptsMidi() const { return false; }
bool CompressorProcessor::producesMidi() const { return false; }
bool CompressorProcessor::isMidiEffect() const { return false; }
double CompressorProcessor::getTailLengthSeconds() const { return 0.0; }
