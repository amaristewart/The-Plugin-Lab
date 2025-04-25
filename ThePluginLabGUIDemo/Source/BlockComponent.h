#pragma once
#include <JuceHeader.h>
#include "CustomShapeButton.h"

// Helper class for connectors with tooltips
class ConnectorComponent : public juce::Component
{
public:
    ConnectorComponent(const juce::String& tooltipText);
    juce::String getTooltip();

private:
    juce::String connectorTooltip;
};

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
    
    // Enum to identify connector positions
    enum ConnectorPosition {
        Left,
        Right,
        Top,
        Bottom
    };
    
    //==============================================================================
    BlockComponent(const juce::String& blockName, juce::Colour blockColour);
    BlockComponent(const juce::String& blockName, juce::Colour blockColour, const juce::Image& blockImage);
    
    void addTextEntryIfNeeded();
    void positionTextEntry();
    
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    void moved() override;
    
    //==============================================================================
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    
    void highlightConnections(bool highlight);
    
    bool isInSidebar() const;
    
    const juce::String& getName() const { return name; }
    juce::Colour getColour() const { return colour; }
    
    juce::Point<int> getConnectorCentre(ConnectorPosition position) const;
    
    juce::Point<int> getInputConnectorCentre() const;
    juce::Point<int> getOutputConnectorCentre() const;

    // Methods to access internal state for connection visualization
    bool isConnecting() const;
    ConnectorPosition getActiveConnectorPosition() const;
    juce::Point<int> getCurrentDragPoint() const;
    
    // Helper methods to check which connector is being dragged
    bool isOutputConnectorBeingDragged() const;
    bool isInputConnectorBeingDragged() const;
    bool isTopConnectorBeingDragged() const;
    bool isBottomConnectorBeingDragged() const;

    // Popup menu for managing connections and deleting the block
    void showConnectionMenu();

    // Method to clean up resources before deletion
    void prepareForDeletion();

    virtual bool isSpectrumVisualizer() const;

    juce::String getTooltip();

    // Add accessors for image-related properties
    bool hasImage = false;
    const juce::Image& getImage() const { return image; }

private:
    //==============================================================================
    juce::String name;
    juce::Colour colour;
    juce::Image image;
    
    std::unique_ptr<ConnectorComponent> leftConnector;
    std::unique_ptr<ConnectorComponent> rightConnector;
    std::unique_ptr<ConnectorComponent> topConnector;
    std::unique_ptr<ConnectorComponent> bottomConnector;
    
    juce::ComponentDragger dragger;
    
    bool isConnecting_ = false;
    ConnectorPosition activeConnectorPosition = Left;
    juce::Point<int> currentDragPoint_;
    
    BlockShape blockShape = RoundedSquare; // Default shape
    
    // Text entry box for parameter values
    std::unique_ptr<juce::TextEditor> textEntry;
    
    juce::String tooltipText;

    void determineBlockShape();

    bool isEQBlock() const;
    bool isGUIBlock() const;
    
    // Paint methods for different shapes
    void paintCircle(juce::Graphics& g);
    void paintCircle(juce::Graphics& g, bool skipText);
    void paintRoundedSquare(juce::Graphics& g);
    void paintRoundedSquare(juce::Graphics& g, bool skipText);
    void paintTriangle(juce::Graphics& g);
    void paintTriangle(juce::Graphics& g, bool skipText);
    void paintHexagon(juce::Graphics& g);
    void paintHexagon(juce::Graphics& g, bool skipText);

    void beginConnectorDrag(ConnectorPosition position, const juce::Point<int>& startPoint);

    // Helper methods for connector positioning
    void adjustGUIBlockConnectors(const juce::Rectangle<int>& bounds, const int connectorSize);
    void adjustTriangleConnectors(const juce::Rectangle<int>& bounds, const int connectorSize);
    void adjustHexagonConnectors(const juce::Rectangle<int>& bounds, const int connectorSize);
    void adjustEQBlockConnectors(const juce::Rectangle<int>& bounds, const int connectorSize);
    
    bool isPointOverConnector(const juce::Point<int>& point, ConnectorPosition position) const;
    
    bool checkConnectorHit(BlockComponent* otherBlock, const juce::Point<int>& mousePos, ConnectorPosition& hitPosition);

    ConnectorPosition getCompatibleConnectorPosition(ConnectorPosition sourcePosition);

    // Helper method to try connecting blocks through the parent canvas
    void connectToCanvas(BlockComponent* source, BlockComponent* dest, ConnectorPosition position);
    
    void notifyParentOfMovement();

    juce::Rectangle<int> getBoundsInParent() const;

    void deleteAllChildren();

    void setTooltipByBlockType();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlockComponent)
};
