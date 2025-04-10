#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

// Forward declarations
class PluginNodeComponent;

class AudioPort
{
public:
    AudioPort(const juce::String& name, ConnectionPointType type, int index);
    ~AudioPort();
    
    // Getters
    juce::String getName() const { return portName; }
    ConnectionPointType getType() const { return portType; }
    int getIndex() const { return portIndex; }
    
    // Audio processing
    void connectTo(AudioPort* destination);
    void disconnect();
    bool isConnected() const { return connectedPort != nullptr; }
    AudioPort* getConnectedPort() const { return connectedPort; }
    
private:
    juce::String portName;
    ConnectionPointType portType;
    int portIndex;
    AudioPort* connectedPort = nullptr;
};
