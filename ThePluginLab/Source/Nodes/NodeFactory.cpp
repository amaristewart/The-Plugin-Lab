#include "NodeFactory.h"
#include "EqualizerNode.h"
#include "CompressorNode.h"
#include "GuiNode.h"
#include "../Common/Features.h"

std::unique_ptr<juce::Component> NodeFactory::createNode(NodeType type)
{
    switch (type)
    {
        case NodeType::GuiControl:
            return std::make_unique<GuiNode>();
            
        case NodeType::Equalizer:
            return std::make_unique<EqualizerNode>();
            
        case NodeType::Compressor:
            return std::make_unique<CompressorNode>();
            
        default:
            // Fix: Don't return PluginNodeComponent directly if it's an abstract class
            // Instead return a concrete implementation or nullptr
            return nullptr;
    }
}

std::unique_ptr<juce::Component> NodeFactory::createNodeFromPreset(const juce::String& presetName)
{
    // For the prototype, we can create nodes with preset configurations
    if (presetName == "Default EQ")
    {
        auto node = std::make_unique<EqualizerNode>();
        // Additional preset configuration can be done here
        return node;
    }
    else if (presetName == "Vocal Compressor")
    {
        auto node = std::make_unique<CompressorNode>();
        // Set up compressor parameters for vocals
        return node;
    }
    
    // If no matching preset, return null
    return nullptr;
}
