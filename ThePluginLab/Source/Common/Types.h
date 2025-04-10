#pragma once
#include <JuceHeader.h>
#include "Forward.h"

// Enum for node types
enum class NodeType
{
    Generic,
    Equalizer,
    Compressor,
    GuiControl
};

// Enum for component types
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
    
    // Compressor Components
    DynamicType,
    CompThreshold,
    CompRatio,
    CompAttack,
    CompRelease
};

// Connection point types
enum class ConnectionPointType
{
    Input,
    Output
};

// EQ filter types
enum class EQType
{
    Lowpass,
    Highpass,
    Bandpass,
    Notch,
    Peaking,
    LowShelf,
    HighShelf
};

// Dynamic processor types
enum class DynamicType
{
    Linear,
    Logarithmic,
    Exponential,
    Compressor,
    Limiter,
    Expander,
    Gate
};

// Structs and other type definitions
struct PluginInfo
{
    juce::String name;
    juce::String type;
    juce::String manufacturer;
    
    bool operator==(const PluginInfo& other) const
    {
        return name == other.name && 
               type == other.type && 
               manufacturer == other.manufacturer;
    }
};

// Useful type aliases
using PluginInfoList = std::vector<PluginInfo>;

// Convert component type to appropriate node type
inline NodeType componentTypeToNodeType(ComponentType type)
{
    switch (type)
    {
        case ComponentType::Knob:
        case ComponentType::Slider:
        case ComponentType::Button:
        case ComponentType::Label:
            return NodeType::GuiControl;
            
        case ComponentType::EQLowpass:
        case ComponentType::EQHighpass:
        case ComponentType::EQBandpass:
        case ComponentType::EQNotch:
        case ComponentType::EQPeaking:
            return NodeType::Equalizer;
            
        case ComponentType::DynamicType:
        case ComponentType::CompThreshold:
        case ComponentType::CompRatio:
        case ComponentType::CompAttack:
        case ComponentType::CompRelease:
            return NodeType::Compressor;
            
        default:
            return NodeType::Generic;
    }
}
