#include "AudioProcessingGraph.h"
#include "../../Components/PluginNodeComponent.h"
#include "../../Nodes/EqualizerNode.h"
#include "../Processors/PluginAudioProcessor.h"
#include "../../Connections/AudioConnectionPoint.h"

AudioProcessingGraph::AudioProcessingGraph()
{
}

AudioProcessingGraph::~AudioProcessingGraph()
{
}


void AudioProcessingGraph::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    graph.setPlayConfigDetails(2, 2, sampleRate, samplesPerBlock);
    graph.prepareToPlay(sampleRate, samplesPerBlock);
    
    if (!inputNode)
    {
        inputNode = graph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
            juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
    }
    
    if (!outputNode)
    {
        outputNode = graph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
            juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));
    }
    
    for (auto* node : nodes)
    {
        if (auto* processor = node->getProcessor())
        {
            processor->prepareToPlay(sampleRate, samplesPerBlock);
        }
    }
}

void AudioProcessingGraph::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (graphNeedsUpdate.compareAndSetBool(false, true))
    {
        rebuildGraph();
    }
    
    graph.processBlock(buffer, midiMessages);
}

void AudioProcessingGraph::releaseResources()
{
    for (auto* node : nodes)
    {
        if (auto* processor = node->getProcessor())
        {
            processor->releaseResources();
        }
    }
}

bool AudioProcessingGraph::addNode(PluginNodeComponent* node)
{
    if (auto* processor = node->getProcessor())
    {
        auto graphNode = graph.addNode(std::unique_ptr<juce::AudioProcessor>(processor));
        if (graphNode)
        {
            nodes.add(node);
            nodeMap[node] = graphNode;
            graphNeedsUpdate = true;
            return true;
        }
    }
    return false;
}

bool AudioProcessingGraph::removeNode(PluginNodeComponent* node)
{
    int index = nodes.indexOf(node);
    if (index >= 0)
    {
        nodes.remove(index);
        return true;
    }
    return false;
}

bool AudioProcessingGraph::connectNodes(PluginNodeComponent* source, int sourceChannel,
                                     PluginNodeComponent* dest, int destChannel)
{
    if (auto sourceNode = nodeMap[source])
    {
        if (auto destNode = nodeMap[dest])
        {
            auto srcChan = static_cast<uint32_t>(sourceChannel);
            auto dstChan = static_cast<uint32_t>(destChannel);
            
            using NodeAndChannel = juce::AudioProcessorGraph::NodeAndChannel;
            NodeAndChannel sourceNAC{sourceNode->nodeID, static_cast<int>(srcChan)};
            NodeAndChannel destNAC{destNode->nodeID, static_cast<int>(dstChan)};
            
            if (graph.addConnection({sourceNAC, destNAC}))
            {
                connections.add(new NodeConnection(source, srcChan, dest, dstChan));
                graphNeedsUpdate = true;
                return true;
            }
        }
    }
    return false;
}

bool AudioProcessingGraph::disconnectNodes(PluginNodeComponent* source, int sourceChannel,
                                         PluginNodeComponent* dest, int destChannel)
{
    if (auto sourceNode = nodeMap[source])
    {
        if (auto destNode = nodeMap[dest])
        {
            auto srcChan = static_cast<uint32_t>(sourceChannel);
            auto dstChan = static_cast<uint32_t>(destChannel);
            
            using NodeAndChannel = juce::AudioProcessorGraph::NodeAndChannel;
            NodeAndChannel sourceNAC{sourceNode->nodeID, static_cast<int>(srcChan)};
            NodeAndChannel destNAC{destNode->nodeID, static_cast<int>(dstChan)};
            
            if (graph.removeConnection({sourceNAC, destNAC}))
            {
                for (int i = connections.size() - 1; i >= 0; --i)
                {
                    auto* connection = connections[i];
                    if (connection->getSourceNode() == source &&
                        connection->getSourceChannel() == sourceChannel &&
                        connection->getDestinationNode() == dest &&
                        connection->getDestinationChannel() == destChannel)
                    {
                        connections.remove(i);
                        break;
                    }
                }
                
                if (onProcessingChainChanged)
                    onProcessingChainChanged();
                
                return true;
            }
        }
    }
    return false;
}

std::vector<PluginNodeComponent*> AudioProcessingGraph::topologicalSort()
{
    std::vector<PluginNodeComponent*> sorted;
    std::set<PluginNodeComponent*> visited;
    
    std::function<void(PluginNodeComponent*)> visit = [&](PluginNodeComponent* node)
    {
        if (visited.find(node) != visited.end())
            return;
            
        visited.insert(node);
        
        for (auto* connection : connections)
        {
            if (connection->getSourceNode() == node)
            {
                visit(connection->getDestinationNode());
            }
        }
        
        sorted.push_back(node);
    };
    
    for (auto* node : nodes)
    {
        bool hasInputs = false;
        for (auto* connection : connections)
        {
            if (connection->getDestinationNode() == node)
            {
                hasInputs = true;
                break;
            }
        }
        if (!hasInputs)
            visit(node);
    }
    
    std::reverse(sorted.begin(), sorted.end());
    return sorted;
}

void AudioProcessingGraph::saveToXml(juce::File& file)
{
    auto state = createStateTree();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->writeTo(file);
}

void AudioProcessingGraph::loadFromXml(const juce::File& file)
{
    if (auto xml = juce::XmlDocument::parse(file))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        restoreFromStateTree(state);
    }
}

juce::ValueTree AudioProcessingGraph::createStateTree() const
{
    juce::ValueTree tree("PLUGIN_LAB_STATE");
    
    juce::ValueTree nodesTree("NODES");
    for (auto* node : nodes)
    {
        juce::ValueTree nodeTree("NODE");
        nodeTree.setProperty("type", static_cast<int>(node->getType()), nullptr);
        nodeTree.setProperty("x", node->getX(), nullptr);
        nodeTree.setProperty("y", node->getY(), nullptr);
        
        if (auto* processor = node->getProcessor())
        {
            juce::MemoryBlock state;
            processor->getStateInformation(state);
            nodeTree.setProperty("processorState", state.toBase64Encoding(), nullptr);
        }
        
        nodesTree.addChild(nodeTree, -1, nullptr);
    }
    tree.addChild(nodesTree, -1, nullptr);
    
    juce::ValueTree connectionsTree("CONNECTIONS");
    for (auto* connection : connections)
    {
        juce::ValueTree connectionTree("CONNECTION");
        connectionTree.setProperty("sourceNode", nodes.indexOf(connection->getSourceNode()), nullptr);
        connectionTree.setProperty("sourceChannel", static_cast<int>(connection->getSourceChannel()), nullptr);
        connectionTree.setProperty("destNode", nodes.indexOf(connection->getDestinationNode()), nullptr);
        connectionTree.setProperty("destChannel", static_cast<int>(connection->getDestinationChannel()), nullptr);
        connectionsTree.addChild(connectionTree, -1, nullptr);
    }
    tree.addChild(connectionsTree, -1, nullptr);
    
    return tree;
}

juce::Array<ConnectionComponent*> AudioProcessingGraph::getInputConnectionsForNode(PluginNodeComponent* node)
{
    juce::Array<ConnectionComponent*> inputs;
    for (auto* connection : connections)
    {
        if (connection->getDestinationNode() == node)
        {
            auto* connComp = new ConnectionComponent(
                new AudioConnectionPoint(*connection->getSourceNode(), ConnectionPointType::Output, connection->sourcePortIndex),
                new AudioConnectionPoint(*connection->getDestinationNode(), ConnectionPointType::Input, connection->destPortIndex)
            );
            inputs.add(connComp);
        }
    }
    return inputs;
}

juce::Array<ConnectionComponent*> AudioProcessingGraph::getOutputConnectionsForNode(PluginNodeComponent* node)
{
    juce::Array<ConnectionComponent*> outputs;
    for (auto* connection : connections)
    {
        if (connection->getSourceNode() == node)
        {
            auto* connComp = new ConnectionComponent(
                new AudioConnectionPoint(*connection->getSourceNode(), ConnectionPointType::Output, connection->sourcePortIndex),
                new AudioConnectionPoint(*connection->getDestinationNode(), ConnectionPointType::Input, connection->destPortIndex)
            );
            outputs.add(connComp);
        }
    }
    return outputs;
}

void AudioProcessingGraph::restoreFromStateTree(const juce::ValueTree& tree)
{
    nodes.clear();
    connections.clear();
    
    if (tree.hasType("PLUGIN_LAB_STATE"))
    {
        auto nodesTree = tree.getChildWithName("NODES");
        if (nodesTree.isValid())
        {
            for (int i = 0; i < nodesTree.getNumChildren(); ++i)
            {
                auto nodeTree = nodesTree.getChild(i);
                int typeInt = nodeTree.getProperty("type", -1);
                if (typeInt < 0) continue;
                
                NodeType type = static_cast<NodeType>(typeInt);
                
                std::unique_ptr<PluginNodeComponent> newNode;
                switch (type)
                {
                    case NodeType::Equalizer:
                        newNode = std::make_unique<EqualizerNode>();
                        break;
                    default:
                        continue;
                }
                
                int x = nodeTree.getProperty("x", 0);
                int y = nodeTree.getProperty("y", 0);
                newNode->setBounds(x, y, newNode->getWidth(), newNode->getHeight());
                
                if (auto* processor = newNode->getProcessor())
                {
                    // Restore processor specific state
                }
                
                addNode(newNode.release());
            }
        }
        
        // Restore connections
        auto connectionsTree = tree.getChildWithName("CONNECTIONS");
        if (connectionsTree.isValid())
        {
            for (int i = 0; i < connectionsTree.getNumChildren(); ++i)
            {
                auto connectionTree = connectionsTree.getChild(i);
                int sourceNodeIndex = connectionTree.getProperty("sourceNode", -1);
                int sourcePort = connectionTree.getProperty("sourcePort", -1);
                int destNodeIndex = connectionTree.getProperty("destNode", -1);
                int destPort = connectionTree.getProperty("destPort", -1);
                
                if (sourceNodeIndex >= 0 && sourceNodeIndex < nodes.size() &&
                    destNodeIndex >= 0 && destNodeIndex < nodes.size())
                {
                    connectNodes(nodes[sourceNodeIndex], sourcePort,
                               nodes[destNodeIndex], destPort);
                }
            }
        }
    }
}

void AudioProcessingGraph::rebuildGraph()
{
    graph.clear();
    
    for (auto* node : nodes)
    {
        if (auto* processor = node->getProcessor())
        {
            auto graphNode = graph.addNode(std::unique_ptr<juce::AudioProcessor>(processor));
            if (graphNode)
            {
                nodeMap[node] = graphNode;
            }
        }
    }
    
    for (auto* connection : connections)
    {
        if (auto sourceNode = nodeMap[connection->getSourceNode()])
        {
            if (auto destNode = nodeMap[connection->getDestinationNode()])
            {
                auto srcChan = connection->getSourceChannel();
                auto dstChan = connection->getDestinationChannel();
                
                using NodeAndChannel = juce::AudioProcessorGraph::NodeAndChannel;
                NodeAndChannel source{sourceNode->nodeID, static_cast<int>(srcChan)};
                NodeAndChannel dest{destNode->nodeID, static_cast<int>(dstChan)};
                
                graph.addConnection({source, dest});
            }
        }
    }
}

void AudioProcessingGraph::saveState(juce::MemoryBlock& destData) const
{
    auto state = createStateTree();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml != nullptr)
    {
        destData.reset();
        juce::MemoryOutputStream stream(destData, false);
        xml->writeToStream(stream, {});
    }
}

void AudioProcessingGraph::loadState(const void* data, int sizeInBytes)
{
    if (auto xml = juce::XmlDocument::parse(juce::String::createStringFromData(data, sizeInBytes)))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        restoreFromStateTree(state);
    }
}
