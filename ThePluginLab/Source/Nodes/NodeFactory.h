#pragma once
#include <JuceHeader.h>
#include "../Components/PluginNodeComponent.h"
#include "../Common/Types.h"

class NodeFactory
{
public:
    // Create a Component-based node for the GUI
    static std::unique_ptr<juce::Component> createNode(NodeType type);
    
    // Create a node from a preset
    static std::unique_ptr<juce::Component> createNodeFromPreset(const juce::String& presetName);
};
