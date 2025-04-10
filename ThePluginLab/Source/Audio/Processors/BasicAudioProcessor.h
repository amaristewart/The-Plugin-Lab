#pragma once
#include <JuceHeader.h>

/**
 * Extremely simplified audio processor for GUI prototype only
 * No actual audio processing or JUCE processor inheritance
 */
class BasicAudioProcessor 
{
public:
    BasicAudioProcessor() {}
    ~BasicAudioProcessor() {}

    // Simple mockup methods that would be used by the GUI
    void setParameter(const juce::String& name, float value) {
        parameters[name] = value;
    }
    
    float getParameter(const juce::String& name) const {
        auto it = parameters.find(name);
        return (it != parameters.end()) ? it->second : 0.0f;
    }
    
    void setBypass(bool shouldBypass) {
        bypassed = shouldBypass;
    }
    
    bool isBypassed() const {
        return bypassed;
    }
    
private:
    std::map<juce::String, float> parameters;
    bool bypassed = false;
};
