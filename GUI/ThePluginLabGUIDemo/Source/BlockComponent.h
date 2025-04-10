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
        : name(blockName), colour(blockColour)
    {
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
    
    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        // Draw based on shape type
        switch (blockShape)
        {
            case Circle:
                paintCircle(g);
                break;
            case RoundedSquare:
                paintRoundedSquare(g);
                break;
            case Triangle:
                paintTriangle(g);
                break;
            case Hexagon:
                paintHexagon(g);
                break;
        }
        
        // Draw connectors
        g.setColour(juce::Colours::black);
        g.fillEllipse(inputConnector.getBounds().toFloat());
        g.fillEllipse(outputConnector.getBounds().toFloat());
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        if (blockShape == Triangle)
        {
            // Position connectors on the sides of the triangle
            float height = bounds.getHeight() - 10.0f; // Account for padding
            float halfWidth = height * 0.866f / 2.0f; // For equilateral triangle
            float centerX = bounds.getCentreX();
            
            // Calculate positions on the slanted sides
            float leftSideX = 0;
            float rightSideX = bounds.getWidth() - 10;
            
            // Position at midpoint of the slanted sides
            float connectorY = bounds.getY() + height * 0.5f - 5;
            
            inputConnector.setBounds(leftSideX, (int)connectorY, 10, 10);
            outputConnector.setBounds(rightSideX, (int)connectorY, 10, 10);
        }
        else if (blockShape == Hexagon)
        {
            // For hexagon shape, position at the middle of the left and right sides
            int verticalCenter = bounds.getCentreY() - 5;
            inputConnector.setBounds(0, verticalCenter, 10, 10);
            outputConnector.setBounds(bounds.getWidth() - 10, verticalCenter, 10, 10);
        }
        else
        {
            // For other shapes (circle, rounded square), standard centered connectors
            inputConnector.setBounds(0, bounds.getCentreY() - 5, 10, 10);
            outputConnector.setBounds(bounds.getWidth() - 10, bounds.getCentreY() - 5, 10, 10);
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

private:
    //==============================================================================
    juce::String name;
    juce::Colour colour;
    
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
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        
        // Fill background with block's color
        g.setColour(colour);
        g.fillEllipse(bounds);
        
        // Add border
        g.setColour(juce::Colours::white);
        g.drawEllipse(bounds, 1.0f);
        
        // Draw the block name
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText(name, getLocalBounds().reduced(5), juce::Justification::centred, true);
    }
    
    void paintRoundedSquare(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        
        // Fill background with block's color
        g.setColour(colour);
        g.fillRoundedRectangle(bounds, 8.0f);
        
        // Add border
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds, 8.0f, 1.0f);
        
        // Draw the block name
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText(name, getLocalBounds().reduced(5), juce::Justification::centred, true);
    }
    
    void paintTriangle(juce::Graphics& g)
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
        g.setColour(juce::Colours::white);
        g.setFont(12.0f);
        g.drawText(name, getLocalBounds().reduced(5).withTrimmedTop(getHeight() / 3), 
                   juce::Justification::centred, true);
                   
        // Draw debug markers for connector positions - uncomment for debugging
        // g.setColour(juce::Colours::red);
        // g.fillEllipse(leftX - 3, leftY/2 + top/2 - 3, 6, 6); // Left side middle
        // g.fillEllipse(rightX - 3, rightY/2 + top/2 - 3, 6, 6); // Right side middle
    }
    
    void paintHexagon(juce::Graphics& g)
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
        g.setColour(juce::Colours::white);
        g.setFont(12.0f);
        g.drawText(name, getLocalBounds().reduced(10), juce::Justification::centred, true);
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
    
    // Helper method to notify parent when block is moved
    void notifyParentOfMovement();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlockComponent)
};
