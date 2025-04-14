#include "SpectrumVisualizerBlock.h"

SpectrumVisualizerBlock::SpectrumVisualizerBlock(const juce::String& name, juce::Colour colour)
    : BlockComponent(name, colour)
{
    // Create and add the visualizer component
    addAndMakeVisible(visualizer);
    
    // Set a minimum size for the visualizer block to accommodate the spectrum display
    setSize(160, 120);
}

void SpectrumVisualizerBlock::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    
    // Use a darker background color for the spectrum visualizer block
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    
    // Create hexagon path (copied from paintHexagon method)
    juce::Path hexagon;
    float w = bounds.getWidth();
    float h = bounds.getHeight();
    float x = bounds.getX();
    float y = bounds.getY();
    
    hexagon.startNewSubPath(x + w * 0.25f, y);
    hexagon.lineTo(x + w * 0.75f, y);
    hexagon.lineTo(x + w, y + h * 0.5f);
    hexagon.lineTo(x + w * 0.75f, y + h);
    hexagon.lineTo(x + w * 0.25f, y + h);
    hexagon.lineTo(x, y + h * 0.5f);
    hexagon.closeSubPath();
    
    // Fill the hexagon with a dark background
    g.fillPath(hexagon);
    
    // Draw a purple border around the hexagon
    g.setColour(juce::Colours::purple.withAlpha(0.8f)); // Use purple color for the outline
    g.strokePath(hexagon, juce::PathStrokeType(2.0f));
    
    // Draw the block name at the top
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText(getName(), getLocalBounds().reduced(10).withHeight(20), juce::Justification::centred, true);
    
    // The visualizer component will draw itself since it's a child component
    
    // Draw connectors with white circles and black outlines
    for (const auto& connector : { getChildComponent(0), getChildComponent(1), 
                                  getChildComponent(2), getChildComponent(3) })
    {
        if (connector != nullptr)
        {
            // Draw white fill
            g.setColour(juce::Colours::white);
            g.fillEllipse(connector->getBounds().toFloat());
            
            // Draw black outline
            g.setColour(juce::Colours::black);
            g.drawEllipse(connector->getBounds().toFloat(), 1.0f);
        }
    }
}

void SpectrumVisualizerBlock::resized()
{
    BlockComponent::resized();
    
    // Make the visualizer fill most of the block, but leave some space at the top for the name
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(20); // Space for the block name
    
    // Create a hexagon-shaped area for the visualizer
    auto hexBounds = createHexagonalBounds(bounds);
    visualizer.setBounds(hexBounds);
}

juce::Rectangle<int> SpectrumVisualizerBlock::createHexagonalBounds(const juce::Rectangle<int>& rect)
{
    // Create inset bounds to fit a hexagon within the rectangle
    // A hexagon inscribed in a rectangle needs some horizontal inset
    int insetX = rect.getHeight() / 4; // Approximately right for a regular hexagon
    
    return rect.withTrimmedLeft(insetX).withTrimmedRight(insetX);
}

void SpectrumVisualizerBlock::updateParametersFromConnections(const juce::Array<CenterCanvasTypes::BlockConnection>& connections)
{
    // Reset parameters if no connections
    visualizer.clearFilterBands();
    
    // Track if we've found each parameter
    bool foundFreq = false;
    bool foundQ = false;
    bool foundGain = false;
    float freqValue = 1000.0f; // Default values
    float qValue = 1.0f;
    float gainValue = 0.0f;
    
    // Look for blocks connected to this visualizer
    for (const auto& connection : connections)
    {
        // If this visualizer is the destination block
        if (connection.destBlock == this)
        {
            auto* sourceBlock = connection.sourceBlock;
            if (sourceBlock != nullptr)
            {
                juce::String blockName = sourceBlock->getName();
                
                // Check if the connected block is a parameter block
                if (blockName == "Frequency")
                {
                    auto* editor = findTextEditor(sourceBlock);
                    if (editor != nullptr)
                    {
                        freqValue = extractNumberFromString(editor->getText());
                        foundFreq = true;
                    }
                }
                else if (blockName == "Q")
                {
                    auto* editor = findTextEditor(sourceBlock);
                    if (editor != nullptr)
                    {
                        qValue = extractNumberFromString(editor->getText());
                        foundQ = true;
                    }
                }
                else if (blockName == "Gain")
                {
                    auto* editor = findTextEditor(sourceBlock);
                    if (editor != nullptr)
                    {
                        gainValue = extractNumberFromString(editor->getText());
                        foundGain = true;
                    }
                }
            }
        }
    }
    
    // If we found any parameters, update the visualizer
    if (foundFreq || foundQ || foundGain)
    {
        visualizer.addFilterBand(freqValue, qValue, gainValue);
        
        // Optional debug logging
        juce::Logger::writeToLog("Updating spectrum with: Freq=" + juce::String(freqValue) +
                              " Q=" + juce::String(qValue) +
                              " Gain=" + juce::String(gainValue));
    }
}

float SpectrumVisualizerBlock::extractNumberFromString(const juce::String& text)
{
    // Extract numerical value from text that may contain units like "Hz" or "dB"
    juce::String numericPart = text.trimCharactersAtEnd("HzhZDbdB ");
    return numericPart.getFloatValue();
}

juce::TextEditor* SpectrumVisualizerBlock::findTextEditor(BlockComponent* block)
{
    if (block != nullptr && block->getParentComponent() != nullptr)
    {
        // Look for a TextEditor among the parent's children
        // This is a workaround because the text editor might be a direct child of the canvas
        auto* parent = block->getParentComponent();
        
        for (int i = 0; i < parent->getNumChildComponents(); ++i)
        {
            auto* child = parent->getChildComponent(i);
            auto* editor = dynamic_cast<juce::TextEditor*>(child);
            
            if (editor != nullptr)
            {
                // Check if this editor corresponds to the block
                // by checking its position relative to the block
                juce::Rectangle<int> editorBounds = editor->getBounds();
                juce::Rectangle<int> blockBounds = block->getBounds();
                
                // If the editor is positioned below the block, it likely belongs to it
                if (std::abs(editorBounds.getX() - blockBounds.getX()) < 20 &&
                    std::abs(editorBounds.getRight() - blockBounds.getRight()) < 20 &&
                    editorBounds.getY() > blockBounds.getBottom() - 5 &&
                    editorBounds.getY() < blockBounds.getBottom() + 30)
                {
                    return editor;
                }
            }
        }
    }
    
    return nullptr;
}

void SpectrumVisualizerBlock::drawHexagonBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    float w = bounds.getWidth();
    float h = bounds.getHeight();
    float x = bounds.getX();
    float y = bounds.getY();
    
    juce::Path hexagon;
    hexagon.startNewSubPath(x + w * 0.25f, y);
    hexagon.lineTo(x + w * 0.75f, y);
    hexagon.lineTo(x + w, y + h * 0.5f);
    hexagon.lineTo(x + w * 0.75f, y + h);
    hexagon.lineTo(x + w * 0.25f, y + h);
    hexagon.lineTo(x, y + h * 0.5f);
    hexagon.closeSubPath();
    
    g.strokePath(hexagon, juce::PathStrokeType(2.0f));
}
