#pragma once
#include <JuceHeader.h>
#include "Types.h"

/**
 * Centralized location for helper functions to avoid duplication
 */
namespace Helpers
{
    /**
     * Convert a component type to a human-readable name
     */
    inline juce::String getComponentName(ComponentType type)
    {
        switch (type)
        {
            case ComponentType::Knob:      return "Knob";
            case ComponentType::Slider:    return "Slider";
            case ComponentType::Button:    return "Button";
            case ComponentType::Label:     return "Label";
            case ComponentType::EQLowpass: return "Low Pass Filter";
            case ComponentType::EQHighpass: return "High Pass Filter";
            case ComponentType::EQBandpass: return "Band Pass Filter";
            case ComponentType::EQNotch:    return "Notch Filter";
            case ComponentType::EQPeaking:  return "Peaking EQ";
            case ComponentType::CompThreshold: return "Compressor Threshold";
            case ComponentType::CompRatio:     return "Compressor Ratio";
            case ComponentType::CompAttack:    return "Compressor Attack";
            case ComponentType::CompRelease:   return "Compressor Release";
            case ComponentType::DynamicType:   return "Dynamics";
            default:                       return "Unknown";
        }
    }
    
    /**
     * Get color for a component type
     */
    inline juce::Colour getComponentColor(ComponentType type)
    {
        // Use existing colors from Features namespace
        using namespace Features;
        
        if (type == ComponentType::Knob || 
            type == ComponentType::Slider || 
            type == ComponentType::Button || 
            type == ComponentType::Label)
            return guiColor;
            
        if (type == ComponentType::EQLowpass ||
            type == ComponentType::EQHighpass ||
            type == ComponentType::EQBandpass ||
            type == ComponentType::EQNotch ||
            type == ComponentType::EQPeaking)
            return eqColor;
            
        if (type == ComponentType::CompThreshold ||
            type == ComponentType::CompRatio ||
            type == ComponentType::CompAttack ||
            type == ComponentType::CompRelease ||
            type == ComponentType::DynamicType)
            return compColor;
            
        return juce::Colours::grey;
    }
}
