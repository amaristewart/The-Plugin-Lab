#pragma once

#include <JuceHeader.h>
#include "BlockComponent.h"
#include "SpectrumVisualizer.h"

// Forward declaration for CenterCanvasTypes
namespace CenterCanvasTypes {
    struct BlockConnection;
}

/**
 * A specialized BlockComponent that serves as an EQ spectrum visualizer
 * and can receive parameters from connected blocks.
 */
class SpectrumVisualizerBlock : public BlockComponent
{
public:
    SpectrumVisualizerBlock(const juce::String& name, juce::Colour colour);
    ~SpectrumVisualizerBlock() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Method to update parameters based on connected blocks
    void updateParametersFromConnections(const juce::Array<CenterCanvasTypes::BlockConnection>& connections);
    
    // Override this method to ensure we keep our custom behavior
    bool isSpectrumVisualizer() const override { return true; }
    
private:
    SpectrumVisualizer visualizer;
    
    // Helper to extract number from text like "1000 Hz" or "0.5 dB"
    float extractNumberFromString(const juce::String& text);
    
    // Helper to find the TextEditor in a block
    juce::TextEditor* findTextEditor(BlockComponent* block);
    
    // Helper method to draw the hexagon border for consistency
    void drawHexagonBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    
    // Helper method to calculate a hexagonal bounds within a rectangle
    juce::Rectangle<int> createHexagonalBounds(const juce::Rectangle<int>& rect);
};
