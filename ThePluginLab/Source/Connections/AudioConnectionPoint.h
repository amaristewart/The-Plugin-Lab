#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

class PluginNodeComponent;
class PluginEditorCanvas;

class AudioConnectionPoint : public juce::Component,
                           public juce::DragAndDropTarget
{
public:
    AudioConnectionPoint(PluginNodeComponent& owner, ConnectionPointType type, int portIndex);
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Mouse handling
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    
    // DragAndDropTarget
    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& details) override;
    void itemDropped(const juce::DragAndDropTarget::SourceDetails& details) override;
    
    // Connection management
    void disconnect();
    AudioConnectionPoint* findConnectionTarget(const juce::Point<int>& screenPosition) const;
    
    // Getters/Setters
    ConnectionPointType getType() const { return connectionType; }
    int getPortIndex() const { return portIndex; }
    bool getIsConnected() const { return isConnected; }
    PluginNodeComponent& getOwner() const { return ownerNode; }
    void setIsConnected(bool connected) { isConnected = connected; repaint(); }

private:
    PluginNodeComponent& ownerNode;
    ConnectionPointType connectionType;
    int portIndex;
    bool isConnected = false;
    
    // Drag state
    bool isDragging = false;
    juce::Point<float> dragStartPosition;
    juce::Point<float> currentDragPosition;
    
    // Helper methods
    bool canConnect(AudioConnectionPoint* other) const;
    void createConnection(AudioConnectionPoint* other);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConnectionPoint)
};
