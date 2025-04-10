#pragma once

#include <JuceHeader.h>

// Forward declaration
class Block;

class RightSidebar : public juce::Component
{
public:
    //==============================================================================
    RightSidebar();
    ~RightSidebar() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // Update the inspector panel with the selected block's parameters
    void updateForBlock(const Block& selectedBlock);
    
    // Clear the inspector panel
    void clearBlock();
    
    // Show live preview of the selected block
    void showLivePreview(const Block& selectedBlock);
    
private:
    //==============================================================================
    // Add any private member variables and methods here
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RightSidebar)
};