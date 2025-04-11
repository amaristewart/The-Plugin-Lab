#pragma once

#include <JuceHeader.h>

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

    // Overarching tabs
    juce::TabbedButtonBar overarchingTabs { juce::TabbedButtonBar::Orientation::TabsAtTop };

    // Category buttons - now using ImageButtons instead of custom shape buttons
    std::unique_ptr<juce::ImageButton> guiButton;
    std::unique_ptr<juce::ImageButton> eqButton;
    std::unique_ptr<juce::ImageButton> dynamicsButton;
    std::unique_ptr<juce::ImageButton> visualizerButton;

    juce::ComboBox categorySelector;

    void updateBlocksForCategory(const juce::String& category);
    void updateCategoryButtonsVisibility();
    void handleCategoryButtonClicked(juce::Button* clickedButton, const juce::String& category);
    void addBlockToCategory(const juce::String& name, juce::Colour colour);
    
    // Helper method to create placeholder icons when binary resources aren't available
    juce::Image createPlaceholderIcon(const juce::String& label, juce::Colour colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeftSidebar)
};