#pragma once
#include <JuceHeader.h>

// Forward declare DraggableComponent to avoid circular dependencies
class DraggableComponent;

// Component types
enum class ComponentType
{
    // GUI Components
    Knob,
    Slider,
    Button,
    Label,
    
    // EQ Components
    EQLowpass,
    EQHighpass,
    EQBandpass,
    EQNotch,
    EQPeaking,
    EQFrequency,
    EQGain,
    EQQ,
    EQSlope,
    
    // Dynamics Components
    DynamicType,
    CompThreshold,
    CompRatio,
    CompAttack,
    CompRelease,
    
    // Misc
    Bypass
};

enum class DynamicType
{
    Compressor,
    Limiter,
    Expander,
    Gate
};

enum class EQType
{
    Lowpass,
    Highpass,
    Bandpass,
    Notch,
    Peaking
};

enum class NodeType
{
    Input,
    Output,
    Equalizer,
    Compressor,
    CompThreshold,
    CompRatio,
    CompAttack,
    CompRelease,
    Delay,
    Reverb,
    GUI,
    Knob,
    Slider,
    Button,
    Label,
    Unknown
};

enum class ConnectionPointType
{
    Input,
    Output
};

inline NodeType componentTypeToNodeType(ComponentType type)
{
    switch (type)
    {
        case ComponentType::Knob: return NodeType::Knob;
        case ComponentType::Slider: return NodeType::Slider;
        case ComponentType::Button: return NodeType::Button;
        case ComponentType::Label: return NodeType::Label;
        case ComponentType::EQLowpass: return NodeType::Equalizer;
        case ComponentType::EQHighpass: return NodeType::Equalizer;
        case ComponentType::EQBandpass: return NodeType::Equalizer;
        case ComponentType::EQNotch: return NodeType::Equalizer;
        case ComponentType::EQPeaking: return NodeType::Equalizer;
        case ComponentType::EQFrequency: return NodeType::Equalizer;
        case ComponentType::EQGain: return NodeType::Equalizer;
        case ComponentType::EQQ: return NodeType::Equalizer;
        case ComponentType::EQSlope: return NodeType::Equalizer;
        case ComponentType::DynamicType: return NodeType::Compressor;
        case ComponentType::CompThreshold: return NodeType::CompThreshold;
        case ComponentType::CompRatio: return NodeType::CompRatio;
        case ComponentType::CompAttack: return NodeType::CompAttack;
        case ComponentType::CompRelease: return NodeType::CompRelease;
        default: return NodeType::Unknown;
    }
}
