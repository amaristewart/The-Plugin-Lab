#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"
#include "../Common/Features.h"

// Forward declarations to avoid circular dependencies
class AudioConnectionPoint;
class ConnectionComponent;
class PluginNodeComponent;
class ConcretePluginNode;
class GuiNode;
class EqualizerNode;
class CompressorNode;
class AudioProcessingGraph;

/**
 * Main canvas for editing plugin components
 */
class PluginEditorCanvas : public juce::Component,
                         public juce::DragAndDropTarget
{
public:
    PluginEditorCanvas();
    ~PluginEditorCanvas() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Node management
    void addNode(std::unique_ptr<ConcretePluginNode> node);
    void removeNode(ConcretePluginNode* node);
    
    // Connection management
    void addConnection(AudioConnectionPoint* source, AudioConnectionPoint* destination);
    void removeConnection(ConnectionComponent* connection);
    
    // Connection dragging handlers
    void startConnectionDrag(AudioConnectionPoint* startPoint);
    void dragConnection(AudioConnectionPoint* source, const juce::Point<float>& mousePos);
    void endConnectionDrag();
    void updateConnectionsForPort(AudioConnectionPoint* port);
    AudioConnectionPoint* findPortAt(const juce::Point<int>& screenPosition, const AudioConnectionPoint* excludePort = nullptr);
    void disconnectPort(AudioConnectionPoint* port);
    
    // Project management
    void saveToProject(juce::ValueTree& projectData);
    void loadFromProject(const juce::ValueTree& projectData);
    void clear();
    
    // DragAndDropTarget interface
    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& details) override;
    void itemDragEnter(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDragMove(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDragExit(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDropped(const juce::DragAndDropTarget::SourceDetails& details) override;
    
    // Callbacks for node and connection events
    std::function<void(PluginNodeComponent*)> onNodeAdded;
    std::function<void(PluginNodeComponent*)> onNodeRemoved;
    std::function<void(AudioConnectionPoint*, AudioConnectionPoint*)> onConnectionMade;
    std::function<void(ConnectionComponent*)> onConnectionRemoved;
    
    // Access to the audio processing graph
    AudioProcessingGraph* getProcessingGraph() { return processingGraph.get(); }
    
private:
    // Helper method
    void drawGrid(juce::Graphics& g, int gridSize);
    
    // Nodes
    juce::OwnedArray<ConcretePluginNode> nodes;
    juce::OwnedArray<GuiNode> guiNodes;
    juce::OwnedArray<EqualizerNode> eqNodes;
    juce::OwnedArray<CompressorNode> compressorNodes;
    
    // Connections
    juce::OwnedArray<ConnectionComponent> connections;
    
    // Connection drag state
    bool isDraggingConnection = false;
    AudioConnectionPoint* selectedSource = nullptr;
    juce::Point<float> dragEndPoint;
    
    // Audio processing graph
    std::unique_ptr<AudioProcessingGraph> processingGraph;
    
    // UI state
    bool showTutorialOverlay = true;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorCanvas)
};
