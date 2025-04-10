#include "RightSidebar.h"
// Add the include for Block definition
#include "Block.h"

//==============================================================================
// Constructor
RightSidebar::RightSidebar()
{
    setSize(200, 400); // Set the size of the sidebar
    // Additional initialization code can go here
}

// Destructor
RightSidebar::~RightSidebar()
{
}

//==============================================================================
// Paint method
void RightSidebar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey); // Fill background with dark grey color

    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Inspector Panel", getLocalBounds().reduced(10), juce::Justification::topLeft, true);
}

//==============================================================================
// Resized method
void RightSidebar::resized()
{
    // Layout adjustments for child components can be made here
}

// Method to update the inspector panel based on selected block
void RightSidebar::updateForBlock(const Block& selectedBlock)
{
    // Code to update the inspector panel with parameters of the selected block
}

// Method to clear the inspector panel
void RightSidebar::clearBlock()
{
    // Code to clear the inspector panel when nothing is selected
}

// Method to show live preview of the selected block
void RightSidebar::showLivePreview(const Block& selectedBlock)
{
    // Code to display a live preview of the selected block's output
}