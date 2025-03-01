#pragma once
#include <JuceHeader.h>
#include "../Common/Forward.h"
#include "../Nodes/EqualizerNode.h"
#include "../Nodes/CompressorNode.h"
#include "../Export/PluginProject.h"
#include <memory>
#include <functional>

class PluginEditorCanvas : public juce::Component,
                          public juce::DragAndDropContainer,
                          public juce::DragAndDropTarget
{
public:
    PluginEditorCanvas();
    ~PluginEditorCanvas() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    // Node management
    void addNode(std::unique_ptr<PluginNodeComponent> node);
    void removeNode(PluginNodeComponent* node);
    
    // Connection management
    void addConnection(AudioConnectionPoint* source, AudioConnectionPoint* destination);
    void removeConnection(ConnectionComponent* connection);
    
    // Project management
    void clear();
    void saveToProject(PluginProject& project);
    void loadFromProject(const PluginProject& project);
    
    // DragAndDropTarget implementation
    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& details) override
    {
        if (auto* obj = details.description.getDynamicObject())
        {
            return obj->hasProperty("type");
        }
        return false;
    }
    void itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;

    // Callback types
    std::function<void(PluginNodeComponent*)> onNodeAdded;
    std::function<void(PluginNodeComponent*)> onNodeRemoved;
    std::function<void(AudioConnectionPoint*, AudioConnectionPoint*)> onConnectionMade;
    std::function<void(ConnectionComponent*)> onConnectionRemoved;

    void updateAllConnections();

    // Drag line visualization
    void updateDragLine(const juce::Point<float>& newPosition);
    void clearDragLine();

private:
    // Helper functions
    std::unique_ptr<PluginNodeComponent> createNodeFromType(const juce::String& type);
    juce::String getNodeTypeName(NodeType type);

    juce::OwnedArray<PluginNodeComponent> nodes;
    juce::OwnedArray<ConnectionComponent> connections;
    
    // Drag line states
    bool isDraggingConnection = false;
    juce::Point<float> dragLineStart;
    juce::Point<float> dragLineEnd;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorCanvas)
};
