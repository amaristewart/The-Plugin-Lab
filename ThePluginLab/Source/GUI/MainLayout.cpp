#include "MainLayout.h"

MainLayout::MainLayout()
{
    // Create main components
    canvas = std::make_unique<PluginEditorCanvas>();
    componentPanel = std::make_unique<ComponentPanel>();
    
    // Add components to the layout
    addAndMakeVisible(*canvas);
    addAndMakeVisible(*componentPanel);
    
    // Configure layout
    setSize(1024, 768);
    
    // Set up callbacks for nodes and connections
    canvas->onNodeAdded = [this](PluginNodeComponent* node) {
        handleNodeAdded(node);
    };
    
    canvas->onNodeRemoved = [this](PluginNodeComponent* node) {
        handleNodeRemoved(node);
    };
    
    canvas->onConnectionMade = [this](AudioConnectionPoint* source, AudioConnectionPoint* destination) {
        handleConnectionMade(source, destination);
    };
    
    canvas->onConnectionRemoved = [this](ConnectionComponent* connection) {
        handleConnectionRemoved(connection);
    };
}

MainLayout::~MainLayout()
{
    // Clean up resources
}

void MainLayout::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainLayout::resized()
{
    auto bounds = getLocalBounds();
    
    // Component panel on the left
    auto panelWidth = 250;
    componentPanel->setBounds(bounds.removeFromLeft(panelWidth));
    
    // Canvas takes the rest of the space
    canvas->setBounds(bounds);
}

void MainLayout::createNode(ComponentType type)
{
    std::unique_ptr<ConcretePluginNode> newNode;
    
    switch (type)
    {
        // GUI Components
        case ComponentType::Knob:
        case ComponentType::Slider:
        case ComponentType::Button:
        case ComponentType::Label:
            newNode = std::make_unique<ConcretePluginNode>("GUI Control", Features::guiColor);
            break;
            
        // EQ Components
        case ComponentType::EQLowpass:
        case ComponentType::EQHighpass:
        case ComponentType::EQBandpass:
        case ComponentType::EQNotch:
        case ComponentType::EQPeaking:
            newNode = std::make_unique<ConcretePluginNode>("Equalizer", Features::eqColor);
            break;
            
        // Compressor Components
        case ComponentType::CompThreshold:
        case ComponentType::CompRatio:
        case ComponentType::CompAttack:
        case ComponentType::CompRelease:
            newNode = std::make_unique<ConcretePluginNode>("Compressor", Features::compColor);
            break;
            
        default:
            break;
    }
    
    if (newNode != nullptr)
    {
        // Position the node in the center of the canvas
        auto canvasBounds = canvas->getBounds();
        newNode->setBounds(canvasBounds.getCentreX() - newNode->getWidth() / 2,
                         canvasBounds.getCentreY() - newNode->getHeight() / 2,
                         newNode->getWidth(),
                         newNode->getHeight());
        
        canvas->addNode(std::move(newNode));
    }
}

void MainLayout::handleNodeAdded(PluginNodeComponent* node)
{
    // Handle node added event
    if (canvas->getProcessingGraph() != nullptr)
    {
        // Add the node to the audio processing graph
    }
}

void MainLayout::handleNodeRemoved(PluginNodeComponent* node)
{
    // Handle node removed event
    if (canvas->getProcessingGraph() != nullptr)
    {
        // Remove the node from the audio processing graph
    }
}

void MainLayout::handleConnectionMade(AudioConnectionPoint* source, AudioConnectionPoint* destination)
{
    // Handle connection made event
    if (canvas->getProcessingGraph() != nullptr && source != nullptr && destination != nullptr)
    {
        // Connect nodes in the audio processing graph
        PluginNodeComponent* sourceNode = &source->getOwner();
        PluginNodeComponent* destNode = &destination->getOwner();
        
        if (sourceNode != nullptr && destNode != nullptr)
        {
            int sourceChannel = source->getPortIndex();
            int destChannel = destination->getPortIndex();
            
            // Use processors directly if getNodeId doesn't exist
            canvas->getProcessingGraph()->connectProcessors(
                sourceNode->getProcessor(),
                sourceChannel,
                destNode->getProcessor(),
                destChannel
            );
        }
    }
}

void MainLayout::handleConnectionRemoved(ConnectionComponent* connection)
{
    // Handle connection removed event
    if (connection != nullptr && canvas->getProcessingGraph() != nullptr)
    {
        auto source = connection->getSourcePoint();
        auto dest = connection->getDestinationPoint();
        
        if (source != nullptr && dest != nullptr)
        {
            PluginNodeComponent* sourceNode = &source->getOwner();
            PluginNodeComponent* destNode = &dest->getOwner();
            
            if (sourceNode != nullptr && destNode != nullptr)
            {
                int sourceChannel = source->getPortIndex();
                int destChannel = dest->getPortIndex();
                
                // Use the modified getNodeId() method from PluginNodeComponent
                canvas->getProcessingGraph()->disconnectNodes(
                    sourceNode->getNodeId(),
                    destNode->getNodeId()
                );
            }
        }
    }
}
