#include "AudioPort.h"

AudioPort::AudioPort(const juce::String& name, ConnectionPointType type, int index)
    : portName(name), portType(type), portIndex(index), connectedPort(nullptr)
{
    // Initialize the audio port
}

AudioPort::~AudioPort()
{
    // Disconnect if connected
    disconnect();
}

void AudioPort::connectTo(AudioPort* destination)
{
    if (destination != nullptr && destination->getType() != portType)
    {
        // Disconnect any existing connections
        disconnect();
        
        // Connect to the new destination
        connectedPort = destination;
        
        // Make sure the destination is connected back to us
        if (destination->getConnectedPort() != this)
        {
            destination->connectTo(this);
        }
    }
}

void AudioPort::disconnect()
{
    if (connectedPort != nullptr)
    {
        // Store a reference to the connected port
        AudioPort* connected = connectedPort;
        
        // Clear our connection
        connectedPort = nullptr;
        
        // Disconnect the other port if it's still connected to us
        if (connected->getConnectedPort() == this)
        {
            // Temporarily set our connection to null to avoid infinite recursion
            connected->disconnect();
        }
    }
}
