#pragma once
#include <JuceHeader.h>
#include "../../Common/Forward.h"
#include "../../Components/ConnectionComponent.h"
#include <vector>
#include <map>
#include <functional>

class AudioProcessingGraph
{
public:
    AudioProcessingGraph();
    ~AudioProcessingGraph();
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void releaseResources();
    
    bool addNode(PluginNodeComponent* node);
    bool removeNode(PluginNodeComponent* node);
    bool connectNodes(PluginNodeComponent* source, int sourceChannel,
                     PluginNodeComponent* dest, int destChannel);
    bool disconnectNodes(PluginNodeComponent* source, int sourceChannel,
                        PluginNodeComponent* dest, int destChannel);
    
    void saveState(juce::MemoryBlock& destData) const;
    void loadState(const void* data, int sizeInBytes);
    
    juce::Array<PluginNodeComponent*> getConnectedNodes() const 
    { 
        juce::Array<PluginNodeComponent*> result;
        for (auto* node : nodes)
            result.add(node);
        return result;
    }
    
    void saveToXml(juce::File& file);
    void loadFromXml(const juce::File& file);
    
    juce::ValueTree createStateTree() const;
    void restoreFromStateTree(const juce::ValueTree& tree);
    
    std::function<void()> onProcessingChainChanged;
    
    juce::Array<ConnectionComponent*> getInputConnectionsForNode(PluginNodeComponent* node);
    juce::Array<ConnectionComponent*> getOutputConnectionsForNode(PluginNodeComponent* node);

    int getNumNodes() const { return nodes.size(); }
    bool hasActiveNodes() const { return !nodes.isEmpty(); }
    
private:
    struct NodeConnection
    {
        PluginNodeComponent* source;
        uint32_t sourcePortIndex;
        PluginNodeComponent* destination;
        uint32_t destPortIndex;

        NodeConnection() = default;
        NodeConnection(PluginNodeComponent* src, uint32_t srcPort, PluginNodeComponent* dest, uint32_t destPort)
            : source(src), sourcePortIndex(srcPort),
              destination(dest), destPortIndex(destPort) {}

        PluginNodeComponent* getSourceNode() const { return source; }
        uint32_t getSourceChannel() const { return sourcePortIndex; }
        PluginNodeComponent* getDestinationNode() const { return destination; }
        uint32_t getDestinationChannel() const { return destPortIndex; }
    };

    juce::OwnedArray<PluginNodeComponent> nodes;
    juce::OwnedArray<NodeConnection> connections;

    std::vector<PluginNodeComponent*> topologicalSort();
    
    void rebuildGraph();
    
    juce::AudioProcessorGraph graph;
    juce::AudioProcessorGraph::Node::Ptr inputNode;
    juce::AudioProcessorGraph::Node::Ptr outputNode;
    
    std::map<PluginNodeComponent*, juce::AudioProcessorGraph::Node::Ptr> nodeMap;
    juce::Atomic<bool> graphNeedsUpdate{false};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessingGraph)
};
