#include "PluginEditorCanvas.h"
#include "../Components/PluginNodeComponent.h"
#include "../Nodes/EqualizerNode.h"
#include "../Nodes/CompressorNode.h"
#include "../Nodes/GuiNode.h"
#include "../Connections/AudioConnectionPoint.h"
#include "../Connections/AudioConnection.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/DraggableComponent.h"
#include <typeinfo>

PluginEditorCanvas::PluginEditorCanvas()
{
    setOpaque(true);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);
    
    setAlwaysOnTop(false);
}

PluginEditorCanvas::~PluginEditorCanvas()
{
    connections.clear();
    nodes.clear();
}

void PluginEditorCanvas::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF2D2D2D));
    
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    
    for (int x = 0; x < getWidth(); x += 20)
        g.drawVerticalLine(x, 0.0f, (float)getHeight());
        
    for (int y = 0; y < getHeight(); y += 20)
        g.drawHorizontalLine(y, 0.0f, (float)getWidth());

    if (isDraggingConnection)
    {
        g.setColour(juce::Colours::white);
        g.drawLine(dragLineStart.x, dragLineStart.y,
                  dragLineEnd.x, dragLineEnd.y,
                  2.0f);
    }
}

void PluginEditorCanvas::resized()
{
    for (auto* node : nodes)
    {
        auto bounds = node->getBounds();
        bounds.setX(juce::jlimit(0, getWidth() - bounds.getWidth(), bounds.getX()));
        bounds.setY(juce::jlimit(0, getHeight() - bounds.getHeight(), bounds.getY()));
        node->setBounds(bounds);
    }
    
    updateAllConnections();
}

void PluginEditorCanvas::addNode(std::unique_ptr<PluginNodeComponent> node)
{
    if (node != nullptr)
    {
        if (node->getBounds().isEmpty())
        {
            node->setBounds((getWidth() - node->getWidth()) / 2,
                           (getHeight() - node->getHeight()) / 2,
                           node->getWidth(),
                           node->getHeight());
        }
        
        auto* nodePtr = node.get();
        addAndMakeVisible(nodePtr);
        nodes.add(node.release());
        
        if (onNodeAdded)
            onNodeAdded(nodePtr);
    }
}

void PluginEditorCanvas::removeNode(PluginNodeComponent* node)
{
    for (int i = connections.size() - 1; i >= 0; --i)
    {
        auto* connection = connections[i];
        if (&connection->getSourcePoint()->getOwner() == node ||
            &connection->getDestinationPoint()->getOwner() == node)
        {
            if (onConnectionRemoved)
                onConnectionRemoved(connection);
            connections.remove(i);
        }
    }
    
    if (onNodeRemoved)
        onNodeRemoved(node);
    
    nodes.removeObject(node);
}

void PluginEditorCanvas::addConnection(AudioConnectionPoint* source, AudioConnectionPoint* destination)
{
    if (source == nullptr || destination == nullptr)
        return;
        
    for (auto* connection : connections)
    {
        if (connection->getSourcePoint() == source && 
            connection->getDestinationPoint() == destination)
        {
            return;
        }
    }
    
    auto newConnection = std::make_unique<ConnectionComponent>(source, destination);
    addAndMakeVisible(newConnection.get());
    
    if (onConnectionMade)
        onConnectionMade(source, destination);
    
    connections.add(newConnection.release());
    
    updateAllConnections();
    
    auto& sourceNode = source->getOwner();
    auto& destNode = destination->getOwner();
    sourceNode.repaint();
    destNode.repaint();
}

void PluginEditorCanvas::removeConnection(ConnectionComponent* connection)
{
    connections.removeObject(connection);
}

void PluginEditorCanvas::itemDropped(const SourceDetails& details)
{
    if (auto* obj = details.description.getDynamicObject())
    {
        if (obj->hasProperty("type"))
        {
            auto typeInt = static_cast<int>(obj->getProperty("type"));
            auto type = static_cast<ComponentType>(typeInt);
            
            std::unique_ptr<PluginNodeComponent> newNode;
            
            switch (type)
            {
                // GUI Components
                case ComponentType::Knob:
                case ComponentType::Slider:
                case ComponentType::Button:
                case ComponentType::Label:
                    newNode = std::make_unique<GuiNode>(type);
                    break;
                    
                // EQ Components
                case ComponentType::EQLowpass:
                case ComponentType::EQHighpass:
                case ComponentType::EQBandpass:
                case ComponentType::EQNotch:
                case ComponentType::EQPeaking:
                    newNode = std::make_unique<EqualizerNode>();
                    break;
                    
                // Compressor Components
                case ComponentType::DynamicType:
                case ComponentType::CompThreshold:
                case ComponentType::CompRatio:
                case ComponentType::CompAttack:
                case ComponentType::CompRelease:
                    newNode = std::make_unique<CompressorNode>();
                    break;
                    
                default:
                    break;
            }
            
            if (newNode != nullptr)
            {
                auto dropPosition = details.localPosition;
                newNode->setBounds(dropPosition.x - newNode->getWidth() / 2,
                                 dropPosition.y - newNode->getHeight() / 2,
                                 newNode->getWidth(),
                                 newNode->getHeight());
                
                addNode(std::move(newNode));
            }
        }
    }
}

void PluginEditorCanvas::updateAllConnections()
{
    for (auto* connection : connections)
    {
        connection->updatePosition();
    }
    repaint();
}

void PluginEditorCanvas::updateDragLine(const juce::Point<float>& newPosition)
{
    isDraggingConnection = true;
    dragLineEnd = newPosition;
    repaint();
}

void PluginEditorCanvas::clearDragLine()
{
    isDraggingConnection = false;
    repaint();
}

void PluginEditorCanvas::clear()
{
    connections.clear();
    nodes.clear();
    repaint();
}

void PluginEditorCanvas::saveToProject(PluginProject& project)
{
    juce::MemoryBlock state;
    
    for (auto* node : nodes)
    {
        if (auto* processor = node->getProcessor())
        {
            processor->getStateInformation(state);
        }
    }
    
    project.setProcessorState(state);
}

void PluginEditorCanvas::loadFromProject(const PluginProject& project)
{
    auto state = project.getProcessorState();
    
    for (auto* node : nodes)
    {
        if (auto* processor = node->getProcessor())
        {
            processor->setStateInformation(state.getData(), state.getSize());
        }
    }
}

std::unique_ptr<PluginNodeComponent> PluginEditorCanvas::createNodeFromType(const juce::String& type)
{
    if (type == "EQLowShelf" || type == "EQPeak" || type == "EQHighShelf")
        return std::make_unique<EqualizerNode>();
    return nullptr;
}

juce::String PluginEditorCanvas::getNodeTypeName(NodeType type)
{
    switch (type)
    {
        case NodeType::Equalizer: return "Equalizer";
        case NodeType::Compressor: return "Compressor";
        default: return "Unknown";
    }
}
