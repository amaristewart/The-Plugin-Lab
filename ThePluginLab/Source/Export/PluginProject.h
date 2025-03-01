#pragma once
#include <JuceHeader.h>

class PluginProject
{
public:
    struct NodeData
    {
        juce::String type;
        juce::Point<int> position;
        juce::ValueTree parameters;
    };
    
    struct ConnectionData
    {
        int sourceNodeIndex;
        int sourcePortIndex;
        int destNodeIndex;
        int destPortIndex;
    };
    
    PluginProject() = default;
    
    void clear()
    {
        projectName = "Untitled Plugin";
        nodes.clear();
        connections.clear();
        state = juce::ValueTree("PluginProject");
    }
    
    bool saveToFile(const juce::File& file)
    {
        // Create project state
        state = juce::ValueTree("PluginProject");
        state.setProperty("name", projectName, nullptr);
        state.setProperty("version", "1.0", nullptr);
        
        // Save nodes
        auto nodesState = state.getOrCreateChildWithName("Nodes", nullptr);
        for (const auto& node : nodes)
        {
            auto nodeState = juce::ValueTree("Node");
            nodeState.setProperty("type", node.type, nullptr);
            nodeState.setProperty("x", node.position.x, nullptr);
            nodeState.setProperty("y", node.position.y, nullptr);
            nodeState.addChild(node.parameters.createCopy(), -1, nullptr);
            nodesState.addChild(nodeState, -1, nullptr);
        }
        
        // Save connections
        auto connectionsState = state.getOrCreateChildWithName("Connections", nullptr);
        for (const auto& connection : connections)
        {
            auto connectionState = juce::ValueTree("Connection");
            connectionState.setProperty("sourceNode", connection.sourceNodeIndex, nullptr);
            connectionState.setProperty("sourcePort", connection.sourcePortIndex, nullptr);
            connectionState.setProperty("destNode", connection.destNodeIndex, nullptr);
            connectionState.setProperty("destPort", connection.destPortIndex, nullptr);
            connectionsState.addChild(connectionState, -1, nullptr);
        }
        
        // Write to file
        if (auto xml = state.createXml())
            return xml->writeTo(file);
        return false;
    }
    
    bool loadFromFile(const juce::File& file)
    {
        if (auto xml = juce::XmlDocument::parse(file))
        {
            state = juce::ValueTree::fromXml(*xml);
            
            if (state.hasType("PluginProject"))
            {
                clear();
                projectName = state.getProperty("name", "Untitled Plugin");
                
                // Load nodes
                auto nodesState = state.getChildWithName("Nodes");
                for (int i = 0; i < nodesState.getNumChildren(); ++i)
                {
                    auto nodeState = nodesState.getChild(i);
                    NodeData node;
                    node.type = nodeState.getProperty("type");
                    node.position.x = nodeState.getProperty("x");
                    node.position.y = nodeState.getProperty("y");
                    node.parameters = nodeState.getChild(0).createCopy();
                    nodes.add(node);
                }
                
                // Load connections
                auto connectionsState = state.getChildWithName("Connections");
                for (int i = 0; i < connectionsState.getNumChildren(); ++i)
                {
                    auto connectionState = connectionsState.getChild(i);
                    ConnectionData connection;
                    connection.sourceNodeIndex = connectionState.getProperty("sourceNode");
                    connection.sourcePortIndex = connectionState.getProperty("sourcePort");
                    connection.destNodeIndex = connectionState.getProperty("destNode");
                    connection.destPortIndex = connectionState.getProperty("destPort");
                    connections.add(connection);
                }
                
                return true;
            }
        }
        return false;
    }
    
    bool loadTemplate(const juce::String& templateName)
    {
        // Load built-in templates from BinaryData
        if (templateName == "Basic EQ")
            return loadFromData(BinaryData::BasicEQ_xml, BinaryData::BasicEQ_xmlSize);
        else if (templateName == "Basic Compressor")
            return loadFromData(BinaryData::BasicCompressor_xml, BinaryData::BasicCompressor_xmlSize);
        // Temporarily remove Multi-band EQ until we add the resource
        // else if (templateName == "Multi-band EQ")
        //     return loadFromData(BinaryData::MultibandEQ_xml, BinaryData::MultibandEQ_xmlSize);
            
        return false;
    }
    
    const juce::String& getName() const { return projectName; }
    void setName(const juce::String& name) { projectName = name; }
    
    juce::Array<NodeData>& getNodes() { return nodes; }
    juce::Array<ConnectionData>& getConnections() { return connections; }
    
    void setProcessorState(const juce::MemoryBlock& state)
    {
        processorState = state;
    }
    
    const juce::MemoryBlock& getProcessorState() const
    {
        return processorState;
    }
    
private:
    bool loadFromData(const void* data, size_t dataSize)
    {
        if (auto xml = juce::XmlDocument::parse(juce::String::createStringFromData(data, (int)dataSize)))
        {
            state = juce::ValueTree::fromXml(*xml);
            return state.hasType("PluginProject");
        }
        return false;
    }
    
    juce::String projectName = "Untitled Plugin";
    juce::Array<NodeData> nodes;
    juce::Array<ConnectionData> connections;
    juce::ValueTree state{"PluginProject"};
    juce::MemoryBlock processorState;
};
