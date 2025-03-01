#include "AudioConnection.h"
#include "AudioConnectionPoint.h"

AudioConnection::~AudioConnection() = default;

AudioConnection::AudioConnection(AudioConnectionPoint* source, AudioConnectionPoint* destination)
    : sourcePoint(source), destPoint(destination)
{
    updatePosition();
}

void AudioConnection::paint(juce::Graphics& g)
{
    auto start = sourcePoint->getBounds().getCentre();
    auto end = destPoint->getBounds().getCentre();

    g.setColour(juce::Colours::white);
    g.drawLine(start.x, start.y, end.x, end.y, 2.0f);
}

void AudioConnection::updatePosition()
{
    auto start = sourcePoint->getBounds().getCentre();
    auto end = destPoint->getBounds().getCentre();

    auto bounds = juce::Rectangle<int>::leftTopRightBottom(
        juce::jmin(start.x, end.x),
        juce::jmin(start.y, end.y),
        juce::jmax(start.x, end.x),
        juce::jmax(start.y, end.y)
    ).expanded(4);

    setBounds(bounds);
    repaint();
}

void AudioConnection::resized()
{
    updatePosition();
}
