#include "BlockComponent.h"
#include "CenterCanvas.h"

//==============================================================================
// ConnectorComponent Implementation
//==============================================================================
ConnectorComponent::ConnectorComponent(const juce::String& tooltipText)
    : connectorTooltip(tooltipText)
{
}

juce::String ConnectorComponent::getTooltip()
{
    return connectorTooltip;
}

//==============================================================================
// BlockComponent Implementation
//==============================================================================

BlockComponent::BlockComponent(const juce::String& blockName, juce::Colour blockColour)
{
    name = blockName;
    colour = blockColour;
    
    // Enable the component to be dragged
    setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    
    // Create four connector components
    leftConnector = std::make_unique<ConnectorComponent>("Left connector: Connect to the output of another block");
    rightConnector = std::make_unique<ConnectorComponent>("Right connector: Connect to the input of another block");
    topConnector = std::make_unique<ConnectorComponent>("Top connector: Connect to the bottom of another block");
    bottomConnector = std::make_unique<ConnectorComponent>("Bottom connector: Connect to the top of another block");
    
    // Add connectors as child components
    addAndMakeVisible(leftConnector.get());
    addAndMakeVisible(rightConnector.get());
    addAndMakeVisible(topConnector.get());
    addAndMakeVisible(bottomConnector.get());
    
    // Set the size
    setSize(120, 60);
    
    // Determine shape based on name or color
    determineBlockShape();
    
    // Set appropriate tooltip based on block type
    setTooltipByBlockType();
}

BlockComponent::BlockComponent(const juce::String& blockName, juce::Colour blockColour, const juce::Image& blockImage)
{
    name = blockName;
    colour = blockColour;
    image = blockImage;
    
    // Enable the component to be dragged
    setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    
    // Create four connector components
    leftConnector = std::make_unique<ConnectorComponent>("Left connector: Connect to the output of another block");
    rightConnector = std::make_unique<ConnectorComponent>("Right connector: Connect to the input of another block");
    topConnector = std::make_unique<ConnectorComponent>("Top connector: Connect to the bottom of another block");
    bottomConnector = std::make_unique<ConnectorComponent>("Bottom connector: Connect to the top of another block");
    
    // Add connectors as child components
    addAndMakeVisible(leftConnector.get());
    addAndMakeVisible(rightConnector.get());
    addAndMakeVisible(topConnector.get());
    addAndMakeVisible(bottomConnector.get());
    
    // Set the size
    setSize(120, 60);
    
    // Determine shape based on name or color
    determineBlockShape();
    
    // If we have an image, use it
    hasImage = image.isValid();
    
    // Set appropriate tooltip based on block type
    setTooltipByBlockType();
}

void BlockComponent::addTextEntryIfNeeded()
{
    // Only add text entry for Frequency, Q, and Gain blocks
    if ((name == "Frequency" || name == "Q" || name == "Gain") && !isInSidebar() && !textEntry)
    {
        textEntry = std::make_unique<juce::TextEditor>("valueEntry");
        textEntry->setMultiLine(false);
        textEntry->setReturnKeyStartsNewLine(false);
        textEntry->setReadOnly(false);
        textEntry->setScrollbarsShown(false);
        textEntry->setCaretVisible(true);
        textEntry->setPopupMenuEnabled(true);
        
        // Set default value based on the block name with appropriate units
        if (name == "Frequency")
            textEntry->setText("1000 Hz");
        else if (name == "Q")
            textEntry->setText("1.0");
        else if (name == "Gain")
            textEntry->setText("0.0 dB");
            
        textEntry->setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
        textEntry->setColour(juce::TextEditor::outlineColourId, juce::Colours::black);
        textEntry->setColour(juce::TextEditor::textColourId, juce::Colours::black);
        textEntry->setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::blue);
        textEntry->applyColourToAllText(juce::Colours::black, true);
        textEntry->setFont(juce::Font(14.0f, juce::Font::bold));
        textEntry->setJustification(juce::Justification::centred);
        
        if (auto* parent = getParentComponent())
        {
            parent->addAndMakeVisible(textEntry.get());
            parent->repaint();
        }
        else
        {
            addAndMakeVisible(textEntry.get());
        }
        
        positionTextEntry();
    }
}

void BlockComponent::positionTextEntry()
{
    if (textEntry != nullptr)
    {
        const int textHeight = 25;
        const int margin = 5;
        
        int xPos = getX() + margin;
        int yPos = getY() + getHeight() + 5; 
        int width = getWidth() - (margin * 2);
        
        // Set the bounds in parent coordinate space if added to parent
        if (textEntry->getParentComponent() != this && textEntry->getParentComponent() != nullptr)
        {
            textEntry->setBounds(xPos, yPos, width, textHeight);
        }
        else
        {
            // Set in local coordinate space if added to this component
            textEntry->setBounds(margin, getHeight() + 5, width, textHeight);
        }
        
        textEntry->toFront(false);
    }
}

void BlockComponent::paint(juce::Graphics& g)
{
    // If we have an image, use it without drawing a background shape
    if (hasImage)
    {        g.drawImageWithin(image,
                         0, 0,
                         getWidth(), getHeight(),
                         juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
    }
    else
    {
        // For blocks without images, use the original shape drawing
        switch (blockShape)
        {
            case Circle:
                paintCircle(g, false);
                break;
            case RoundedSquare:
                paintRoundedSquare(g, false);
                break;
            case Triangle:
                paintTriangle(g, false);
                break;
            case Hexagon:
                paintHexagon(g, false);
                break;
        }
    }
    
    // Draw connectors with white circles and black outlines
    for (const auto& connector : { leftConnector.get(), rightConnector.get(), topConnector.get(), bottomConnector.get() })
    {
        g.setColour(juce::Colours::white);
        g.fillEllipse(connector->getBounds().toFloat());
        
        g.setColour(juce::Colours::black);
        g.drawEllipse(connector->getBounds().toFloat(), 1.0f);
    }
    
    if (textEntry != nullptr && textEntry->isVisible())
    {
        auto textBounds = textEntry->getBounds();
        
        g.setColour(juce::Colours::white);
        g.fillRoundedRectangle(textBounds.toFloat().expanded(2), 4.0f);
        
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(textBounds.toFloat().expanded(2), 4.0f, 1.5f);
    }
}

void BlockComponent::resized()
{
    auto bounds = getLocalBounds();
    
    positionTextEntry();
    
    auto mainBounds = bounds;
    if (textEntry != nullptr)
    {
        mainBounds = bounds.withHeight(bounds.getHeight() - 30);
    }
    
    const int connectorSize = 10;
    
    leftConnector->setBounds(
        0,
        mainBounds.getCentreY() - connectorSize/2,
        connectorSize,
        connectorSize
    );
    
    rightConnector->setBounds(
        mainBounds.getWidth() - connectorSize,
        mainBounds.getCentreY() - connectorSize/2,
        connectorSize,
        connectorSize
    );
    
    topConnector->setBounds(
        mainBounds.getCentreX() - connectorSize/2,
        0,
        connectorSize,
        connectorSize
    );
    
    bottomConnector->setBounds(
        mainBounds.getCentreX() - connectorSize/2,
        mainBounds.getHeight() - connectorSize,
        connectorSize,
        connectorSize
    );
    
    if (hasImage)
    {
        // Check if this is an EQ-related block with an image
        if (isEQBlock())
        {
            // For EQ blocks with images, apply the EQ connector positioning
            adjustEQBlockConnectors(mainBounds, connectorSize);
        }
        else if (isGUIBlock())
        {
            // For other image blocks, use the standard image customization
            adjustGUIBlockConnectors(mainBounds, connectorSize);
        }
    }
    else if (blockShape == Triangle)
    {
        // For triangles, adjust connector positions to match the shape
        adjustTriangleConnectors(mainBounds, connectorSize);
    }
    else if (blockShape == Hexagon)
    {
        // For hexagons, adjust connector positions to match the shape
        adjustHexagonConnectors(mainBounds, connectorSize);
    }
    else if (blockShape == RoundedSquare)
    {
        // For EQ blocks (rounded squares), modify connector positions
        adjustEQBlockConnectors(mainBounds, connectorSize);
    }
}

void BlockComponent::moved()
{
    Component::moved();
    
    positionTextEntry();
    
    if (!isInSidebar())
    {
        notifyParentOfMovement();
    }
}

void BlockComponent::mouseDown(const juce::MouseEvent& e)
{
    juce::String tooltipText = getTooltip();
    
    // Right click for context menu
    if (e.mods.isRightButtonDown() && !isInSidebar())
    {
        showConnectionMenu();
        return;
    }
    
    // Check if mouse is over a connector and display connector-specific tooltip
    auto pos = e.getPosition();
    
    if (isPointOverConnector(pos, Left))
    {
        // Show left connector tooltip
        tooltipText = leftConnector->getTooltip();
    }
    else if (isPointOverConnector(pos, Right))
    {
        // Show right connector tooltip
        tooltipText = rightConnector->getTooltip();
    }
    else if (isPointOverConnector(pos, Top))
    {
        // Show top connector tooltip
        tooltipText = topConnector->getTooltip();
    }
    else if (isPointOverConnector(pos, Bottom))
    {
        // Show bottom connector tooltip
        tooltipText = bottomConnector->getTooltip();
    }
    
    // Display tooltip using JUCE's tooltip manager
    if (tooltipText.isNotEmpty())
    {
    }
    
    // Handle double-click on connectors to disconnect
    if (e.getNumberOfClicks() == 2 && !isInSidebar())
    {
        auto pos = e.getPosition();
        
        // Check which connector was double-clicked
        if (isPointOverConnector(pos, Left))
        {
            auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
            if (canvas != nullptr)
                canvas->disconnectBlockConnector(this, Left);
        }
        else if (isPointOverConnector(pos, Right))
        {
            auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
            if (canvas != nullptr)
                canvas->disconnectBlockConnector(this, Right);
        }
        else if (isPointOverConnector(pos, Top))
        {
            auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
            if (canvas != nullptr)
                canvas->disconnectBlockConnector(this, Top);
        }
        else if (isPointOverConnector(pos, Bottom))
        {
            auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
            if (canvas != nullptr)
                canvas->disconnectBlockConnector(this, Bottom);
        }
        
        return;
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
        
        // Check if clicked on any connector with larger hit area
        if (isPointOverConnector(pos, Left))
        {
            beginConnectorDrag(Left, pos);
        }
        else if (isPointOverConnector(pos, Right))
        {
            beginConnectorDrag(Right, pos);
        }
        else if (isPointOverConnector(pos, Top))
        {
            beginConnectorDrag(Top, pos);
        }
        else if (isPointOverConnector(pos, Bottom))
        {
            beginConnectorDrag(Bottom, pos);
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

void BlockComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (isInSidebar())
        return; // Don't drag in the sidebar
    
    if (isConnecting_)
    {
        // Update the connector position for drawing
        currentDragPoint_ = e.getPosition();
        getParentComponent()->repaint(); // Make sure the connection line gets redrawn
    }
    else
    {
        // Normal component dragging
        dragger.dragComponent(this, e, nullptr);
        
        getParentComponent()->repaint();
    }
}

void BlockComponent::mouseUp(const juce::MouseEvent& e)
{
    if (isConnecting_)
    {
        // Check if we released over another block's connector
        auto* parent = getParentComponent();
        if (parent != nullptr)
        {
            auto mousePos = e.getEventRelativeTo(parent).getPosition();
            
            // Find if there's another block at this position
            for (auto* siblingComp : parent->getChildren())
            {
                auto* sibling = dynamic_cast<BlockComponent*>(siblingComp);
                if (sibling != nullptr && sibling != this)
                {
                    ConnectorPosition compatiblePosition = getCompatibleConnectorPosition(activeConnectorPosition);
                    ConnectorPosition hitPosition;
                    
                    // Check if we're over the compatible connector on the target block
                    if (checkConnectorHit(sibling, mousePos, hitPosition) && hitPosition == compatiblePosition)
                    {
                        // Connect the blocks via parent canvas
                        connectToCanvas(this, sibling, activeConnectorPosition);
                        break;
                    }
                }
            }
        }
        
        // Reset connecting state
        isConnecting_ = false;
        getParentComponent()->repaint();
    }
    else if (!isInSidebar())
    {
        // If we were dragging the block, notify the parent for connection update
        notifyParentOfMovement();
    }
}

bool BlockComponent::isInSidebar() const
{
    return findParentComponentOfClass<juce::Viewport>() != nullptr;
}

juce::Point<int> BlockComponent::getConnectorCentre(ConnectorPosition position) const
{
    switch (position)
    {
        case Left:   return leftConnector->getBounds().getCentre();
        case Right:  return rightConnector->getBounds().getCentre();
        case Top:    return topConnector->getBounds().getCentre();
        case Bottom: return bottomConnector->getBounds().getCentre();
        default:     return leftConnector->getBounds().getCentre();
            
    }
}

juce::Point<int> BlockComponent::getInputConnectorCentre() const
{
    return leftConnector->getBounds().getCentre();
}

juce::Point<int> BlockComponent::getOutputConnectorCentre() const
{
    return rightConnector->getBounds().getCentre();
}

bool BlockComponent::isConnecting() const
{
    return isConnecting_;
}

BlockComponent::ConnectorPosition BlockComponent::getActiveConnectorPosition() const
{
    return activeConnectorPosition;
}

juce::Point<int> BlockComponent::getCurrentDragPoint() const
{
    return currentDragPoint_;
}

bool BlockComponent::isOutputConnectorBeingDragged() const
{
    return isConnecting_ && activeConnectorPosition == Right;
}

bool BlockComponent::isInputConnectorBeingDragged() const
{
    return isConnecting_ && activeConnectorPosition == Left;
}

bool BlockComponent::isTopConnectorBeingDragged() const
{
    return isConnecting_ && activeConnectorPosition == Top;
}

bool BlockComponent::isBottomConnectorBeingDragged() const
{
    return isConnecting_ && activeConnectorPosition == Bottom;
}

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
    
    menu.showMenuAsync(juce::PopupMenu::Options()
                      .withTargetComponent(this)
                      .withMinimumWidth(120)
                      .withMaximumNumColumns(1),
                      [this, canvas](int result) {
                          if (result == 10000 && canvas != nullptr)
                          {
                              canvas->removeBlock(this);
                          }
                          else if (result > 0 && result < 10000)
                          {
                              // Handle connection removal based on the selected menu item
                              if (canvas != nullptr)
                              {
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

void BlockComponent::prepareForDeletion()
{
    // Clear the text entry pointer before the component gets deleted
    if (textEntry != nullptr && textEntry->getParentComponent() != this)
    {
        textEntry->getParentComponent()->removeChildComponent(textEntry.get());
    }
    textEntry = nullptr;
}

bool BlockComponent::isSpectrumVisualizer() const
{
    return false;
}

juce::String BlockComponent::getTooltip()
{
    return tooltipText;
}

void BlockComponent::determineBlockShape()
{
    // GUI blocks (green) are circles
    if (name.contains("Button") || name.contains("Slider") || name.contains("Knob") ||
        (colour.getRed() == 91 && colour.getGreen() == 187 && colour.getBlue() == 91))
    {
        blockShape = Circle;
    }
    // EQ blocks (pink) are rounded squares
    else if (name.contains("Highpass") || name.contains("Lowpass") || name.contains("Peaking") ||
             name.contains("Frequency") || name.contains("Q") || name.contains("Gain") ||
             (colour.getRed() == 250 && colour.getGreen() == 144 && colour.getBlue() == 167) ||
             (colour.getRed() == 254 && colour.getGreen() == 204 && colour.getBlue() == 214))
    {
        blockShape = RoundedSquare;
    }
    // Dynamics blocks (blue-green) are triangles
    else if (name.contains("Compressor") || name.contains("Limiter") ||
             name.contains("Expander") || name.contains("Gate") ||
             name.contains("Threshold") || name.contains("Ratio") ||
             name.contains("Attack") || name.contains("Release") ||
             (colour.getRed() == 127 && colour.getGreen() == 222 && colour.getBlue() == 224) ||
             (colour.getRed() == 185 && colour.getGreen() == 230 && colour.getBlue() == 231))
    {
        blockShape = Triangle;
    }
    // Visualizer blocks (purple) are hexagons
    else if (name.contains("Spectrum") || name.contains("Waveform") || name.contains("Meter") ||
             (colour.getRed() == 187 && colour.getGreen() == 91 && colour.getBlue() == 187))
    {
        blockShape = Hexagon;
    }
}

bool BlockComponent::isEQBlock() const
{
    // Check if this is an EQ block by name or block shape
    return blockShape == RoundedSquare ||
           name.contains("Highpass") || name.contains("Lowpass") ||
           name.contains("Peaking") || name.contains("Frequency") ||
           name.contains("Q") || name.contains("Gain");
}

bool BlockComponent::isGUIBlock() const
{
    // Check if this is a GUI block by name or block shape
    return blockShape == Circle ||
              name.contains("Button") || name.contains("Slider") ||
                name.contains("Knob");
}

void BlockComponent::paintCircle(juce::Graphics& g)
{
    paintCircle(g, false);
}

void BlockComponent::paintCircle(juce::Graphics& g, bool skipText)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw a filled circle with the component's color
    g.setColour(colour);
    g.fillEllipse(bounds);
    
    // Draw the outline
    g.setColour(juce::Colours::black);
    g.drawEllipse(bounds, 1.5f);
    
    if (!skipText)
    {
        // Draw the text
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(15.0f, juce::Font::bold));
        g.drawText(name, bounds, juce::Justification::centred, true);
    }
}

void BlockComponent::paintRoundedSquare(juce::Graphics& g)
{
    paintRoundedSquare(g, false);
}

void BlockComponent::paintRoundedSquare(juce::Graphics& g, bool skipText)
{
    auto bounds = getLocalBounds().toFloat();
    float cornerRadius = 10.0f;
    
    // Draw a filled rounded rectangle with the component's color
    g.setColour(colour);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Draw the outline
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(bounds, cornerRadius, 1.5f);
    
    if (!skipText)
    {
        // Draw the text
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(15.0f, juce::Font::bold));
        g.drawText(name, bounds, juce::Justification::centred, true);
    }
}

void BlockComponent::paintTriangle(juce::Graphics& g)
{
    paintTriangle(g, false);
}

void BlockComponent::paintTriangle(juce::Graphics& g, bool skipText)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Create a path for the triangle
    juce::Path trianglePath;
    trianglePath.startNewSubPath(bounds.getX() + bounds.getWidth() * 0.5f, bounds.getY());
    trianglePath.lineTo(bounds.getRight(), bounds.getBottom());
    trianglePath.lineTo(bounds.getX(), bounds.getBottom());
    trianglePath.closeSubPath();
    
    // Fill with the component's color
    g.setColour(colour);
    g.fillPath(trianglePath);
    
    // Draw the outline
    g.setColour(juce::Colours::black);
    g.strokePath(trianglePath, juce::PathStrokeType(1.5f));
    
    if (!skipText)
    {
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(15.0f, juce::Font::bold));
        
        float textY = bounds.getY() + bounds.getHeight() * 0.65f; 
        g.drawText(name, bounds.getX(), textY, bounds.getWidth(), bounds.getHeight() * 0.35f,
                  juce::Justification::centred, true);
    }
}

void BlockComponent::paintHexagon(juce::Graphics& g)
{
    paintHexagon(g, false);
}

void BlockComponent::paintHexagon(juce::Graphics& g, bool skipText)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Calculate dimensions of the hexagon
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    float xCenter = bounds.getCentreX();
    float yCenter = bounds.getCentreY();
    
    // Calculate hexagon points based on radius
    float radius = juce::jmin(width, height) * 0.5f;
    
    // Create a path for the hexagon
    juce::Path hexPath;
    for (int i = 0; i < 6; ++i)
    {
        float angle = juce::MathConstants<float>::pi * 2.0f * i / 6.0f;
        float x = xCenter + radius * std::cos(angle);
        float y = yCenter + radius * std::sin(angle);
        
        if (i == 0)
            hexPath.startNewSubPath(x, y);
        else
            hexPath.lineTo(x, y);
    }
    hexPath.closeSubPath();
    
    g.setColour(colour);
    g.fillPath(hexPath);
    
    g.setColour(juce::Colours::black);
    g.strokePath(hexPath, juce::PathStrokeType(1.5f));
    
    if (!skipText)
    {
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(15.0f, juce::Font::bold));
        g.drawText(name, bounds, juce::Justification::centred, true);
    }
}

void BlockComponent::beginConnectorDrag(ConnectorPosition position, const juce::Point<int>& startPoint)
{
    isConnecting_ = true;
    activeConnectorPosition = position;
    currentDragPoint_ = startPoint;
    
    juce::Point<int> connectorCenter = getConnectorCentre(position);
    currentDragPoint_ = connectorCenter;
}

void BlockComponent::adjustGUIBlockConnectors(const juce::Rectangle<int> &bounds, const int connectorSize)
{
    if (name == "Button")
            {
                leftConnector->setBounds(
                    0,
                    bounds.getCentreY() - connectorSize/2,
                    connectorSize,
                    connectorSize
                );
                
                rightConnector->setBounds(
                    bounds.getWidth() - connectorSize,
                    bounds.getCentreY() - connectorSize/2,
                    connectorSize,
                    connectorSize
                );
                
                topConnector->setBounds(
                    bounds.getCentreX() - connectorSize/2,
                    5,
                    connectorSize,
                    connectorSize
                );
                
                bottomConnector->setBounds(
                    bounds.getCentreX() - connectorSize/2,
                    bounds.getHeight() - connectorSize - 5,
                    connectorSize,
                    connectorSize
                );
            }
            else if (name == "Slider")
            {
                leftConnector->setBounds(
                    35,
                    bounds.getCentreY() - connectorSize/2,
                    connectorSize,
                    connectorSize
                );
                
                rightConnector->setBounds(
                    bounds.getWidth() - connectorSize - 35,
                    bounds.getCentreY() - connectorSize/2,
                    connectorSize,
                    connectorSize
                );
                
                topConnector->setBounds(
                    bounds.getCentreX() - connectorSize/2,
                    0,
                    connectorSize,
                    connectorSize
                );
                
                bottomConnector->setBounds(
                    bounds.getCentreX() - connectorSize/2,
                    bounds.getHeight() - connectorSize - 21,
                    connectorSize,
                    connectorSize
                );
            }
            else if (name == "Knob")
            {
                leftConnector->setBounds(
                    20,
                    bounds.getCentreY() - connectorSize/2,
                    connectorSize,
                    connectorSize
                );
                
                rightConnector->setBounds(
                    bounds.getWidth() - connectorSize - 20,
                    bounds.getCentreY() - connectorSize/2,
                    connectorSize,
                    connectorSize
                );
                
                topConnector->setBounds(
                    bounds.getCentreX() - connectorSize/2,
                    0,
                    connectorSize,
                    connectorSize
                );
                
                bottomConnector->setBounds(
                    bounds.getCentreX() - connectorSize/2,
                    bounds.getHeight() - connectorSize,
                    connectorSize,
                    connectorSize
                );
            }
}

void BlockComponent::adjustTriangleConnectors(const juce::Rectangle<int>& bounds, const int connectorSize)
{

    leftConnector->setBounds(
        0,
        bounds.getHeight() - connectorSize,
        connectorSize,
        connectorSize);
    
    rightConnector->setBounds(
        bounds.getWidth() - connectorSize,
        bounds.getHeight() - connectorSize,
        connectorSize,
        connectorSize);
    
    topConnector->setBounds(
        bounds.getCentreX() - connectorSize/2,
        0,
        connectorSize,
        connectorSize);
    
    bottomConnector->setBounds(
        bounds.getCentreX() - connectorSize/2,
        bounds.getHeight() - connectorSize,
        connectorSize,
        connectorSize);
}

void BlockComponent::adjustHexagonConnectors(const juce::Rectangle<int>& bounds, const int connectorSize)
{
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    
    // Get hexagon vertex positions
    float angleLeft = juce::MathConstants<float>::pi;
    float angleRight = 0.0f;
    float angleTop = juce::MathConstants<float>::pi * 1.5f;
    float angleBottom = juce::MathConstants<float>::pi * 0.5f;
    
    float xCenter = bounds.getCentreX();
    float yCenter = bounds.getCentreY();
    
    // Left connector - left vertex
    int leftX = static_cast<int>(xCenter + radius * std::cos(angleLeft)) - connectorSize/2;
    int leftY = static_cast<int>(yCenter + radius * std::sin(angleLeft)) - connectorSize/2;
    leftConnector->setBounds(leftX, leftY, connectorSize, connectorSize);
    
    // Right connector - right vertex
    int rightX = static_cast<int>(xCenter + radius * std::cos(angleRight)) - connectorSize/2;
    int rightY = static_cast<int>(yCenter + radius * std::sin(angleRight)) - connectorSize/2;
    rightConnector->setBounds(rightX, rightY, connectorSize, connectorSize);
    
    // Top connector - top vertex
    int topX = static_cast<int>(xCenter + radius * std::cos(angleTop)) - connectorSize/2;
    int topY = static_cast<int>(yCenter + radius * std::sin(angleTop)) - connectorSize/2;
    topConnector->setBounds(topX, topY, connectorSize, connectorSize);
    
    // Bottom connector - bottom vertex
    int bottomX = static_cast<int>(xCenter + radius * std::cos(angleBottom)) - connectorSize/2;
    int bottomY = static_cast<int>(yCenter + radius * std::sin(angleBottom)) - connectorSize/2;
    bottomConnector->setBounds(bottomX, bottomY, connectorSize, connectorSize);
}

void BlockComponent::adjustEQBlockConnectors(const juce::Rectangle<int>& bounds, const int connectorSize)
{
    leftConnector->setBounds(
        30,
        bounds.getCentreY() - connectorSize/2 - 5,
        connectorSize,
        connectorSize
    );
    
    rightConnector->setBounds(
        bounds.getWidth() - connectorSize - 30,
        bounds.getCentreY() - connectorSize/2 - 5,
        connectorSize,
        connectorSize
    );
    
    topConnector->setBounds(
        bounds.getCentreX() - connectorSize/2,
        0,
        connectorSize,
        connectorSize
    );
    
    bottomConnector->setBounds(
        bounds.getCentreX() - connectorSize/2,
        bounds.getHeight() - connectorSize - 20,
        connectorSize,
        connectorSize
    );
}

bool BlockComponent::isPointOverConnector(const juce::Point<int>& point, ConnectorPosition position) const
{
    juce::Rectangle<int> connectorBounds;
    
    switch (position)
    {
        case Left:   connectorBounds = leftConnector->getBounds(); break;
        case Right:  connectorBounds = rightConnector->getBounds(); break;
        case Top:    connectorBounds = topConnector->getBounds(); break;
        case Bottom: connectorBounds = bottomConnector->getBounds(); break;
    }
    
    // Make the hit area larger than the visible connector
    int margin = 5;
    connectorBounds = connectorBounds.expanded(margin);
    
    // Check if the point is within the expanded bounds
    return connectorBounds.contains(point);
}

BlockComponent::ConnectorPosition BlockComponent::getCompatibleConnectorPosition(ConnectorPosition sourcePosition)
{
    // Determine the compatible connector position on the target block
    switch (sourcePosition)
    {
        case Left:   return Right;
        case Right:  return Left;
        case Top:    return Bottom;
        case Bottom: return Top;
        default:     return Left;
    }
}

bool BlockComponent::checkConnectorHit(BlockComponent* otherBlock, const juce::Point<int>& mousePos, ConnectorPosition& hitPosition)
{
    
    juce::Point<int> posInOtherBlock = mousePos - otherBlock->getPosition();
    
    // Check each connector
    if (otherBlock->isPointOverConnector(posInOtherBlock, Left))
    {
        hitPosition = Left;
        return true;
    }
    else if (otherBlock->isPointOverConnector(posInOtherBlock, Right))
    {
        hitPosition = Right;
        return true;
    }
    else if (otherBlock->isPointOverConnector(posInOtherBlock, Top))
    {
        hitPosition = Top;
        return true;
    }
    else if (otherBlock->isPointOverConnector(posInOtherBlock, Bottom))
    {
        hitPosition = Bottom;
        return true;
    }
    
    return false;
}

void BlockComponent::connectToCanvas(BlockComponent* source, BlockComponent* dest, ConnectorPosition position)
{
    auto* parent = getParentComponent();
    if (parent != nullptr)
    {
        auto* canvas = dynamic_cast<CenterCanvas*>(parent);
        if (canvas != nullptr)
        {
            canvas->connectBlocks(source, dest, position);
        }
    }
}

void BlockComponent::notifyParentOfMovement()
{
    if (auto* parent = getParentComponent())
    {
        if (auto* canvas = dynamic_cast<CenterCanvas*>(parent))
        {
            canvas->blockMoved();
        }
    }
}

juce::Rectangle<int> BlockComponent::getBoundsInParent() const
{
    return getLocalBounds().translated(getX(), getY());
}

void BlockComponent::deleteAllChildren()
{
    removeAllChildren();
    leftConnector = nullptr;
    rightConnector = nullptr;
    topConnector = nullptr;
    bottomConnector = nullptr;
    textEntry = nullptr;
}

void BlockComponent::highlightConnections(bool highlight)
{
    auto* canvas = dynamic_cast<CenterCanvas*>(getParentComponent());
    if (canvas == nullptr)
        return;
    
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

void BlockComponent::setTooltipByBlockType()
{
    // Set appropriate tooltip based on block type/shape
    switch (blockShape)
    {
        case Circle:
            tooltipText = name + ": GUI control element";
            break;
            
        case RoundedSquare:
            if (name.contains("Frequency"))
                tooltipText = "Frequency parameter: Controls the center/cutoff frequency";
            else if (name.contains("Q"))
                tooltipText = "Q parameter: Controls the width/resonance of the filter";
            else if (name.contains("Gain"))
                tooltipText = "Gain parameter: Controls the boost/cut amount";
            else
                tooltipText = name + ": EQ/Filter element";
            break;
            
        case Triangle:
            tooltipText = name + ": Dynamics processor";
            break;
            
        case Hexagon:
            tooltipText = name + ": Visualizer element";
            break;
            
        default:
            tooltipText = name;
            break;
    }
}
