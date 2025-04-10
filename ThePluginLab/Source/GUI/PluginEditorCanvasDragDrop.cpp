#include "PluginEditorCanvas.h"
#include "../Components/PluginNodeComponent.h"
#include "../Components/ConcretePluginNode.h"
#include "../Components/ConnectionComponent.h"
#include "../Connections/AudioConnectionPoint.h"
#include "../Audio/Graphs/AudioProcessingGraph.h"
#include "../Common/Features.h"
#include "../Common/Types.h"

// Drag and drop implementation
bool PluginEditorCanvas::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& details)
{
    // Accept all drag sources from our component panel
    return true;
}

void PluginEditorCanvas::itemDropped(const juce::DragAndDropTarget::SourceDetails& details)
{
    // Convert the description to a ComponentType
    int typeValue = details.description.toString().getIntValue();
    ComponentType type = static_cast<ComponentType>(typeValue);
    
    // Create a node based on the component type
    std::unique_ptr<ConcretePluginNode> newNode;
    
    if (type == ComponentType::Knob ||
        type == ComponentType::Slider ||
        type == ComponentType::Button ||
        type == ComponentType::Label)
    {
        // GUI components
        newNode = std::make_unique<ConcretePluginNode>("GUI Control", Features::guiColor);
    }
    else if (type == ComponentType::EQLowpass ||
             type == ComponentType::EQHighpass ||
             type == ComponentType::EQBandpass ||
             type == ComponentType::EQNotch ||
             type == ComponentType::EQPeaking)
    {
        // EQ components
        newNode = std::make_unique<ConcretePluginNode>("Equalizer", Features::eqColor);
    }
    else if (type == ComponentType::CompThreshold ||
             type == ComponentType::CompRatio ||
             type == ComponentType::CompAttack ||
             type == ComponentType::CompRelease)
    {
        // Compressor components
        newNode = std::make_unique<ConcretePluginNode>("Compressor", Features::compColor);
    }
    
    if (newNode != nullptr)
    {
        // Add input and output ports
        newNode->addInputPort("Input");
        newNode->addOutputPort("Output");
        
        // Set size
        newNode->setSize(150, 100);
        
        // Position the node at the drop location
        newNode->setBounds(details.localPosition.x - newNode->getWidth() / 2,
                         details.localPosition.y - newNode->getHeight() / 2,
                         newNode->getWidth(),
                         newNode->getHeight());
        
        // Hide tutorial overlay when first node is added
        showTutorialOverlay = false;
        
        // Add the node to the canvas
        addAndMakeVisible(newNode.get());
        
        // Store a pointer to the node before moving it
        ConcretePluginNode* nodePtr = newNode.get();
        
        // Add to the nodes array - make sure this matches your declaration in PluginEditorCanvas.h
        nodes.add(newNode.release());
        
        // Notify listeners
        if (onNodeAdded)
            onNodeAdded(nodePtr);
    }
}
