#pragma once

#include <JuceHeader.h>
#include "CustomShapeButton.h"

// Forward declarations
class BlockComponent;

//==============================================================================
class LeftSidebar : public juce::Component,
                    public juce::ChangeListener // Add ChangeListener for tab changes
{
public:
    //==============================================================================
    LeftSidebar();
    ~LeftSidebar() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // Add a block to the sidebar
    void addBlock(const juce::String& name, juce::Colour colour);
    
    // Override the changeListenerCallback to handle tab changes
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    //==============================================================================
    juce::Viewport viewport;
    juce::Component blockListComponent;
    juce::OwnedArray<BlockComponent> blockComponents;
    
    const int blockHeight = 60;

    juce::TextButton codeTab { "Code" };
    juce::TextButton customizationTab { "Customization" };

    // Overarching tabs
    juce::TabbedButtonBar overarchingTabs { juce::TabbedButtonBar::Orientation::TabsAtTop };

    // Category buttons
    CircleButton guiTab { "GUI" };
    RoundedSquareButton eqTab { "EQ" };
    TriangleButton dynamicsTab { "Dynamics" };
    HexagonButton visualizerTab { "Visualizer" };

    juce::ComboBox categorySelector;

    void updateBlocksForCategory(const juce::String& category);
    void updateCategoryButtonsVisibility(); // New method to handle tab changes
    void handleCategoryButtonClicked(juce::Button* clickedButton, const juce::String& category);
    void addBlockToCategory(const juce::String& name, juce::Colour colour); // New helper method

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeftSidebar)
};