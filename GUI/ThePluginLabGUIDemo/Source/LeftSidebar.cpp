#include "LeftSidebar.h"
#include "BlockComponent.h"

//==============================================================================
LeftSidebar::LeftSidebar()
{
    // Add overarching tabs (smaller size)
    overarchingTabs.addTab("Code", juce::Colours::white, -1);
    overarchingTabs.addTab("Customization", juce::Colours::white, -1);
    addAndMakeVisible(overarchingTabs);
    
    // Set the selected tab to "Code" by default
    overarchingTabs.setCurrentTabIndex(0);
    
    // Add listener for tab changes
    overarchingTabs.addChangeListener(this);

    // Add category buttons
    addAndMakeVisible(guiTab);
    addAndMakeVisible(eqTab);
    addAndMakeVisible(dynamicsTab);
    addAndMakeVisible(visualizerTab);

    // Style category buttons
    guiTab.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(91, 187, 91));
    eqTab.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(250, 144, 167));
    dynamicsTab.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(127, 222, 224));
    visualizerTab.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(187, 91, 187));
    
    // Add highlight colors for selected state
    guiTab.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(91, 187, 91).brighter(0.5f));
    eqTab.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(250, 144, 167).brighter(0.5f));
    dynamicsTab.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(127, 222, 224).brighter(0.5f));
    visualizerTab.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(187, 91, 187).brighter(0.5f));

    // Also add text color for better visibility
    guiTab.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    eqTab.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    dynamicsTab.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    visualizerTab.setColour(juce::TextButton::textColourOnId, juce::Colours::black);

    // Set up toggle state for category buttons
    guiTab.setClickingTogglesState(true);
    eqTab.setClickingTogglesState(true);
    dynamicsTab.setClickingTogglesState(true);
    visualizerTab.setClickingTogglesState(true);
    
    // Add listeners for category button clicks
    guiTab.onClick = [this] { handleCategoryButtonClicked(&guiTab, "GUI"); };
    eqTab.onClick = [this] { handleCategoryButtonClicked(&eqTab, "EQ"); };
    dynamicsTab.onClick = [this] { handleCategoryButtonClicked(&dynamicsTab, "Dynamics"); };
    visualizerTab.onClick = [this] { handleCategoryButtonClicked(&visualizerTab, "Visualizer"); };

    // Set default selection
    guiTab.setToggleState(true, juce::sendNotification);
    
    // Set up the viewport
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&blockListComponent, false);
    viewport.setScrollBarsShown(true, false);
    
    // Initialize with the first category
    updateBlocksForCategory("GUI");
    
    // Set a fixed size for the sidebar
    setSize(200, 600);
}

LeftSidebar::~LeftSidebar()
{
    // Clean up
    overarchingTabs.removeChangeListener(this);
}

void LeftSidebar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(240, 240, 240)); // Light grey background
}

void LeftSidebar::resized()
{
    auto bounds = getLocalBounds();

    // Layout overarching tabs (smaller height)
    overarchingTabs.setBounds(bounds.removeFromTop(30));

    // Split the remaining area into a column for tabs and an area for blocks
    auto remainingArea = bounds;
    auto tabColumn = remainingArea.removeFromLeft(50); // narrow column for tabs
    auto blocksArea = remainingArea; // remaining area for blocks

    // Layout category buttons with proper spacing in the left column
    int buttonHeight = 40;
    int spacing = 5;
    
    guiTab.setBounds(tabColumn.removeFromTop(buttonHeight).reduced(spacing, 0));
    tabColumn.removeFromTop(spacing); // Add spacing between buttons
    
    eqTab.setBounds(tabColumn.removeFromTop(buttonHeight).reduced(spacing, 0));
    tabColumn.removeFromTop(spacing);
    
    dynamicsTab.setBounds(tabColumn.removeFromTop(buttonHeight).reduced(spacing, 0));
    tabColumn.removeFromTop(spacing);
    
    visualizerTab.setBounds(tabColumn.removeFromTop(buttonHeight).reduced(spacing, 0));

    // Layout the viewport for blocks in the remaining area
    viewport.setBounds(blocksArea);
    
    // Calculate total height needed for all blocks
    int totalBlockHeight = blockComponents.size() * (blockHeight + 10);
    blockListComponent.setBounds(0, 0, viewport.getWidth() - viewport.getScrollBarThickness(), 
                               std::max(viewport.getHeight(), totalBlockHeight));
    
    // Position each block in the list
    int y = 10;
    for (auto* blockComp : blockComponents)
    {
        blockComp->setBounds(5, y, viewport.getWidth() - 15 - viewport.getScrollBarThickness(), blockHeight);
        y += blockHeight + 10;
    }
}

void LeftSidebar::addBlock(const juce::String& name, juce::Colour colour)
{
    auto* blockComp = new BlockComponent(name, colour);
    blockListComponent.addAndMakeVisible(blockComp);
    blockComponents.add(blockComp);
    
    // Trigger a resize to update layout
    resized();
}

void LeftSidebar::updateBlocksForCategory(const juce::String& category)
{
    // Clear existing blocks
    blockComponents.clear();
    
    // Remove all child components from blockListComponent
    blockListComponent.removeAllChildren();

    // Add new blocks based on selected category
    if (category == "GUI")
    {
        addBlockToCategory("Button", juce::Colour::fromRGB(91, 187, 91));
        addBlockToCategory("Slider", juce::Colour::fromRGB(91, 187, 91));
        addBlockToCategory("Knob", juce::Colour::fromRGB(91, 187, 91));
    }
    else if (category == "EQ")
    {
        addBlockToCategory("Highpass", juce::Colour::fromRGB(250, 144, 167));
        addBlockToCategory("Lowpass", juce::Colour::fromRGB(250, 144, 167));
        addBlockToCategory("Peaking", juce::Colour::fromRGB(250, 144, 167));
        addBlockToCategory("Frequency", juce::Colour::fromRGB(254, 204, 214));
        addBlockToCategory("Q", juce::Colour::fromRGB(254, 204, 214));
        addBlockToCategory("Gain", juce::Colour::fromRGB(254, 204, 214));
    }
    else if (category == "Dynamics")
    {
        addBlockToCategory("Compressor", juce::Colour::fromRGB(127, 222, 224));
        addBlockToCategory("Limiter", juce::Colour::fromRGB(127, 222, 224));
        addBlockToCategory("Expander", juce::Colour::fromRGB(127, 222, 224));
        addBlockToCategory("Gate", juce::Colour::fromRGB(127, 222, 224));
        addBlockToCategory("Threshold", juce::Colour::fromRGB(185, 230, 231));
        addBlockToCategory("Ratio", juce::Colour::fromRGB(185, 230, 231));
        addBlockToCategory("Attack", juce::Colour::fromRGB(185, 230, 231));
        addBlockToCategory("Release", juce::Colour::fromRGB(185, 230, 231));
        addBlockToCategory("Makeup Gain", juce::Colour::fromRGB(185, 230, 231));
        addBlockToCategory("Knee", juce::Colour::fromRGB(185, 230, 231));
    }
    else if (category == "Visualizer")
    {
        addBlockToCategory("Spectrum", juce::Colour::fromRGB(187, 91, 187));
        addBlockToCategory("Waveform", juce::Colour::fromRGB(187, 91, 187));
        addBlockToCategory("Meter", juce::Colour::fromRGB(187, 91, 187));
    }
    
    // Update layout
    resized();
}

void LeftSidebar::updateCategoryButtonsVisibility()
{
    bool isCodeTabSelected = overarchingTabs.getCurrentTabIndex() == 0;
    
    guiTab.setVisible(isCodeTabSelected);
    eqTab.setVisible(isCodeTabSelected);
    dynamicsTab.setVisible(isCodeTabSelected);
    visualizerTab.setVisible(isCodeTabSelected);
    
    // Clear and update blocks based on the selected tab
    if (isCodeTabSelected)
    {
        // Show programming blocks for Code tab
        updateBlocksForCategory(guiTab.getToggleState() ? "GUI" : 
                              eqTab.getToggleState() ? "EQ" : 
                              dynamicsTab.getToggleState() ? "Dynamics" : 
                              visualizerTab.getToggleState() ? "Visualizer" : "GUI");
    }
    else
    {
        // Show customization blocks for Customization tab
        blockComponents.clear();
        blockListComponent.removeAllChildren();
        
        addBlockToCategory("Background", juce::Colours::purple);
        addBlockToCategory("Knobs", juce::Colours::red);
        addBlockToCategory("Sliders", juce::Colours::blue);
        addBlockToCategory("Buttons", juce::Colours::green);
        addBlockToCategory("Labels", juce::Colours::yellow);
        addBlockToCategory("Dividers", juce::Colours::grey);
    }
    
    resized();
}

void LeftSidebar::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &overarchingTabs)
    {
        updateCategoryButtonsVisibility();
    }
}

void LeftSidebar::handleCategoryButtonClicked(juce::Button* clickedButton, const juce::String& category)
{
    // Deselect all other category buttons
    if (clickedButton != &guiTab) guiTab.setToggleState(false, juce::dontSendNotification);
    if (clickedButton != &eqTab) eqTab.setToggleState(false, juce::dontSendNotification);
    if (clickedButton != &dynamicsTab) dynamicsTab.setToggleState(false, juce::dontSendNotification);
    if (clickedButton != &visualizerTab) visualizerTab.setToggleState(false, juce::dontSendNotification);
    
    // Make sure the clicked button is toggled on
    clickedButton->setToggleState(true, juce::dontSendNotification);
    
    // Apply visual highlight to the selected button
    guiTab.setColour(juce::TextButton::buttonColourId, 
                    guiTab.getToggleState() ? juce::Colour::fromRGB(91, 187, 91).brighter(0.5f) : juce::Colour::fromRGB(91, 187, 91));
    eqTab.setColour(juce::TextButton::buttonColourId, 
                   eqTab.getToggleState() ? juce::Colour::fromRGB(250, 144, 167).brighter(0.5f) : juce::Colour::fromRGB(250, 144, 167));
    dynamicsTab.setColour(juce::TextButton::buttonColourId, 
                         dynamicsTab.getToggleState() ? juce::Colour::fromRGB(127, 222, 224).brighter(0.5f) : juce::Colour::fromRGB(127, 222, 224));
    visualizerTab.setColour(juce::TextButton::buttonColourId, 
                           visualizerTab.getToggleState() ? juce::Colour::fromRGB(187, 91, 187).brighter(0.5f) : juce::Colour::fromRGB(187, 91, 187));
    
    // Update blocks for the clicked category
    updateBlocksForCategory(category);
}

void LeftSidebar::addBlockToCategory(const juce::String& name, juce::Colour colour)
{
    auto* blockComp = new BlockComponent(name, colour);
    blockListComponent.addAndMakeVisible(blockComp);
    blockComponents.add(blockComp);
}
