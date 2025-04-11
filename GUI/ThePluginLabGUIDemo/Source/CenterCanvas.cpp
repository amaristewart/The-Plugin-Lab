#include "CenterCanvas.h"
#include "BlockComponent.h" // Include the header for BlockComponent

//==============================================================================
CenterCanvas::CenterCanvas()
{
    // Initialize with a default size
    setSize(800, 600);
    
    // Enable drag and drop functionality
    setInterceptsMouseClicks(true, true);
    
    // Allow file drag and drop
    setWantsKeyboardFocus(true);
    
    // Add this component as a DragAndDropTarget
    setExplicitFocusOrder(1); // Make sure we can receive mouse events
    
    // Initialize delete area 
    const int trashSize = 60;
    deleteArea = juce::Rectangle<int>(getWidth() - trashSize - 20, 
                                     getHeight() - trashSize - 20,
                                     trashSize, trashSize);
    
    auto trashImage = juce::ImageCache::getFromMemory(BinaryData::TrashIcon_png, BinaryData::TrashIcon_pngSize);
    
    if (! trashImage.isNull())
    {
        trashIcon.setImage(trashImage, juce::Justification::centred);
        int trashHeight = 60;
        trashIcon.setSize(trashImage.getWidth() * trashHeight / trashImage.getHeight(), trashHeight);
    }
    else
    {
        jassert (!trashImage.isNull()); // Ensure logo image is loaded correctly
        juce::Logger::writeToLog("Trash image is null - could not load from BinaryData");
    }
    
    isHighlightingTrash = false;
    
    addAndMakeVisible(trashIcon);
}

CenterCanvas::~CenterCanvas()
{
    // Cleanup code
}

//==============================================================================
void CenterCanvas::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(juce::Colour::fromRGB(255, 255, 255)); // White background

    // Draw dotted grid
    g.setColour(juce::Colours::lightgrey);
    for (int x = 0; x < getWidth(); x += gridSize)
    {
        for (int y = 0; y < getHeight(); y += gridSize)
        {
            g.fillEllipse((float)x, (float)y, 2.0f, 2.0f); // Dots for the grid
        }
    }
    
    // Draw connections between blocks
    g.setColour(juce::Colours::black);
    for (int i = 0; i < connections.size(); ++i)
    {
        auto& connection = connections.getReference(i);
        
        if (connection.sourceBlock != nullptr && connection.destBlock != nullptr)
        {
            // Calculate connector positions properly accounting for block positions
            juce::Point<float> startPoint = connection.sourceBlock->getPosition().toFloat() + 
                                          connection.sourceBlock->getOutputConnectorCentre().toFloat();
            juce::Point<float> endPoint = connection.destBlock->getPosition().toFloat() + 
                                        connection.destBlock->getInputConnectorCentre().toFloat();
            
            float distance = startPoint.getDistanceFrom(endPoint);
            
            if (distance > 0)
            {
                // Calculate control points for a nice curve - 1/3 distance from each end
                float controlOffset = juce::jmin(80.0f, distance * 0.33f);
                juce::Point<float> control1(startPoint.x + controlOffset, startPoint.y);
                juce::Point<float> control2(endPoint.x - controlOffset, endPoint.y);
                
                // Create a path for the connection
                juce::Path path;
                path.startNewSubPath(startPoint);
                path.cubicTo(control1, control2, endPoint);
                
                // Draw the path with a thicker line - highlight if this is the highlighted connection
                if (i == highlightedConnectionIndex)
                {
                    g.setColour(juce::Colours::orange);
                    g.strokePath(path, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
                }
                else
                {
                    g.setColour(juce::Colours::black);
                    g.strokePath(path, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
                }
            }
        }
    }
    
    // Draw in-progress connections
    for (auto* block : blocks)
    {
        if (block->isConnecting())
        {
            // Calculate start position based on which connector is being dragged
            juce::Point<float> startPos = block->getPosition().toFloat();
            if (block->isOutputConnectorBeingDragged())
                startPos += block->getOutputConnectorCentre().toFloat();
            else
                startPos += block->getInputConnectorCentre().toFloat();
                
            // Calculate end position - mouse position
            juce::Point<float> endPos = block->getPosition().toFloat() + block->getCurrentDragPoint().toFloat();
            
            // Draw in-progress connection
            g.setColour(juce::Colours::darkgrey.withAlpha(0.7f));
            
            // Create path for the connection line
            juce::Path path;
            
            // Create a gentle curve based on distance
            float distance = startPos.getDistanceFrom(endPos);
            float controlOffset = juce::jmin(40.0f, distance * 0.33f);
            
            // Control points for bezier curve
            juce::Point<float> control1, control2;
            if (block->isOutputConnectorBeingDragged())
            {
                control1 = juce::Point<float>(startPos.x + controlOffset, startPos.y);
                control2 = juce::Point<float>(endPos.x - controlOffset, endPos.y);
            }
            else
            {
                control1 = juce::Point<float>(startPos.x - controlOffset, startPos.y);
                control2 = juce::Point<float>(endPos.x + controlOffset, endPos.y);
            }
            
            // Create the curved path
            path.startNewSubPath(startPos);
            path.cubicTo(control1, control2, endPos);
            
            // Draw the path
            g.strokePath(path, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
    }
    
    // Draw trash icon with rounded rectangle
    const int trashSize = 60;
    const int padding = 10;
    const int verticalOffset = 8; // Add vertical offset to move rectangle down
    
    // Draw a rounded rectangle around the trash icon - moved down by verticalOffset
    juce::Rectangle<float> trashBounds = deleteArea.toFloat().expanded(padding).translated(0, verticalOffset);
    float cornerSize = 12.0f;
    
    // Change color based on hover state
    g.setColour(isHighlightingTrash ? juce::Colours::red.withAlpha(0.6f) : juce::Colours::darkgrey.withAlpha(0.5f));
    g.fillRoundedRectangle(trashBounds, cornerSize);
    
    // Add a border with stronger color
    g.setColour(isHighlightingTrash ? juce::Colours::red : juce::Colours::darkgrey);
    g.drawRoundedRectangle(trashBounds, cornerSize, 2.0f);
    
    // Add a "Trash" label beneath the icon - also move down by the same offset
    g.setColour(isHighlightingTrash ? juce::Colours::white : juce::Colours::lightgrey);
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("Trash", 
               deleteArea.getX() - padding, 
               deleteArea.getBottom() - 14 + verticalOffset,
               deleteArea.getWidth() + padding * 2,
               20,
               juce::Justification::centred, true);
}

void CenterCanvas::resized()
{
    // Ensure blocks are properly aligned within the canvas
    for (auto* block : blocks)
    {
        // Adjust block size or position if needed
        block->setBounds(block->getBounds()); // Keep the current bounds
    }
    
    const int trashSize = 60;
    deleteArea = juce::Rectangle<int>(getWidth() - trashSize - 20,
                                     getHeight() - trashSize - 20,
                                     trashSize, trashSize);
    trashIcon.setBounds(deleteArea);
}

//==============================================================================
// Drag and drop functionality
bool CenterCanvas::isInterestedInFileDrag(const juce::StringArray& files)
{
    // Check if any of the files are of a type we're interested in
    return true; // For now, accept all files
}

void CenterCanvas::filesDropped(const juce::StringArray& files, int x, int y)
{
    // Create a new BlockComponent at the drop position
    auto* block = new BlockComponent("New Block", juce::Colours::orange);
    block->setBounds(x, y, 100, 50); // Set the position and size of the block
    addAndMakeVisible(block); // Add the block to the canvas
    blocks.add(block); // Store the block in the collection
}

void CenterCanvas::dragOperationStarted(const juce::DragAndDropTarget::SourceDetails& details)
{
    // We should NOT automatically create a new block when drag starts
    // This method should be left empty to prevent duplicate block creation
    // Do nothing here
}

bool CenterCanvas::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
    // Accept all drag sources - we'll check the content in itemDropped
    return true;
}

void CenterCanvas::itemDragEnter(const SourceDetails& dragSourceDetails)
{
    // Visual feedback when a draggable item enters the canvas
    repaint();
}

void CenterCanvas::itemDragExit(const SourceDetails& dragSourceDetails)
{
    // Reset visual feedback when a draggable item exits the canvas
    isHighlightingTrash = false;
    currentlyDraggedCanvasBlock = nullptr; // Reset the tracked block
    repaint();
}

void CenterCanvas::itemDragMove(const SourceDetails& dragSourceDetails)
{
    // Make the trash area more responsive by using a significantly enlarged hit area
    bool wasHighlighted = isHighlightingTrash;
    
    // Create a larger hit area to make it easier to hit
    juce::Rectangle<int> expandedDeleteArea = deleteArea.expanded(20);
    isHighlightingTrash = expandedDeleteArea.contains(dragSourceDetails.localPosition.toInt());
    
    // Only repaint if the state changed
    if (wasHighlighted != isHighlightingTrash)
        repaint(); // Repaint the entire canvas for smoother visuals
}

void CenterCanvas::itemDropped(const SourceDetails& dragSourceDetails)
{
    // Get the component being dragged
    auto* draggedBlock = dynamic_cast<BlockComponent*>(dragSourceDetails.sourceComponent.get());
    
    // Debug logging
    juce::Logger::writeToLog("Block dropped - Is in delete area: " + 
                          juce::String(isPointInDeleteArea(dragSourceDetails.localPosition.toInt()) ? "YES" : "NO"));
                          
    if (draggedBlock != nullptr)
    {
        juce::Logger::writeToLog("Block type: " + draggedBlock->getName());
        juce::Logger::writeToLog("Is from sidebar: " + juce::String(draggedBlock->isInSidebar() ? "Yes" : "No"));
    }

    // Check if dropped on the delete area
    if (isPointInDeleteArea(dragSourceDetails.localPosition.toInt()))
    {
        isHighlightingTrash = false;
        juce::Logger::writeToLog("Drop detected in trash area");
        
        // Process for blocks already on the canvas
        if (draggedBlock != nullptr && draggedBlock->getParentComponent() == this)
        {
            juce::Logger::writeToLog("Processing canvas block for deletion");
            
            // Remove the block
            removeBlock(draggedBlock);
            currentlyDraggedCanvasBlock = nullptr;
            return;
        }
        // Handle blocks from the sidebar
        else if (draggedBlock != nullptr && draggedBlock->isInSidebar())
        {
            juce::Logger::writeToLog("Ignoring block from sidebar");
            repaint();
            currentlyDraggedCanvasBlock = nullptr;
            return;
        }
        
        repaint();
        currentlyDraggedCanvasBlock = nullptr;
        return;
    }
    
    // Handle normal block drops from here (when NOT over trash)
    juce::String description = dragSourceDetails.description.toString();
    
    // Check if this is a block coming from the sidebar
    if (description.startsWith("BlockDrag:") && draggedBlock != nullptr && draggedBlock->isInSidebar())
    {
        // Create a new block from the sidebar
        // Parse the description to get the block name and color
        juce::StringArray parts;
        parts.addTokens(description, ":", "");
        
        if (parts.size() >= 3)
        {
            juce::String blockName = parts[1];
            juce::Colour blockColour;
            
            // Parse the color correctly
            if (parts[2].startsWith("0x") || parts[2].startsWith("#"))
            {
                blockColour = juce::Colour::fromString(parts[2]);
            }
            else
            {
                // Try to parse RGB values if provided as comma-separated
                if (parts[2].contains(","))
                {
                    juce::StringArray rgbParts;
                    rgbParts.addTokens(parts[2], ",", "");
                    if (rgbParts.size() >= 3)
                    {
                        blockColour = juce::Colour(rgbParts[0].getIntValue(), 
                                               rgbParts[1].getIntValue(), 
                                               rgbParts[2].getIntValue());
                    }
                }
                else
                {
                    // Use a category-specific default color based on the block name
                    if (blockName.contains("Button") || blockName.contains("Slider") || blockName.contains("Knob"))
                        blockColour = juce::Colour::fromRGB(91, 187, 91); // GUI color
                    else if (blockName.contains("Highpass") || blockName.contains("Lowpass") || blockName.contains("Peaking"))
                        blockColour = juce::Colour::fromRGB(250, 144, 167); // EQ color
                    else if (blockName.contains("Frequency") || blockName.contains("Q") || blockName.contains("Gain"))
                        blockColour = juce::Colour::fromRGB(254, 204, 214); // EQ parameter color
                    else if (blockName.contains("Compressor") || blockName.contains("Limiter") || 
                           blockName.contains("Expander") || blockName.contains("Gate"))
                        blockColour = juce::Colour::fromRGB(127, 222, 224); // Dynamics color
                    else if (blockName.contains("Spectrum") || blockName.contains("Waveform") || blockName.contains("Meter"))
                        blockColour = juce::Colour::fromRGB(187, 91, 187); // Visualizer color
                    else
                        blockColour = juce::Colours::orange; // Default fallback
                }
            }
            
            // Create a new block at the drop position
            auto* newBlock = new BlockComponent(blockName, blockColour);
            
            // Calculate the position, accounting for offset
            juce::Point<int> dropPos(dragSourceDetails.localPosition.x - newBlock->getWidth()/2,
                                   dragSourceDetails.localPosition.y - newBlock->getHeight()/2);
            
            // Apply grid snapping if enabled
            if (enableGridSnapping)
                snapToGrid(dropPos);
            
            // Ensure blocks don't go behind the left sidebar (min X position)
            const int minX = 5;  // Add a small margin
            if (dropPos.x < minX)
                dropPos.x = minX;
            
            // Set the bounds and make the block visible
            newBlock->setBounds(dropPos.x, dropPos.y, newBlock->getWidth(), newBlock->getHeight());
            addAndMakeVisible(newBlock);
            blocks.add(newBlock);
            
            juce::Logger::writeToLog("Created new block on canvas: " + blockName);
            repaint();
        }
    }
    // If this is a block already on the canvas being repositioned
    else if (description.startsWith("CanvasBlockDrag:") || 
            (draggedBlock != nullptr && draggedBlock->getParentComponent() == this))
    {
        // Block already exists on canvas, no need to create a new one
        juce::Logger::writeToLog("Block repositioned on canvas");
        currentlyDraggedCanvasBlock = nullptr;
    }
}

// Add special handling for the mouseUp event to ensure blocks can be deleted
void CenterCanvas::mouseUp(const juce::MouseEvent& e)
{
    // Get the original source component that received the mouseDown
    BlockComponent* draggedComponent = currentlyDraggedCanvasBlock;
    
    // If we didn't identify a component yet, search through children
    if (draggedComponent == nullptr)
    {
        for (auto* child : getChildren())
        {
            auto* blockComp = dynamic_cast<BlockComponent*>(child);
            if (blockComp != nullptr && !blockComp->isInSidebar() && blockComp->isMouseOverOrDragging())
            {
                draggedComponent = blockComp;
                break;
            }
        }
    }
    
    // If we have a block and it's over the trash area, delete it
    if (draggedComponent != nullptr)
    {
        juce::Point<int> mousePos = e.getEventRelativeTo(this).getPosition();
        if (isPointInDeleteArea(mousePos))
        {
            juce::Logger::writeToLog("Mouse up over trash - removing block: " + draggedComponent->getName());
            removeBlock(draggedComponent);
        }
    }
    
    // Reset the tracked block regardless
    currentlyDraggedCanvasBlock = nullptr;
}

// Add this to enable proper mouse capture for drag-drop operations
void CenterCanvas::mouseDown(const juce::MouseEvent& event)
{
    // Enable this component to receive drag events even if the click
    // was initially on a child component like a BlockComponent
    if (auto* blockComponent = dynamic_cast<BlockComponent*>(event.eventComponent))
    {
        // Start watching this potential drag operation
        beginDragAutoRepeat(50); // Will help with smoother drag operations
    }
}

// Add this method to handle dragging blocks from the canvas
void CenterCanvas::startDraggingCanvasBlock(BlockComponent* blockComponent)
{
    // Track which block is being dragged
    currentlyDraggedCanvasBlock = blockComponent;
    
    // Start the drag operation with a specific description to identify canvas blocks
    juce::String description = "CanvasBlockDrag:" + blockComponent->getName();
    startDragging(description, blockComponent);
}

//==============================================================================
// Helper methods
void CenterCanvas::snapToGrid(juce::Point<int>& position)
{
    // If grid snapping is enabled, snap the position to the nearest grid point
    if (enableGridSnapping)
    {
        position.setX((position.getX() / gridSize) * gridSize);
        position.setY((position.getY() / gridSize) * gridSize);
    }
}

void CenterCanvas::showActivePluginFeedback(int pluginIndex, bool isActive)
{
    // Visually indicate whether a plugin is active or not
    // This could update the visual state of a block
}

void CenterCanvas::drawGrid(juce::Graphics& g)
{
    // Draw the grid lines
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    
    int width = getWidth();
    int height = getHeight();
    
    // Draw vertical lines
    for (int x = 0; x < width; x += gridSize)
    {
        g.drawLine((float)x, 0.0f, (float)x, (float)height, 1.0f);
    }
    
    // Draw horizontal lines
    for (int y = 0; y < height; y += gridSize)
    {
        g.drawLine(0.0f, (float)y, (float)width, (float)y, 1.0f);
    }
}

void CenterCanvas::connectBlocks(BlockComponent* sourceBlock, BlockComponent* destBlock)
{
    if (sourceBlock != nullptr && destBlock != nullptr)
    {
        // Check if this connection already exists
        bool connectionExists = false;
        for (auto& conn : connections)
        {
            if (conn.sourceBlock == sourceBlock && conn.destBlock == destBlock)
            {
                connectionExists = true;
                break;
            }
        }
        
        // Only add if it doesn't exist already
        if (!connectionExists)
        {
            BlockConnection connection;
            connection.sourceBlock = sourceBlock;
            connection.destBlock = destBlock;
            connections.add(connection);
            
            // Play a sound or provide visual feedback for successful connection
            // This could be a subtle animation, color change, or sound
            
            repaint(); // Redraw to show the connection
        }
    }
}

void CenterCanvas::removeConnection(BlockComponent* sourceBlock, BlockComponent* destBlock)
{
    // Remove a specific connection between blocks
    for (int i = connections.size() - 1; i >= 0; --i)
    {
        if (connections[i].sourceBlock == sourceBlock && 
            connections[i].destBlock == destBlock)
        {
            connections.remove(i);
            break;
        }
    }
    
    repaint();
}

void CenterCanvas::blockMoved()
{
    // Check if any blocks are out of bounds and adjust their positions
    for (auto* block : blocks)
    {
        auto bounds = block->getBounds();
        bool changed = false;
        
        // Ensure blocks don't go behind the left sidebar (min X position)
        const int minX = 5;
        if (bounds.getX() < minX)
        {
            bounds.setX(minX);
            changed = true;
        }
        
        // Also prevent blocks from going off the right, top, or bottom edges
        if (bounds.getRight() > getWidth())
        {
            bounds.setX(getWidth() - bounds.getWidth());
            changed = true;
        }
        
        if (bounds.getY() < 0)
        {
            bounds.setY(0);
            changed = true;
        }
        
        if (bounds.getBottom() > getHeight())
        {
            bounds.setY(getHeight() - bounds.getHeight());
            changed = true;
        }
        
        if (changed)
        {
            // Update the block position without triggering additional moved() callbacks
            block->setBounds(bounds);
        }
    }
    
    // Redraw to update all connections
    repaint();
}

bool CenterCanvas::isPointInDeleteArea(const juce::Point<int>& point) const
{
    // Update hit detection area to match the new visually offset rectangle
    const int verticalOffset = 10; // Same offset as in paint method
    
    // Use a significantly enlarged delete area for easier targeting, with the vertical offset applied
    auto expandedArea = deleteArea.expanded(30).translated(0, verticalOffset);
    bool result = expandedArea.contains(point);
    juce::Logger::writeToLog("Point check x:" + juce::String(point.x) + " y:" + juce::String(point.y) + 
                           " in trash: " + juce::String(result ? "YES" : "NO"));
    return result;
}

void CenterCanvas::removeBlock(BlockComponent* blockToRemove)
{
    if (blockToRemove != nullptr)
    {
        // First remove any connections involving this block
        for (int i = connections.size() - 1; i >= 0; --i)
        {
            if (connections[i].sourceBlock == blockToRemove || 
                connections[i].destBlock == blockToRemove)
            {
                connections.remove(i);
            }
        }
        
        // Then remove the block itself
        for (int i = 0; i < blocks.size(); ++i)
        {
            if (blocks[i] == blockToRemove)
            {
                blocks.remove(i, true); // 'true' deletes the component
                break;
            }
        }
        
        repaint();
    }
}

void CenterCanvas::disconnectBlockConnector(BlockComponent* block, bool isInput)
{
    if (block == nullptr)
        return;
    
    // Find and remove all connections involving this connector
    for (int i = connections.size() - 1; i >= 0; --i)
    {
        if (isInput && connections[i].destBlock == block)
        {
            connections.remove(i);
        }
        else if (!isInput && connections[i].sourceBlock == block)
        {
            connections.remove(i);
        }
    }
    
    repaint();
}

void CenterCanvas::highlightConnection(BlockComponent* sourceBlock, BlockComponent* destBlock, bool highlight)
{
    if (sourceBlock == nullptr || destBlock == nullptr)
    {
        highlightedConnectionIndex = -1;
        repaint();
        return;
    }
    
    // Find the connection
    for (int i = 0; i < connections.size(); ++i)
    {
        if (connections[i].sourceBlock == sourceBlock && connections[i].destBlock == destBlock)
        {
            highlightedConnectionIndex = highlight ? i : -1;
            repaint();
            return;
        }
    }
    
    // If not found and we were asked to unhighlight, make sure nothing is highlighted
    if (!highlight)
    {
        highlightedConnectionIndex = -1;
        repaint();
    }
}
