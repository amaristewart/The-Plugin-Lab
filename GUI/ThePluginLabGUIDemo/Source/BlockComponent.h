#pragma once

#include <JuceHeader.h>
#include "CustomShapeButton.h" // Include for shape drawing

// Forward declaration for CenterCanvas (will be fully defined in cpp file)
class CenterCanvas;

//==============================================================================
class BlockComponent : public juce::Component
{
public:
    enum BlockShape {
        Circle,         // For GUI elements
        RoundedSquare,  // For EQ elements
        Triangle,       // For Dynamics elements
        Hexagon         // For Visualizer elements
    };
    
    //==============================================================================
    BlockComponent(const juce::String& blockName, juce::Colour blockColour)
    {
        name = blockName;
        colour = blockColour;
        
        // Enable the component to be dragged
        setMouseCursor(juce::MouseCursor::DraggingHandCursor);
        
        // Add input/output connectors for connection between blocks
        addAndMakeVisible(inputConnector);
        addAndMakeVisible(outputConnector);
        
        // Set the size
        setSize(120, 60);
        
        // Determine shape based on name or color
        determineBlockShape();
    }

    // Add constructor that accepts an image
    BlockComponent(const juce::String& blockName, juce::Colour blockColour, const juce::Image& blockImage)
    {
        name = blockName;
        colour = blockColour;
        image = blockImage;
        
        // Enable the component to be dragged
        setMouseCursor(juce::MouseCursor::DraggingHandCursor);
        
        // Add input/output connectors for connection between blocks
        addAndMakeVisible(inputConnector);
        addAndMakeVisible(outputConnector);
        
        // Set the size
        setSize(120, 60);
        
        // Determine shape based on name or color
        determineBlockShape();
        
        // If we have an image, use it
        hasImage = image.isValid();
    }
    
    // Add method to create and show text entry box when placed on canvas
    void addTextEntryIfNeeded()
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
                
            // Configure appearance with stronger colors for visibility
            textEntry->setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
            textEntry->setColour(juce::TextEditor::outlineColourId, juce::Colours::black);
            textEntry->setColour(juce::TextEditor::textColourId, juce::Colours::black);
            textEntry->setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::blue);
            
            // Make sure text is visibly black by explicitly setting the text color
            textEntry->applyColourToAllText(juce::Colours::black, true);
            
            // Use a bold font to make text more readable
            textEntry->setFont(juce::Font(14.0f, juce::Font::bold));
            textEntry->setJustification(juce::Justification::centred);
            
            // Add to parent directly for better visibility
            if (auto* parent = getParentComponent())
            {
                parent->addAndMakeVisible(textEntry.get());
                parent->repaint();
            }
            else
            {
                addAndMakeVisible(textEntry.get());
            }
            
            // Position the text entry correctly
            positionTextEntry();
        }
    }
    
    void positionTextEntry()
    {
        if (textEntry != nullptr)
        {
            const int textHeight = 25;
            const int margin = 5;
            
            // Position completely outside the block bounds
            int xPos = getX() + margin;
            int yPos = getY() + getHeight() + 5; // 5px gap between block and text box
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
            
            // Make sure the text entry is at the front
            textEntry->toFront(false);
        }
    }
    
    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        // If we have an image, use it without drawing a background shape
        if (hasImage)
        {
            // Draw only the image, filling more of the available space
            g.drawImageWithin(image, 
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
        
        // Draw connectors
        g.setColour(juce::Colours::black);
        g.fillEllipse(inputConnector.getBounds().toFloat());
        g.fillEllipse(outputConnector.getBounds().toFloat());
        
        // Draw text entry with more prominent appearance
        if (textEntry != nullptr && textEntry->isVisible())
        {
            // Draw text entry with more visible background
            auto textBounds = textEntry->getBounds();
            
            // First draw a more contrasting background to make the text entry more visible
            g.setColour(juce::Colours::white);
            g.fillRoundedRectangle(textBounds.toFloat().expanded(2), 4.0f);
            
            // Then add a darker border for contrast
            g.setColour(juce::Colours::black);
            g.drawRoundedRectangle(textBounds.toFloat().expanded(2), 4.0f, 1.5f);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Position the text entry at the bottom if it exists
        positionTextEntry();
        
        // Create a modified bounds for the main content that excludes the text entry area
        auto mainBounds = bounds;
        if (textEntry != nullptr)
        {
            mainBounds = bounds.withHeight(bounds.getHeight() - 30);
        }
        
        const int connectorSize = 10;
        
        if (hasImage)
        {
            // For image blocks, customize connector positions based on block type
            if (name == "Button")
            {
                // Button: position connectors more outward and slightly down
                inputConnector.setBounds(
                    0, 
                    mainBounds.getCentreY() - 2,
                    connectorSize,
                    connectorSize
                );
                
                outputConnector.setBounds(
                    mainBounds.getWidth() - connectorSize, 
                    mainBounds.getCentreY() - 2,
                    connectorSize,
                    connectorSize
                );
            }
            else if (name == "Slider")
            {
                // Slider: position connectors more inward to align with the slider track
                const int inset = 40; // Increased inset to move connectors inward
                
                inputConnector.setBounds(
                    inset, 
                    mainBounds.getCentreY() - 5,
                    connectorSize,
                    connectorSize
                );
                
                outputConnector.setBounds(
                    mainBounds.getWidth() - connectorSize - inset, 
                    mainBounds.getCentreY() - 5,
                    connectorSize,
                    connectorSize
                );
            }
            else if (name == "Knob")
            {
                const int inset = 22;
                const int verticalOffset = -2;
                
                inputConnector.setBounds(
                    inset, 
                    mainBounds.getCentreY() + verticalOffset, 
                    connectorSize, 
                    connectorSize
                );
                
                outputConnector.setBounds(
                    mainBounds.getWidth() - connectorSize - inset, 
                    mainBounds.getCentreY() + verticalOffset, 
                    connectorSize, 
                    connectorSize
                );
            }
            else
            {
                // Default positioning for other image blocks
                const int inset = 27;
                const int verticalOffset = -7;
                
                inputConnector.setBounds(
                    inset, 
                    mainBounds.getCentreY() - connectorSize/2 + verticalOffset, 
                    connectorSize, 
                    connectorSize
                );
                
                outputConnector.setBounds(
                    mainBounds.getWidth() - connectorSize - inset, 
                    mainBounds.getCentreY() - connectorSize/2 + verticalOffset, 
                    connectorSize, 
                    connectorSize
                );
            }
        }
        else if (blockShape == Triangle)
        {
            // Position connectors on the sides of the triangle
            float height = mainBounds.getHeight() - 10.0f; // Account for padding
            float halfWidth = height * 0.866f / 2.0f; // For equilateral triangle
            float centerX = mainBounds.getCentreX();
            
            // Calculate positions on the slanted sides
            float leftSideX = 0;
            float rightSideX = mainBounds.getWidth() - 10;
            
            // Position at midpoint of the slanted sides
            float connectorY = mainBounds.getY() + height * 0.5f - 5;
            
            inputConnector.setBounds(leftSideX, (int)connectorY, 10, 10);
            outputConnector.setBounds(rightSideX, (int)connectorY, 10, 10);
        }
        else if (blockShape == Hexagon)
        {
            // For hexagon shape, position at the middle of the left and right sides
            int verticalCenter = mainBounds.getCentreY() - 5;
            inputConnector.setBounds(0, verticalCenter, 10, 10);
            outputConnector.setBounds(mainBounds.getWidth() - 10, verticalCenter, 10, 10);
        }
        else
        {
            // For other shapes (circle, rounded square), standard centered connectors
            inputConnector.setBounds(0, mainBounds.getCentreY() - 5, 10, 10);
            outputConnector.setBounds(mainBounds.getWidth() - 10, mainBounds.getCentreY() - 5, 10, 10);
        }
    }
    
    // Override Component::moved but declare without implementation (will be in cpp file)
    void moved() override;
    
    //==============================================================================
    // Override Component::mouseDown (implemented in cpp file)
    void mouseDown(const juce::MouseEvent& e) override;
    
    // Add a specific method to highlight connections involving this block
    void highlightConnections(bool highlight);
    
    void mouseDrag(const juce::MouseEvent& e) override
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
            
            // Just trigger a repaint for now - connection update happens in moved()
            getParentComponent()->repaint();
        }
    }
    
    void mouseUp(const juce::MouseEvent& e) override
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
                        // If we're dragging from output, look for an input connector
                        if (isOutputConnecting_)
                        {
                            juce::Point<int> siblingInputPos = sibling->getPosition() + sibling->getInputConnectorCentre();
                            // Use a bigger hit area for easier connections
                            juce::Rectangle<int> hitArea(siblingInputPos.x - 20, siblingInputPos.y - 20, 40, 40);
                            
                            if (hitArea.contains(mousePos))
                            {
                                // Try to connect blocks via parent canvas
                                connectToCanvas(this, sibling);
                                // Visual feedback
                                sibling->inputConnector.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                                sibling->repaint();
                                break;
                            }
                        }
                        // If we're dragging from input, look for an output connector
                        else
                        {
                            juce::Point<int> siblingOutputPos = sibling->getPosition() + sibling->getOutputConnectorCentre();
                            // Use a bigger hit area for easier connections
                            juce::Rectangle<int> hitArea(siblingOutputPos.x - 20, siblingOutputPos.y - 20, 40, 40);
                            
                            if (hitArea.contains(mousePos))
                            {
                                // Try to connect blocks via parent canvas
                                connectToCanvas(sibling, this);
                                // Visual feedback
                                sibling->outputConnector.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                                sibling->repaint();
                                break;
                            }
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
    
    // Helper to determine if this block is in the sidebar
    bool isInSidebar() const
    {
        return findParentComponentOfClass<juce::Viewport>() != nullptr;
    }
    
    const juce::String& getName() const { return name; }
    juce::Colour getColour() const { return colour; }
    
    // Methods to get connector positions
    juce::Point<int> getInputConnectorCentre() const
    {
        return inputConnector.getBounds().getCentre();
    }

    juce::Point<int> getOutputConnectorCentre() const
    {
        return outputConnector.getBounds().getCentre();
    }

    // Add methods to access internal state for connection visualization
    bool isConnecting() const { return isConnecting_; }
    bool isOutputConnectorBeingDragged() const { return isOutputConnecting_; }
    juce::Point<int> getCurrentDragPoint() const { return currentDragPoint_; }
    
    // Show a popup menu for managing connections and deleting the block
    void showConnectionMenu();

    // Add accessors for image-related properties
    bool hasImage = false;
    const juce::Image& getImage() const { return image; }
    
    // Add a public method to clean up resources before deletion
    void prepareForDeletion()
    {
        // Clear the text entry pointer before the component gets deleted
        if (textEntry != nullptr && textEntry->getParentComponent() != this)
        {
            textEntry->getParentComponent()->removeChildComponent(textEntry.get());
        }
        textEntry = nullptr;
    }

private:
    //==============================================================================
    juce::String name;
    juce::Colour colour;
    juce::Image image;
    
    // Connector components for input/output
    juce::Component inputConnector;
    juce::Component outputConnector;
    
    // Component dragger for moving blocks on the canvas
    juce::ComponentDragger dragger;
    
    // Connector dragging state - renamed with trailing underscore to avoid name collision
    bool isConnecting_ = false;
    bool isOutputConnecting_ = false;
    juce::Point<int> currentDragPoint_;
    
    BlockShape blockShape = RoundedSquare; // Default shape
    
    // Text entry box for parameter values
    std::unique_ptr<juce::TextEditor> textEntry;
    
    // Determine the shape based on the block name or color
    void determineBlockShape()
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
    
    // Paint methods for different shapes
    void paintCircle(juce::Graphics& g)
    {
        paintCircle(g, false);
    }

    void paintCircle(juce::Graphics& g, bool skipText)
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        
        // Fill background with block's color
        g.setColour(colour);
        g.fillEllipse(bounds);
        
        // Add border
        g.setColour(juce::Colours::white);
        g.drawEllipse(bounds, 1.0f);
        
        // Draw the block name if not skipped
        if (!skipText)
        {
            g.setColour(juce::Colours::white);
            g.setFont(14.0f);
            g.drawText(name, getLocalBounds().reduced(5), juce::Justification::centred, true);
        }
    }
    
    void paintRoundedSquare(juce::Graphics& g)
    {
        paintRoundedSquare(g, false);
    }

    void paintRoundedSquare(juce::Graphics& g, bool skipText)
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        
        // Fill background with block's color
        g.setColour(colour);
        g.fillRoundedRectangle(bounds, 8.0f);
        
        // Add border
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds, 8.0f, 1.0f);
        
        // Draw the block name if not skipped
        if (!skipText)
        {
            g.setColour(juce::Colours::white);
            g.setFont(14.0f);
            g.drawText(name, getLocalBounds().reduced(5), juce::Justification::centred, true);
        }
    }
    
    void paintTriangle(juce::Graphics& g)
    {
        paintTriangle(g, false);
    }

    void paintTriangle(juce::Graphics& g, bool skipText)
    {
        auto bounds = getLocalBounds().toFloat().reduced(5.0f);
        
        // Make an equilateral triangle - base width equal to height * sqrt(3)
        // For equilateral triangle, we need to adjust the x positions
        float height = bounds.getHeight();
        float centerX = bounds.getCentreX();
        float top = bounds.getY();
        float bottom = bounds.getBottom();
        
        // Calculate width of an equilateral triangle based on height
        float halfWidth = height * 0.866f / 2.0f; // sqrt(3)/2 ≈ 0.866
        
        // Define triangle points
        float topX = centerX;
        float topY = top;
        float leftX = centerX - halfWidth;
        float leftY = bottom;
        float rightX = centerX + halfWidth;
        float rightY = bottom;
        
        // Create an equilateral triangle path
        juce::Path triangle;
        triangle.addTriangle(leftX, leftY, topX, topY, rightX, rightY);
        
        // Fill with block's color
        g.setColour(colour);
        g.fillPath(triangle);
        
        // Add border
        g.setColour(juce::Colours::white);
        g.strokePath(triangle, juce::PathStrokeType(1.0f));
        
        // Draw the block name - position lower to fit within triangle
        if (!skipText)
        {
            g.setColour(juce::Colours::white);
            g.setFont(12.0f);
            g.drawText(name, getLocalBounds().reduced(5).withTrimmedTop(getHeight() / 3), 
                    juce::Justification::centred, true);
        }
    }
    
    void paintHexagon(juce::Graphics& g)
    {
        paintHexagon(g, false);
    }

    void paintHexagon(juce::Graphics& g, bool skipText)
    {
        auto bounds = getLocalBounds().toFloat().reduced(5.0f);
        float w = bounds.getWidth();
        float h = bounds.getHeight();
        float x = bounds.getX();
        float y = bounds.getY();
        
        // Create hexagon path
        juce::Path hexagon;
        hexagon.startNewSubPath(x + w * 0.25f, y);
        hexagon.lineTo(x + w * 0.75f, y);
        hexagon.lineTo(x + w, y + h * 0.5f);
        hexagon.lineTo(x + w * 0.75f, y + h);
        hexagon.lineTo(x + w * 0.25f, y + h);
        hexagon.lineTo(x, y + h * 0.5f);
        hexagon.closeSubPath();
        
        // Fill with block's color
        g.setColour(colour);
        g.fillPath(hexagon);
        
        // Add border
        g.setColour(juce::Colours::white);
        g.strokePath(hexagon, juce::PathStrokeType(1.0f));
        
        // Draw the block name
        if (!skipText)
        {
            g.setColour(juce::Colours::white);
            g.setFont(12.0f);
            g.drawText(name, getLocalBounds().reduced(10), juce::Justification::centred, true);
        }
    }

    // Start a connector drag operation
    void beginConnectorDrag(bool fromOutput, const juce::Point<int>& startPoint)
    {
        isConnecting_ = true;
        isOutputConnecting_ = fromOutput;
        currentDragPoint_ = startPoint;
    }

    // Helper method to try connecting blocks through the parent canvas
    void connectToCanvas(BlockComponent* source, BlockComponent* dest);
    
    // Helper method to notify parent of movement
    void notifyParentOfMovement();

    juce::Rectangle<int> getBoundsInParent() const
    {
        auto bounds = juce::Component::getBoundsInParent();
        
        // If we have a text entry, extend the bounds to include its area
        if (textEntry != nullptr && textEntry->isVisible())
        {
            auto textBounds = textEntry->getBoundsInParent().translated(getX(), getY());
            bounds = bounds.getUnion(textBounds);
        }
        
        return bounds;
    }

    // Remove the override keyword since deleteAllChildren is not virtual in Component
    void deleteAllChildren()
    {
        // Clear the text entry pointer before the component gets deleted
        if (textEntry != nullptr && textEntry->getParentComponent() != this)
        {
            textEntry->getParentComponent()->removeChildComponent(textEntry.get());
        }
        textEntry = nullptr;
        
        juce::Component::deleteAllChildren();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlockComponent)
};
