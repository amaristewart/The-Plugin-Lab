#pragma once
#include <JuceHeader.h>
#include "../../Common/Forward.h"
#include "../../Components/PluginNodeComponent.h"
#include "../../Nodes/EqualizerNode.h"
#include "../Processors/PluginAudioProcessor.h"
#include "../../Connections/AudioConnectionPoint.h"
#include <vector>
#include <map>
#include <memory>
#include <functional>

// Forward declarations
class PluginNodeComponent;

/**
 * Class for handling audio graph processing
 */
class AudioProcessingGraph
{
public:
    AudioProcessingGraph();
    ~AudioProcessingGraph();
    
    // Audio processing setup
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    
    // Graph management - basic methods
    void addNode(juce::AudioProcessor* processor);
    void removeNode(juce::AudioProcessor* processor);
    bool connectNodes(int sourceNodeId, int sourceChannelIndex, 
                     int destNodeId, int destChannelIndex);
    void disconnectNodes(int sourceNodeId, int destNodeId);
    void clear();
    
    // Advanced node management for PluginNodeComponents
    void addPluginNode(PluginNodeComponent* node);
    void removePluginNode(PluginNodeComponent* node);
    
    // Connection management for processors
    bool connectProcessors(juce::AudioProcessor* sourceProcessor, int sourceChannel,
                          juce::AudioProcessor* destProcessor, int destChannel);
    bool disconnectProcessors(juce::AudioProcessor* sourceProcessor, int sourceChannel,
                             juce::AudioProcessor* destProcessor, int destChannel);
    
    // Check if graph has any active nodes
    bool hasActiveNodes() const { return audioGraph != nullptr && audioGraph->getNumNodes() > 0; }
    
    // Callback when processing chain changes
    std::function<void()> onProcessingChainChanged;
    
    // Add this method declaration in the public section:
    juce::AudioProcessorGraph::Node::Ptr createNodeWithoutOwnership(juce::AudioProcessor& processor, juce::AudioProcessorGraph::NodeID nodeID);

private:
    // Helper methods
    juce::AudioProcessorGraph::Node* findNode(juce::AudioProcessor* processor);
    
    // Method for adding processors that handles unique_ptr requirements
    juce::AudioProcessorGraph::Node::Ptr addProcessor(juce::AudioProcessor* processor);
    
    // The JUCE audio processing graph
    std::unique_ptr<juce::AudioProcessorGraph> audioGraph;
    
    // Default sample rate and block size
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    
    // Track processors and nodes
    juce::Array<juce::AudioProcessor*> processors;
    juce::HashMap<PluginNodeComponent*, juce::AudioProcessor*> nodeProcessorMap;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessingGraph)
};
