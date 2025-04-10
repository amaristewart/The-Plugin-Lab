#include "PluginEditorCanvas.h"
#include "../Connections/AudioConnectionPoint.h"
#include "../Connections/ConnectionComponent.h"
#include "../Components/PluginNodeComponent.h"
#include "../Components/ConcretePluginNode.h"
#include "../Nodes/GuiNode.h"
#include "../Nodes/EqualizerNode.h"
#include "../Nodes/CompressorNode.h"
#include "../Audio/Graphs/AudioProcessingGraph.h"
#include "../Common/Features.h"

PluginEditorCanvas::PluginEditorCanvas()
{
    setOpaque(true);
    setSize(800, 600);
    
    // Enable drag and drop
    setInterceptsMouseClicks(true, true);
    
    // Initialize the processing graph
    processingGraph = std::make_unique<AudioProcessingGraph>();
}

PluginEditorCanvas::~PluginEditorCanvas()
{
    // Clean up resources
    clear();
}

void PluginEditorCanvas::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(Features::canvasBackgroundColor);
    
    // Draw grid
    drawGrid(g, Features::gridSize);
    
    // Draw tutorial overlay if needed
    if (showTutorialOverlay && nodes.isEmpty())
    {
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(juce::Font(24.0f));
        g.drawText("Drag components here to start building your plugin",
                  getLocalBounds(), juce::Justification::centred, true);
    }
    
    // Draw connection being dragged
    if (isDraggingConnection && selectedSource != nullptr)
    {
        g.setColour(juce::Colours::white);
        
        // Get the global position of the source point
        juce::Point<float> startPoint = selectedSource->getBounds().getCentre().toFloat();
        startPoint = selectedSource->localPointToGlobal(startPoint);
        startPoint = getLocalPoint(nullptr, startPoint);
        
        // Draw the connection line
        g.drawLine(startPoint.x, startPoint.y, dragEndPoint.x, dragEndPoint.y, 2.0f);
    }
}

void PluginEditorCanvas::resized()
{
    // Nothing to do here
}

void PluginEditorCanvas::drawGrid(juce::Graphics& g, int gridSize)
{
    g.setColour(juce::Colours::white.withAlpha(Features::gridOpacity));
    
    for (int x = 0; x < getWidth(); x += gridSize)
    {
        g.drawVerticalLine(x, 0.0f, static_cast<float>(getHeight()));
    }
    
    for (int y = 0; y < getHeight(); y += gridSize)
    {
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(getWidth()));
    }
}

void PluginEditorCanvas::addConnection(AudioConnectionPoint* source, AudioConnectionPoint* destination)
{
    if (source != nullptr && destination != nullptr)
    {
        // Create a new connection component
        auto* connection = new ConnectionComponent();
        connection->setSourcePoint(source);
        connection->setDestinationPoint(destination);
        
        addAndMakeVisible(connection);
        connections.add(connection);
        
        // Update connected state
        source->setConnected(true);
        destination->setConnected(true);
        
        // Notify listeners
        if (onConnectionMade)
            onConnectionMade(source, destination);
            
        repaint();
    }
}

void PluginEditorCanvas::removeConnection(ConnectionComponent* connection)
{
    if (connection != nullptr)
    {
        // Update connected state
        if (auto* source = connection->getSourcePoint())
            source->setConnected(false);
            
        if (auto* dest = connection->getDestinationPoint())
            dest->setConnected(false);
        
        // Notify listeners
        if (onConnectionRemoved)
            onConnectionRemoved(connection);
            
        connections.removeObject(connection);
        repaint();
    }
}

void PluginEditorCanvas::clear()
{
    // Remove all connections
    connections.clear();
    
    // Remove all nodes
    nodes.clear();
    
    // Reset state
    showTutorialOverlay = true;
    isDraggingConnection = false;
    selectedSource = nullptr;
    
    repaint();
}

void PluginEditorCanvas::saveToProject(juce::ValueTree& projectData)
{
    // Create nodes section
    juce::ValueTree nodesData("Nodes");
    
    for (int i = 0; i < nodes.size(); ++i)
    {
        auto* node = nodes[i];
        
        if (node != nullptr)
        {
            juce::ValueTree nodeData("Node");
            
            // Store node properties
            nodeData.setProperty("id", i, nullptr);
            nodeData.setProperty("type", static_cast<int>(node->getType()), nullptr);
            nodeData.setProperty("name", node->getName(), nullptr);
            nodeData.setProperty("x", node->getX(), nullptr);
            nodeData.setProperty("y", node->getY(), nullptr);
            nodeData.setProperty("width", node->getWidth(), nullptr);
            nodeData.setProperty("height", node->getHeight(), nullptr);
            
            nodesData.addChild(nodeData, -1, nullptr);
        }
    }
    
    // Create connections section
    juce::ValueTree connectionsData("Connections");
    
    for (auto* connection : connections)
    {
        if (connection != nullptr)
        {
            juce::ValueTree connectionData("Connection");
            
            // Find source and destination nodes
            auto* sourcePoint = connection->getSourcePoint();
            auto* destPoint = connection->getDestinationPoint();
            
            if (sourcePoint != nullptr && destPoint != nullptr)
            {
                auto* sourceNode = dynamic_cast<ConcretePluginNode*>(&sourcePoint->getOwner());
                auto* destNode = dynamic_cast<ConcretePluginNode*>(&destPoint->getOwner());
                
                if (sourceNode != nullptr && destNode != nullptr)
                {
                    // Find node indices
                    int sourceNodeIndex = nodes.indexOf(sourceNode);
                    int destNodeIndex = nodes.indexOf(destNode);
                    
                    if (sourceNodeIndex >= 0 && destNodeIndex >= 0)
                    {
                        // Store connection properties
                        connectionData.setProperty("sourceNode", sourceNodeIndex, nullptr);
                        connectionData.setProperty("sourcePort", sourcePoint->getPortIndex(), nullptr);
                        connectionData.setProperty("destNode", destNodeIndex, nullptr);
                        connectionData.setProperty("destPort", destPoint->getPortIndex(), nullptr);
                        
                        connectionsData.addChild(connectionData, -1, nullptr);
                    }
                }
            }
        }
    }
    
    // Add nodes and connections to project data
    projectData.addChild(nodesData, -1, nullptr);
    projectData.addChild(connectionsData, -1, nullptr);
}

void PluginEditorCanvas::loadFromProject(const juce::ValueTree& projectData)
{
    // Clear existing nodes and connections
    clear();
    
    // Load nodes
    juce::ValueTree nodesData = projectData.getChildWithName("Nodes");
    
    if (nodesData.isValid())
    {
        for (int i = 0; i < nodesData.getNumChildren(); ++i)
        {
            juce::ValueTree nodeData = nodesData.getChild(i);
            
            if (nodeData.hasProperty("type"))
            {
                // Create node based on type
                NodeType type = static_cast<NodeType>(static_cast<int>(nodeData.getProperty("type")));
                juce::String name = nodeData.getProperty("name", "Node");
                
                std::unique_ptr<ConcretePluginNode> newNode;
                
                switch (type)
                {
                    case NodeType::GuiControl:
                        newNode = std::make_unique<ConcretePluginNode>(name, Features::guiColor);
                        break;
                        
                    case NodeType::Equalizer:
                        newNode = std::make_unique<ConcretePluginNode>(name, Features::eqColor);
                        break;
                        
                    case NodeType::Compressor:
                        newNode = std::make_unique<ConcretePluginNode>(name, Features::compColor);
                        break;
                        
                    default:
                        newNode = std::make_unique<ConcretePluginNode>(name, juce::Colours::grey);
                        break;
                }
                
                if (newNode != nullptr)
                {
                    // Set position and size
                    int x = nodeData.getProperty("x", 0);
                    int y = nodeData.getProperty("y", 0);
                    int width = nodeData.getProperty("width", 150);
                    int height = nodeData.getProperty("height", 100);
                    
                    newNode->setBounds(x, y, width, height);
                    
                    // Add input and output ports
                    newNode->addInputPort("Input");
                    newNode->addOutputPort("Output");
                    
                    // Add to canvas
                    addAndMakeVisible(newNode.get());
                    nodes.add(newNode.release());
                }
            }
        }
    }
    
    // Load connections
    juce::ValueTree connectionsData = projectData.getChildWithName("Connections");
    
    if (connectionsData.isValid())
    {
        for (int i = 0; i < connectionsData.getNumChildren(); ++i)
        {
            juce::ValueTree connectionData = connectionsData.getChild(i);
            
            if (connectionData.hasProperty("sourceNode") && connectionData.hasProperty("destNode"))
            {
                int sourceNodeIndex = connectionData.getProperty("sourceNode");
                int sourcePortIndex = connectionData.getProperty("sourcePort");
                int destNodeIndex = connectionData.getProperty("destNode");
                int destPortIndex = connectionData.getProperty("destPort");
                
                if (sourceNodeIndex >= 0 && sourceNodeIndex < nodes.size() &&
                    destNodeIndex >= 0 && destNodeIndex < nodes.size())
                {
                    auto* sourceNode = nodes[sourceNodeIndex];
                    auto* destNode = nodes[destNodeIndex];
                    
                    if (sourceNode != nullptr && destNode != nullptr)
                    {
                        // Find connection points
                        AudioConnectionPoint* sourcePoint = sourceNode->getOutputPort(sourcePortIndex);
                        AudioConnectionPoint* destPoint = destNode->getInputPort(destPortIndex);
                        
                        // If you have the connection points, create the connection
                        if (sourcePoint != nullptr && destPoint != nullptr)
                        {
                            addConnection(sourcePoint, destPoint);
                        }
                    }
                }
            }
        }
    }
    
    // Hide tutorial overlay if we loaded nodes
    if (nodes.size() > 0)
    {
        showTutorialOverlay = false;
    }
    
    repaint();
}

// Implementation of drag and drop methods
/*
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
    std::unique_ptr<juce::Component> newNode;
    
    if (type == ComponentType::Knob ||
        type == ComponentType::Slider ||
        type == ComponentType::Button ||
        type == ComponentType::Label)
    {
        // GUI components - create GuiNode without passing a ComponentType parameter
        newNode = std::make_unique<GuiNode>();
        
        // If needed, set the control type after creation
        if (auto* guiNode = dynamic_cast<GuiNode*>(newNode.get()))
        {
            int controlTypeValue = static_cast<int>(type);
            guiNode->setControlType(controlTypeValue);
        }
    }
    else if (type == ComponentType::EQLowpass ||
             type == ComponentType::EQHighpass ||
             type == ComponentType::EQBandpass ||
             type == ComponentType::EQNotch ||
             type == ComponentType::EQPeaking)
    {
        // EQ components
        newNode = std::make_unique<EqualizerNode>();
    }
    else if (type == ComponentType::CompThreshold ||
             type == ComponentType::CompRatio ||
             type == ComponentType::CompAttack ||
             type == ComponentType::CompRelease)
    {
        // Compressor components
        newNode = std::make_unique<CompressorNode>();
    }
    
    if (newNode != nullptr)
    {
        // Set size
        newNode->setSize(200, 150);
        
        // Position the node at the drop location
        newNode->setBounds(details.localPosition.x - newNode->getWidth() / 2,
                          details.localPosition.y - newNode->getHeight() / 2,
                          newNode->getWidth(),
                          newNode->getHeight());
        
        // Hide tutorial overlay when first node is added
        showTutorialOverlay = false;
        
        // Add the node to the canvas using the addNode method
        if (auto* concreteNode = dynamic_cast<ConcretePluginNode*>(newNode.get()))
        {
            // If it's a ConcretePluginNode
            addNode(std::unique_ptr<ConcretePluginNode>(concreteNode));
            newNode.release(); // Release ownership as it's now managed by addNode
        }
        else
        {
            // Add the node to the canvas
            addAndMakeVisible(newNode.get());
            juce::Component* nodePtr = newNode.get();
            
            // Add to appropriate array based on type
            if (auto* guiNode = dynamic_cast<GuiNode*>(newNode.get()))
            {
                guiNodes.add(guiNode);
                newNode.release(); // Release ownership as it's now owned by OwnedArray
            }
            else if (auto* eqNode = dynamic_cast<EqualizerNode*>(newNode.get()))
            {
                eqNodes.add(eqNode);
                newNode.release(); // Release ownership as it's now owned by OwnedArray
            }
            else if (auto* compNode = dynamic_cast<CompressorNode*>(newNode.get()))
            {
                compressorNodes.add(compNode);
                newNode.release(); // Release ownership as it's now owned by OwnedArray
            }
            
            // Notify listeners
            if (onNodeAdded && nodePtr != nullptr)
                onNodeAdded(static_cast<PluginNodeComponent*>(nodePtr));
        }
    }
}
*/

void PluginEditorCanvas::startConnectionDrag(AudioConnectionPoint* startPoint)
{
    if (startPoint != nullptr)
    {
        selectedSource = startPoint;
        isDraggingConnection = true;
        dragEndPoint = startPoint->getBounds().getCentre().toFloat();
        repaint();
    }
}

void PluginEditorCanvas::dragConnection(AudioConnectionPoint* source, const juce::Point<float>& mousePos)
{
    if (isDraggingConnection && source == selectedSource)
    {
        dragEndPoint = getLocalPoint(nullptr, mousePos);
        repaint();
    }
}

void PluginEditorCanvas::endConnectionDrag()
{
    if (isDraggingConnection && selectedSource != nullptr)
    {
        // Convert drag end point to screen coordinates
        juce::Point<int> screenPoint = localPointToGlobal(dragEndPoint.toInt());
        
        // Find a connection point at this position
        AudioConnectionPoint* target = findPortAt(screenPoint, selectedSource);
        
        // If we found a valid target and can connect them
        if (target != nullptr && selectedSource->canConnect(target))
        {
            // Determine source and destination based on port types
            AudioConnectionPoint* source = selectedSource;
            AudioConnectionPoint* destination = target;
            
            // If our selected source is an input, swap them
            // Use getDirection instead of getType
            if (selectedSource->getDirection() == AudioConnectionPoint::PortDirection::Input)
            {
                source = target;
                destination = selectedSource;
            }
            
            // Create the connection
            addConnection(source, destination);
        }
        
        // Reset connection state
        isDraggingConnection = false;
        selectedSource = nullptr;
        repaint();
    }
}

void PluginEditorCanvas::updateConnectionsForPort(AudioConnectionPoint* port)
{
    if (port == nullptr)
        return;
        
    // Find all connections that use this port
    for (auto* connection : connections)
    {
        if (connection->getSourcePoint() == port || connection->getDestinationPoint() == port)
        {
            connection->updatePosition();
        }
    }
}

AudioConnectionPoint* PluginEditorCanvas::findPortAt(const juce::Point<int>& screenPosition, const AudioConnectionPoint* excludePort)
{
    // Convert screen position to local coordinates for each node
    for (auto* node : nodes)
    {
        if (node == nullptr)
            continue;
            
        // Check input ports
        for (int i = 0; i < 10; i++) // Assuming a reasonable maximum number of ports
        {
            auto* port = node->getInputPort(i);
            if (port != nullptr && port != excludePort)
            {
                // Convert port bounds to screen coordinates
                juce::Rectangle<int> portBounds = port->getScreenBounds();
                
                // Check if the position is within the port bounds
                if (portBounds.contains(screenPosition))
                {
                    return port;
                }
            }
        }
        
        // Check output ports
        for (int i = 0; i < 10; i++) // Assuming a reasonable maximum number of ports
        {
            auto* port = node->getOutputPort(i);
            if (port != nullptr && port != excludePort)
            {
                // Convert port bounds to screen coordinates
                juce::Rectangle<int> portBounds = port->getScreenBounds();
                
                // Check if the position is within the port bounds
                if (portBounds.contains(screenPosition))
                {
                    return port;
                }
            }
        }
    }
    
    return nullptr;
}

void PluginEditorCanvas::disconnectPort(AudioConnectionPoint* port)
{
    if (port == nullptr)
        return;
        
    // Find all connections that use this port
    for (int i = connections.size() - 1; i >= 0; --i)
    {
        auto* connection = connections[i];
        
        if (connection->getSourcePoint() == port || connection->getDestinationPoint() == port)
        {
            // Notify listeners before removing
            if (onConnectionRemoved)
                onConnectionRemoved(connection);
                
            // Remove the connection
            connections.remove(i);
        }
    }
    
    // Update the port state
    port->setConnected(false);
    
    repaint();
}

void PluginEditorCanvas::addNode(std::unique_ptr<ConcretePluginNode> node)
{
    if (node != nullptr)
    {
        // Add the node to the canvas
        addAndMakeVisible(node.get());
        PluginNodeComponent* nodePtr = node.get();
        nodes.add(node.release());
        
        // Notify listeners
        if (onNodeAdded)
            onNodeAdded(nodePtr);
    }
}

void PluginEditorCanvas::removeNode(ConcretePluginNode* node)
{
    if (node != nullptr)
    {
        // Notify listeners
        if (onNodeRemoved)
            onNodeRemoved(node);
        
        // Remove the node from the canvas
        nodes.removeObject(node);
    }
}

