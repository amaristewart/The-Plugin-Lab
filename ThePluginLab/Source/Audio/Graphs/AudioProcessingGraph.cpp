#include "AudioProcessingGraph.h"

// Add this proxy processor class that delegates to the real processor
class ProcessorProxy : public juce::AudioProcessor
{
public:
    ProcessorProxy(juce::AudioProcessor* sourceProcessor) : source(sourceProcessor)
    {
        jassert(source != nullptr);
    }
    
    // Forward essential methods to the source processor
    void prepareToPlay(double sampleRate, int maxBlockSize) override
    {
        if (source) source->prepareToPlay(sampleRate, maxBlockSize);
    }
    
    void releaseResources() override
    {
        if (source) source->releaseResources();
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        if (source) source->processBlock(buffer, midiMessages);
    }
    
    // Required overrides
    const juce::String getName() const override { return source ? source->getName() : "Proxy"; }
    bool acceptsMidi() const override { return source ? source->acceptsMidi() : false; }
    bool producesMidi() const override { return source ? source->producesMidi() : false; }
    double getTailLengthSeconds() const override { return source ? source->getTailLengthSeconds() : 0.0; }
    bool hasEditor() const override { return false; }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    int getNumPrograms() override { return source ? source->getNumPrograms() : 0; }
    int getCurrentProgram() override { return source ? source->getCurrentProgram() : 0; }
    void setCurrentProgram(int index) override { if (source) source->setCurrentProgram(index); }
    const juce::String getProgramName(int index) override { return source ? source->getProgramName(index) : ""; }
    void changeProgramName(int index, const juce::String& name) override { if (source) source->changeProgramName(index, name); }
    void getStateInformation(juce::MemoryBlock& block) override { if (source) source->getStateInformation(block); }
    void setStateInformation(const void* data, int size) override { if (source) source->setStateInformation(data, size); }

    juce::AudioProcessor* getSource() const { return source; }
    
private:
    juce::AudioProcessor* source;
};

AudioProcessingGraph::AudioProcessingGraph()
{
    // Create the audio graph
    audioGraph = std::make_unique<juce::AudioProcessorGraph>();
}

AudioProcessingGraph::~AudioProcessingGraph()
{
    // Clean up resources
    clear();
}

bool AudioProcessingGraph::connectProcessors(juce::AudioProcessor* sourceProcessor, int sourceChannel,
                                      juce::AudioProcessor* destProcessor, int destChannel)
{
    if (sourceProcessor == nullptr || destProcessor == nullptr || audioGraph == nullptr)
        return false;
        
    // Find the corresponding graph nodes
    auto sourceNode = findNode(sourceProcessor);
    auto destNode = findNode(destProcessor);
    
    if (sourceNode == nullptr || destNode == nullptr)
        return false;
        
    // Create connection by explicitly constructing NodeAndChannel objects
    juce::AudioProcessorGraph::NodeAndChannel source;
    source.nodeID = sourceNode->nodeID;
    source.channelIndex = static_cast<juce::uint32>(sourceChannel);
    
    juce::AudioProcessorGraph::NodeAndChannel dest;
    dest.nodeID = destNode->nodeID;
    dest.channelIndex = static_cast<juce::uint32>(destChannel);
    
    // Now create the connection
    return audioGraph->addConnection({ source, dest });
}

bool AudioProcessingGraph::disconnectProcessors(juce::AudioProcessor* sourceProcessor, int sourceChannel,
                                         juce::AudioProcessor* destProcessor, int destChannel)
{
    if (sourceProcessor == nullptr || destProcessor == nullptr || audioGraph == nullptr)
        return false;
        
    // Find the corresponding graph nodes
    auto sourceNode = findNode(sourceProcessor);
    auto destNode = findNode(destProcessor);
    
    if (sourceNode == nullptr || destNode == nullptr)
        return false;
        
    // Create connection by explicitly constructing NodeAndChannel objects
    juce::AudioProcessorGraph::NodeAndChannel source;
    source.nodeID = sourceNode->nodeID;
    source.channelIndex = static_cast<juce::uint32>(sourceChannel);
    
    juce::AudioProcessorGraph::NodeAndChannel dest;
    dest.nodeID = destNode->nodeID;
    dest.channelIndex = static_cast<juce::uint32>(destChannel);
    
    // Remove the connection
    return audioGraph->removeConnection({ source, dest });
}

// Fix the createNodeWithoutOwnership method:
juce::AudioProcessorGraph::Node::Ptr AudioProcessingGraph::createNodeWithoutOwnership(juce::AudioProcessor& processor, juce::AudioProcessorGraph::NodeID nodeID)
{
    // Create a proxy that delegates to the real processor without taking ownership
    auto proxy = std::make_unique<ProcessorProxy>(&processor);
    
    // Store the relationship between proxy and real processor
    auto node = audioGraph->addNode(std::move(proxy), nodeID);
    
    return node;
}

// Fix the addProcessor method:
juce::AudioProcessorGraph::Node::Ptr AudioProcessingGraph::addProcessor(juce::AudioProcessor* processor)
{
    if (processor == nullptr || audioGraph == nullptr)
        return nullptr;
        
    auto nodeID = juce::AudioProcessorGraph::NodeID(processors.indexOf(processor) + 1);
    
    // Create a node without transferring ownership
    return createNodeWithoutOwnership(*processor, nodeID);
}

void AudioProcessingGraph::addNode(juce::AudioProcessor* processor)
{
    if (processor == nullptr || audioGraph == nullptr)
        return;
    
    // Add to our tracking array without taking ownership
    processors.add(processor);
    
    // Use our helper method to add the processor
    auto node = addProcessor(processor);
    
    // If node was added successfully, we'll have a valid node pointer
    jassert(node != nullptr);
}

void AudioProcessingGraph::removeNode(juce::AudioProcessor* processor)
{
    if (processor == nullptr || audioGraph == nullptr)
        return;
        
    auto node = findNode(processor);
    if (node != nullptr)
    {
        audioGraph->removeNode(node->nodeID);
        processors.removeFirstMatchingValue(processor);
    }
}

void AudioProcessingGraph::addPluginNode(PluginNodeComponent* node)
{
    if (node == nullptr || node->getProcessor() == nullptr || audioGraph == nullptr)
        return;
        
    // Add the processor to the graph
    auto* processor = node->getProcessor();
    processors.add(processor);
    
    // Use our helper method to add the processor
    auto graphNode = addProcessor(processor);
    
    if (graphNode != nullptr)
    {
        // Store the mapping
        nodeProcessorMap.set(node, processor);
        
        // Notify listeners
        if (onProcessingChainChanged)
            onProcessingChainChanged();
    }
}

void AudioProcessingGraph::removePluginNode(PluginNodeComponent* node)
{
    if (node == nullptr)
        return;
        
    // Get associated processor
    auto* processor = nodeProcessorMap[node];
    if (processor != nullptr)
    {
        // Remove from graph
        removeNode(processor);
        
        // Remove from our internal tracking
        processors.removeFirstMatchingValue(processor);
        nodeProcessorMap.remove(node);
        
        // Notify listeners
        if (onProcessingChainChanged)
            onProcessingChainChanged();
    }
}

void AudioProcessingGraph::clear()
{
    if (audioGraph != nullptr)
    {
        audioGraph->clear();
        processors.clear();
        nodeProcessorMap.clear();
    }
}

juce::AudioProcessorGraph::Node* AudioProcessingGraph::findNode(juce::AudioProcessor* processor)
{
    if (processor == nullptr || audioGraph == nullptr)
        return nullptr;
        
    for (auto* node : audioGraph->getNodes())
    {
        // Check if the processor itself matches
        if (node->getProcessor() == processor)
            return node;
            
        // Check if this is a proxy pointing to our processor
        auto* proxy = dynamic_cast<ProcessorProxy*>(node->getProcessor());
        if (proxy && proxy->getSource() == processor)
            return node;
    }
    
    return nullptr;
}

bool AudioProcessingGraph::connectNodes(int sourceNodeId, int sourceChannelIndex, 
                                      int destNodeId, int destChannelIndex)
{
    if (audioGraph == nullptr)
        return false;
        
    juce::AudioProcessorGraph::NodeAndChannel source;
    source.nodeID = juce::AudioProcessorGraph::NodeID(sourceNodeId);
    source.channelIndex = static_cast<juce::uint32>(sourceChannelIndex);
    
    juce::AudioProcessorGraph::NodeAndChannel dest;
    dest.nodeID = juce::AudioProcessorGraph::NodeID(destNodeId);
    dest.channelIndex = static_cast<juce::uint32>(destChannelIndex);
    
    return audioGraph->addConnection({ source, dest });
}

void AudioProcessingGraph::disconnectNodes(int sourceNodeId, int destNodeId)
{
    if (audioGraph == nullptr)
        return;
        
    audioGraph->disconnectNode(juce::AudioProcessorGraph::NodeID(sourceNodeId));
}

void AudioProcessingGraph::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    if (audioGraph != nullptr)
    {
        audioGraph->setPlayConfigDetails(2, 2, sampleRate, samplesPerBlock);
        audioGraph->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void AudioProcessingGraph::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (audioGraph != nullptr)
    {
        audioGraph->processBlock(buffer, midiMessages);
    }
}

void AudioProcessingGraph::releaseResources()
{
    if (audioGraph != nullptr)
    {
        audioGraph->releaseResources();
    }
}
