#pragma once

#include <JuceHeader.h>
#include "Block.h"

// Forward declare BlockComponent to avoid circular dependencies
class BlockComponent;

//==============================================================================
class CenterCanvas : public juce::Component,
                     public juce::FileDragAndDropTarget,
                     public juce::DragAndDropContainer,
                     public juce::DragAndDropTarget
{
public:
    // Structure to represent a connection between blocks - moved up before it's used
    struct BlockConnection
    {
        BlockComponent* sourceBlock = nullptr;
        BlockComponent* destBlock = nullptr;
    };

    //==============================================================================
    CenterCanvas();
    ~CenterCanvas() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // FileDragAndDropTarget methods
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void dragOperationStarted(const juce::DragAndDropTarget::SourceDetails& details);

    //==============================================================================
    // DragAndDropTarget methods
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragMove(const SourceDetails& dragSourceDetails) override; 
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

    //==============================================================================
    // Helper methods
    void snapToGrid(juce::Point<int>& position);
    void showActivePluginFeedback(int pluginIndex, bool isActive);
    void drawGrid(juce::Graphics& g);

    // Method to create a connection between blocks
    void connectBlocks(BlockComponent* sourceBlock, BlockComponent* destBlock);
    
    // Method to remove a connection between blocks
    void removeConnection(BlockComponent* sourceBlock, BlockComponent* destBlock);

    // Method to notify that a block was moved
    void blockMoved();

    // Method to remove a block
    void removeBlock(BlockComponent* blockToRemove);
    
    // Method to disconnect a specific block connector (input or output)
    void disconnectBlockConnector(BlockComponent* block, bool isInput);
    
    // Method to get all connections (needed for the connection menu)
    const juce::Array<BlockConnection>& getConnections() const { return connections; }
    
    // Method to highlight a connection when hovering over it (visual feedback)
    void highlightConnection(BlockComponent* sourceBlock, BlockComponent* destBlock, bool highlight);

    // Make the trash detection method public so BlockComponents can use it
    bool isPointInDeleteArea(const juce::Point<int>& point) const;
    
    // Add mouseDown to improve drag behavior
    void mouseDown(const juce::MouseEvent& event) override;

    // Override mouseUp to handle blocks dropped on trash
    void mouseUp(const juce::MouseEvent& e) override;

    // Add this method to handle dragging blocks from the canvas
    void startDraggingCanvasBlock(BlockComponent* blockComponent);

private:
    //==============================================================================
    // Grid properties
    int gridSize = 20;
    bool showGrid = true;
    bool enableGridSnapping = true;
    
    // Collection of blocks on the canvas
    juce::OwnedArray<BlockComponent> blocks;

    // Collection of connections between blocks
    juce::Array<BlockConnection> connections;

    // Delete area properties
    juce::Rectangle<int> deleteArea;
    juce::Image trashIcon;
    bool isHighlightingTrash = false;
    
    // Helper methods for trash area
    void drawTrashIcon(juce::Graphics& g);

    // Track highlighted connection for visual feedback
    int highlightedConnectionIndex = -1;

    // Add a tracked dragging block pointer
    BlockComponent* currentlyDraggedCanvasBlock = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CenterCanvas)
};