#include "GuiControlAudioProcessor.h"

GuiControlAudioProcessor::GuiControlAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo())
        .withOutput("Output", juce::AudioChannelSet::stereo())),
      controlValue(0.5f)
{
    // No initialization needed for prototype
}

GuiControlAudioProcessor::~GuiControlAudioProcessor()
{
}

void GuiControlAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // No preparation needed for prototype
}

void GuiControlAudioProcessor::releaseResources()
{
    // Nothing to release
}

void GuiControlAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // For prototype, we don't need to modify the buffer
}

void GuiControlAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(controlValue);
}

void GuiControlAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    controlValue = stream.readFloat();
}

void GuiControlAudioProcessor::setValue(float newValue)
{
    controlValue = newValue;
}

float GuiControlAudioProcessor::getValue() const
{
    return controlValue;
}
