#include "BlockComponent.h"
#include "CenterCanvas.h"

// Implement the connectToCanvas method
void BlockComponent::connectToCanvas(BlockComponent* source, BlockComponent* dest)
{
    auto* parent = getParentComponent();
    if (parent != nullptr)
    {
        // Use dynamic_cast to check if parent is a CenterCanvas
        auto* canvas = dynamic_cast<CenterCanvas*>(parent);
        if (canvas != nullptr)
        {
            // Now we can safely call the method on CenterCanvas
            canvas->connectBlocks(source, dest);
        }
    }
}

// Implement the moved method
void BlockComponent::moved()
{
    // Call the base class implementation first
    Component::moved();
    
    // Notify parent if needed
    if (!isInSidebar())
    {
        notifyParentOfMovement();
    }
}

// Implement helper to notify parent when block is moved
void BlockComponent::notifyParentOfMovement()
{
    if (auto* parent = getParentComponent())
    {
        // Use dynamic_cast to check if parent is a CenterCanvas
        if (auto* canvas = dynamic_cast<CenterCanvas*>(parent))
        {
            canvas->blockMoved();
        }
    }
}

// Show a popup menu for connection management
void BlockComponent::showConnectionMenu()
{
    juce::PopupMenu menu;
    auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
    
    if (canvas != nullptr)
    {
        // Find all connections involving this block
        int connectionCount = 0;
        
        // Get all connections from the canvas
        auto connections = canvas->getConnections();
        
        // Add menu items for each connection
        for (int i = 0; i < connections.size(); ++i)
        {
            auto& connection = connections.getReference(i);
            
            // Check if this block is involved in the connection
            if (connection.sourceBlock == this || connection.destBlock == this)
            {
                connectionCount++;
                
                bool isOutput = (connection.sourceBlock == this);
                juce::String directionText = isOutput ? "Output to: " : "Input from: ";
                juce::String blockName = isOutput ? connection.destBlock->getName() : connection.sourceBlock->getName();
                
                // Create menu item with connection details and index stored as ID
                menu.addItem(i + 1, directionText + blockName);
            }
        }
        
        if (connectionCount == 0)
        {
            menu.addItem(1, "No connections", false);
        }
        
        menu.addSeparator();
    }
    
    menu.addItem(10000, "Delete Block", true);
    
    // Show the menu
    menu.showMenuAsync(juce::PopupMenu::Options()
                      .withTargetComponent(this)
                      .withMinimumWidth(120)
                      .withMaximumNumColumns(1),
                      [this, canvas](int result) {
                          if (result == 10000 && canvas != nullptr)
                          {
                              // Delete this block
                              canvas->removeBlock(this);
                          }
                          else if (result > 0 && result < 10000)
                          {
                              // Handle connection removal based on the selected menu item
                              if (canvas != nullptr)
                              {
                                  // Get the connection index (result-1)
                                  int connectionIndex = result - 1;
                                  auto connections = canvas->getConnections();
                                  
                                  if (connectionIndex >= 0 && connectionIndex < connections.size())
                                  {
                                      auto& connection = connections.getReference(connectionIndex);
                                      canvas->removeConnection(connection.sourceBlock, connection.destBlock);
                                  }
                              }
                          }
                      });
}

void BlockComponent::mouseDown(const juce::MouseEvent& e)
{
    // Handle right click for context menu
    if (e.mods.isRightButtonDown() && !isInSidebar())
    {
        showConnectionMenu();
        return;
    }
    
    // Handle double-click on connectors to disconnect
    if (e.getNumberOfClicks() == 2 && !isInSidebar())
    {
        auto pos = e.getPosition();
        juce::Rectangle<int> inputHitArea = inputConnector.getBounds().expanded(10);
        juce::Rectangle<int> outputHitArea = outputConnector.getBounds().expanded(10);
        
        if (inputHitArea.contains(pos) || outputHitArea.contains(pos))
        {
            auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
            if (canvas != nullptr)
            {
                // Find and disconnect connections for this connector
                bool isInput = inputHitArea.contains(pos);
                canvas->disconnectBlockConnector(this, isInput);
                return;
            }
        }
    }
    
    // Check if we're in the sidebar or on the canvas
    if (isInSidebar())
    {
        // Start drag-and-drop operation from sidebar
        auto* container = juce::DragAndDropContainer::findParentDragContainerFor(this);
        if (container != nullptr)
        {
            // Create a description string with block info and RGB color values
            juce::String colorStr = juce::String(colour.getRed()) + "," + 
                                  juce::String(colour.getGreen()) + "," + 
                                  juce::String(colour.getBlue());
            
            juce::String description = "BlockDrag:" + name + ":" + colorStr;
            
            // Start the drag operation
            container->startDragging(description, this);
        }
    }
    else
    {
        // We're on the canvas
        auto pos = e.getPosition();
        
        // Check if clicked on input connector - make hit target bigger
        juce::Rectangle<int> inputHitArea = inputConnector.getBounds().expanded(5);
        juce::Rectangle<int> outputHitArea = outputConnector.getBounds().expanded(5);
        
        if (inputHitArea.contains(pos))
        {
            beginConnectorDrag(false, pos);
        }
        // Check if clicked on output connector
        else if (outputHitArea.contains(pos))
        {
            beginConnectorDrag(true, pos);
        }
        else
        {
            // Start the component dragging
            dragger.startDraggingComponent(this, e);
            
            // Enable dragging to trash for blocks on canvas
            auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
            if (canvas != nullptr)
            {
                canvas->startDraggingCanvasBlock(this);
            }
        }
    }
}

// Optional: Add a highlightConnections implementation
void BlockComponent::highlightConnections(bool highlight)
{
    // Find the parent canvas
    auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
    if (canvas == nullptr)
        return;
    
    // Get all connections from the canvas
    auto& connections = canvas->getConnections();
    
    // Highlight connections that involve this block
    for (int i = 0; i < connections.size(); ++i)
    {
        auto& connection = connections.getReference(i);
        if (connection.sourceBlock == this || connection.destBlock == this)
        {
            canvas->highlightConnection(connection.sourceBlock, connection.destBlock, highlight);
        }
    }
}
