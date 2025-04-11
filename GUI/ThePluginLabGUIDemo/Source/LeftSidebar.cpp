#include "LeftSidebar.h"
#include "BlockComponent.h"

//==============================================================================
LeftSidebar::LeftSidebar()
{
    // Load tab images from BinaryData
    auto codeTabImage = juce::ImageCache::getFromMemory(BinaryData::CodeIcon_png, BinaryData::CodeIcon_pngSize);
    auto customizationTabImage = juce::ImageCache::getFromMemory(BinaryData::CustomizationIcon_png, BinaryData::CustomizationIcon_pngSize);
    
    // Create placeholder images if the BinaryData resources aren't available
    if (codeTabImage.isNull())
        codeTabImage = createTabImage("Code", juce::Colours::darkblue);
        
    if (customizationTabImage.isNull())
        customizationTabImage = createTabImage("Customization", juce::Colours::darkgreen);
    
    // Configure the tabs with custom drawing that shows the images
    overarchingTabs.setLookAndFeel(&tabLookAndFeel);
    tabLookAndFeel.setTabImage(0, codeTabImage);
    tabLookAndFeel.setTabImage(1, customizationTabImage);
    
    // Add overarching tabs with actual names (required by jassert check)
    overarchingTabs.addTab("Code", juce::Colours::white, -1);
    overarchingTabs.addTab("Customization", juce::Colours::white, -1);
    addAndMakeVisible(overarchingTabs);
    
    // Set the selected tab to "Code" by default
    overarchingTabs.setCurrentTabIndex(0);
    
    // Add listener for tab changes
    overarchingTabs.addChangeListener(this);

    // Replace shape buttons with image buttons using BinaryData resources
    // Load images from BinaryData
    auto guiIconImage = juce::ImageCache::getFromMemory(BinaryData::GUIIcon_png, BinaryData::GUIIcon_pngSize);
    auto eqIconImage = juce::ImageCache::getFromMemory(BinaryData::EQIcon_png, BinaryData::EQIcon_pngSize);
    auto dynamicsIconImage = juce::ImageCache::getFromMemory(BinaryData::DynamicsIcon_png, BinaryData::DynamicsIcon_pngSize);
    auto visualizerIconImage = juce::ImageCache::getFromMemory(BinaryData::VisualizersIcon_png, BinaryData::VisualizersIcon_pngSize);  
    // If images aren't found in BinaryData, create placeholder images
    if (guiIconImage.isNull())
    {
        guiIconImage = createPlaceholderIcon("GUI", juce::Colour::fromRGB(91, 187, 91));
    }
    
    if (eqIconImage.isNull())
    {
        eqIconImage = createPlaceholderIcon("EQ", juce::Colour::fromRGB(250, 144, 167));
    }
    
    if (dynamicsIconImage.isNull())
    {
        dynamicsIconImage = createPlaceholderIcon("DYN", juce::Colour::fromRGB(127, 222, 224));
    }
    
    if (visualizerIconImage.isNull())
    {
        visualizerIconImage = createPlaceholderIcon("VIZ", juce::Colour::fromRGB(187, 91, 187));
    }

    // Create image buttons to replace shape buttons
    guiButton = std::make_unique<juce::ImageButton>("GUI");
    eqButton = std::make_unique<juce::ImageButton>("EQ");
    dynamicsButton = std::make_unique<juce::ImageButton>("Dynamics");
    visualizerButton = std::make_unique<juce::ImageButton>("Visualizer");
    
    // Set up images for different button states (normal, over, down)
    guiButton->setImages(
        false, true, true,          // resize, maintain aspect ratio, use alpha channel
        guiIconImage, 0.7f, juce::Colours::transparentBlack,  // normal
        guiIconImage, 1.0f, juce::Colours::white.withAlpha(0.2f),  // over
        guiIconImage, 1.0f, juce::Colour::fromRGB(91, 187, 91).brighter(0.5f),  // down
        0.5f);
    
    eqButton->setImages(
        false, true, true,
        eqIconImage, 0.7f, juce::Colours::transparentBlack,
        eqIconImage, 1.0f, juce::Colours::white.withAlpha(0.2f),
        eqIconImage, 1.0f, juce::Colour::fromRGB(250, 144, 167).brighter(0.5f),
        0.5f);
    
    dynamicsButton->setImages(
        false, true, true,
        dynamicsIconImage, 0.7f, juce::Colours::transparentBlack,
        dynamicsIconImage, 1.0f, juce::Colours::white.withAlpha(0.2f),
        dynamicsIconImage, 1.0f, juce::Colour::fromRGB(127, 222, 224).brighter(0.5f),
        0.5f);
    
    visualizerButton->setImages(
        false, true, true,
        visualizerIconImage, 0.7f, juce::Colours::transparentBlack,
        visualizerIconImage, 1.0f, juce::Colours::white.withAlpha(0.2f),
        visualizerIconImage, 1.0f, juce::Colour::fromRGB(187, 91, 187).brighter(0.5f),
        0.5f);

    // Add the image buttons to this component
    addAndMakeVisible(guiButton.get());
    addAndMakeVisible(eqButton.get());
    addAndMakeVisible(dynamicsButton.get());
    addAndMakeVisible(visualizerButton.get());
    
    // Set up click callbacks for category buttons
    guiButton->onClick = [this] { handleCategoryButtonClicked(guiButton.get(), "GUI"); };
    eqButton->onClick = [this] { handleCategoryButtonClicked(eqButton.get(), "EQ"); };
    dynamicsButton->onClick = [this] { handleCategoryButtonClicked(dynamicsButton.get(), "Dynamics"); };
    visualizerButton->onClick = [this] { handleCategoryButtonClicked(visualizerButton.get(), "Visualizer"); };

    // Set default selection
    guiButton->setToggleState(true, juce::sendNotification);
    
    // Set up the viewport
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&blockListComponent, false);
    viewport.setScrollBarsShown(true, false);
    
    // Initialize with the first category
    updateBlocksForCategory("GUI");
    
    // Set a fixed size for the sidebar
    setSize(200, 600);
}

// Helper method to create placeholder icons if binary resources aren't available
juce::Image LeftSidebar::createPlaceholderIcon(const juce::String& label, juce::Colour colour)
{
    juce::Image icon(juce::Image::RGB, 40, 40, true);
    juce::Graphics g(icon);
    
    // Fill background with the category color
    g.fillAll(colour);
    
    // Add a border
    g.setColour(juce::Colours::white);
    g.drawRect(0, 0, 40, 40, 1);
    
    // Draw text
    g.setFont(14.0f);
    g.setColour(juce::Colours::white);
    g.drawText(label, 0, 0, 40, 40, juce::Justification::centred, true);
    
    return icon;
}

// Helper method to create placeholder tab images if binary resources aren't available
juce::Image LeftSidebar::createTabImage(const juce::String& label, juce::Colour colour)
{
    juce::Image icon(juce::Image::ARGB, 120, 30, true);
    juce::Graphics g(icon);
    
    // Make icon transparent initially
    g.setColour(juce::Colours::transparentBlack);
    g.fillAll();
    
    // Fill a rounded rectangle for the tab
    g.setColour(colour);
    g.fillRoundedRectangle(0.0f, 0.0f, (float)icon.getWidth(), (float)icon.getHeight(), 12.0f);
    
    // Add a subtle border
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawRoundedRectangle(0.0f, 0.0f, (float)icon.getWidth(), (float)icon.getHeight(), 12.0f, 1.0f);
    
    // Draw text
    g.setFont(16.0f);
    g.setColour(juce::Colours::white);
    g.drawText(label, 0, 0, icon.getWidth(), icon.getHeight(), juce::Justification::centred, true);
    
    return icon;
}

LeftSidebar::~LeftSidebar()
{
    // Clean up
    overarchingTabs.removeChangeListener(this);
    overarchingTabs.setLookAndFeel(nullptr);
}

void LeftSidebar::paint(juce::Graphics& g)
{
    // Change background to white
    g.fillAll(juce::Colours::white);
    
    // Draw a divider line between the category buttons and block list
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    
    // Draw vertical divider line between tab column and blocks area
    g.drawLine(50.0f, 30.0f, 50.0f, float(getHeight()), 1.0f);
    
    // Draw horizontal divider under the tabs
    g.drawLine(0.0f, 30.0f, float(getWidth()), 30.0f, 1.0f);
    
    // Draw a vertical divider line at the right edge of the sidebar
    // Use a slightly darker color for better visibility
    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    g.drawLine(float(getWidth() - 1), 0.0f, float(getWidth() - 1), float(getHeight()), 1.5f);
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

    // Increase button size - make them bigger
    int buttonHeight = 48; // Increased from 40
    int spacing = 6;     // Increased spacing slightly
    
    // Make buttons take more of the available width
    int buttonPadding = 3; // Reduced from 5 to make buttons wider
    
    guiButton->setBounds(tabColumn.removeFromTop(buttonHeight).reduced(buttonPadding, 0));
    tabColumn.removeFromTop(spacing); // Add spacing between buttons
    
    eqButton->setBounds(tabColumn.removeFromTop(buttonHeight).reduced(buttonPadding, 0));
    tabColumn.removeFromTop(spacing);
    
    dynamicsButton->setBounds(tabColumn.removeFromTop(buttonHeight).reduced(buttonPadding, 0));
    tabColumn.removeFromTop(spacing);
    
    visualizerButton->setBounds(tabColumn.removeFromTop(buttonHeight).reduced(buttonPadding, 0));

    // Layout the viewport for blocks in the remaining area
    viewport.setBounds(blocksArea);
    
    // Position each block in the list with their own heights
    int y = 10;
    for (auto* blockComp : blockComponents)
    {
        int blockHeight = blockComp->getHeight(); // Use component's actual height
        blockComp->setBounds(5, y, viewport.getWidth() - 15 - viewport.getScrollBarThickness(), blockHeight);
        y += blockHeight + 10;
    }
    
    // Update the list component size
    blockListComponent.setBounds(0, 0, viewport.getWidth() - viewport.getScrollBarThickness(), 
                               std::max(viewport.getHeight(), y));
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
        // Get viewport dimensions to calculate block sizes
        const int availableWidth = viewport.getWidth() - 25 - viewport.getScrollBarThickness();
        
        // Create custom images if BinaryData is not available
        juce::Image buttonImage, sliderImage, knobImage;
        
        // First try to load from BinaryData
        buttonImage = juce::ImageCache::getFromMemory(BinaryData::ButtonComponent_png, BinaryData::ButtonComponent_pngSize);
        sliderImage = juce::ImageCache::getFromMemory(BinaryData::SliderComponent_png, BinaryData::SliderComponent_pngSize);
        knobImage = juce::ImageCache::getFromMemory(BinaryData::KnobComponent_png, BinaryData::KnobComponent_pngSize);
        
        // If images not found, create custom ones
        if (buttonImage.isNull()) {
            buttonImage = createButtonImage();
        }
        
        if (sliderImage.isNull()) {
            sliderImage = createSliderImage();
        }
        
        if (knobImage.isNull()) {
            knobImage = createKnobImage();
        }
        
        // Button: standard height
        const int buttonHeight = 60;
        auto* buttonComp = new BlockComponent("Button", juce::Colour::fromRGB(91, 187, 91), buttonImage);
        buttonComp->setSize(availableWidth, buttonHeight);
        blockListComponent.addAndMakeVisible(buttonComp);
        blockComponents.add(buttonComp);
        
        // Slider: make MUCH taller than other components
        const int sliderHeight = 180; // Increased from 120 to 180
        auto* sliderComp = new BlockComponent("Slider", juce::Colour::fromRGB(91, 187, 91), sliderImage);
        sliderComp->setSize(availableWidth, sliderHeight);
        blockListComponent.addAndMakeVisible(sliderComp);
        blockComponents.add(sliderComp);
        
        // Knob: slightly larger than button but smaller than slider
        const int knobHeight = 80;
        auto* knobComp = new BlockComponent("Knob", juce::Colour::fromRGB(91, 187, 91), knobImage);
        knobComp->setSize(availableWidth, knobHeight);
        blockListComponent.addAndMakeVisible(knobComp);
        blockComponents.add(knobComp);
    }
    else if (category == "EQ")
    {
        // Load EQ binary data images
        auto highpassImage = juce::ImageCache::getFromMemory(BinaryData::HighpassComponent_png, BinaryData::HighpassComponent_pngSize);
        auto lowpassImage = juce::ImageCache::getFromMemory(BinaryData::LowpassComponent_png, BinaryData::LowpassComponent_pngSize);
        auto peakingImage = juce::ImageCache::getFromMemory(BinaryData::PeakingComponent_png, BinaryData::PeakingComponent_pngSize);
        auto frequencyImage = juce::ImageCache::getFromMemory(BinaryData::FrequencyComponent_png, BinaryData::FrequencyComponent_pngSize);
        auto qImage = juce::ImageCache::getFromMemory(BinaryData::QComponent_png, BinaryData::QComponent_pngSize);
        auto gainImage = juce::ImageCache::getFromMemory(BinaryData::GainComponent_png, BinaryData::GainComponent_pngSize);
        
        // Increase size for EQ blocks
        const int eqBlockWidth = viewport.getWidth() - 25 - viewport.getScrollBarThickness();
        const int eqBlockHeight = 80; // Increased from standard height
        
        // Check if images loaded successfully, otherwise use colors
        if (!highpassImage.isNull())
        {
            auto* blockComp = new BlockComponent("Highpass", juce::Colour::fromRGB(250, 144, 167), highpassImage);
            blockComp->setSize(eqBlockWidth, eqBlockHeight);
            blockListComponent.addAndMakeVisible(blockComp);
            blockComponents.add(blockComp);
        }
        else
            addBlockToCategory("Highpass", juce::Colour::fromRGB(250, 144, 167));
            
        if (!lowpassImage.isNull())
        {
            auto* blockComp = new BlockComponent("Lowpass", juce::Colour::fromRGB(250, 144, 167), lowpassImage);
            blockComp->setSize(eqBlockWidth, eqBlockHeight);
            blockListComponent.addAndMakeVisible(blockComp);
            blockComponents.add(blockComp);
        }
        else
            addBlockToCategory("Lowpass", juce::Colour::fromRGB(250, 144, 167));
            
        if (!peakingImage.isNull())
        {
            auto* blockComp = new BlockComponent("Peaking", juce::Colour::fromRGB(250, 144, 167), peakingImage);
            blockComp->setSize(eqBlockWidth, eqBlockHeight);
            blockListComponent.addAndMakeVisible(blockComp);
            blockComponents.add(blockComp);
        }
        else
            addBlockToCategory("Peaking", juce::Colour::fromRGB(250, 144, 167));
            
        if (!frequencyImage.isNull())
        {
            auto* blockComp = new BlockComponent("Frequency", juce::Colour::fromRGB(254, 204, 214), frequencyImage);
            blockComp->setSize(eqBlockWidth, eqBlockHeight);
            blockListComponent.addAndMakeVisible(blockComp);
            blockComponents.add(blockComp);
        }
        else
            addBlockToCategory("Frequency", juce::Colour::fromRGB(254, 204, 214));
            
        if (!qImage.isNull())
        {
            auto* blockComp = new BlockComponent("Q", juce::Colour::fromRGB(254, 204, 214), qImage);
            blockComp->setSize(eqBlockWidth, eqBlockHeight);
            blockListComponent.addAndMakeVisible(blockComp);
            blockComponents.add(blockComp);
        }
        else
            addBlockToCategory("Q", juce::Colour::fromRGB(254, 204, 214));
            
        if (!gainImage.isNull())
        {
            auto* blockComp = new BlockComponent("Gain", juce::Colour::fromRGB(254, 204, 214), gainImage);
            blockComp->setSize(eqBlockWidth, eqBlockHeight);
            blockListComponent.addAndMakeVisible(blockComp);
            blockComponents.add(blockComp);
        }
        else
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
    
    // Update layout with increased spacing between taller components
    int y = 10;
    for (auto* blockComp : blockComponents)
    {
        // Use block's actual height instead of standard height
        int blockHeight = blockComp->getHeight();
        blockComp->setBounds(5, y, viewport.getWidth() - 15 - viewport.getScrollBarThickness(), blockHeight);
        
        // Add extra spacing after very tall components
        if (blockHeight > 100)
            y += blockHeight + 20; // Increased spacing after tall components
        else
            y += blockHeight + 15; // Standard spacing for normal components
    }
    
    // Update total height calculation for the component
    int totalHeight = y;
    blockListComponent.setBounds(0, 0, viewport.getWidth() - viewport.getScrollBarThickness(), 
                               std::max(viewport.getHeight(), totalHeight));
}

// Add methods to create custom control images
juce::Image LeftSidebar::createButtonImage()
{
    const int width = 160;
    const int height = 60;
    
    juce::Image image(juce::Image::RGB, width, height, true);
    juce::Graphics g(image);
    
    // Fill background
    g.fillAll(juce::Colour::fromRGB(240, 240, 240));
    
    // Draw button shape
    g.setColour(juce::Colour::fromRGB(91, 187, 91));
    g.fillRoundedRectangle(10.0f, 10.0f, width - 20.0f, height - 20.0f, 6.0f);
    
    // Draw button border
    g.setColour(juce::Colour::fromRGB(60, 150, 60));
    g.drawRoundedRectangle(10.0f, 10.0f, width - 20.0f, height - 20.0f, 6.0f, 2.0f);
    
    // Draw button text
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("BUTTON", 10, 10, width - 20, height - 20, juce::Justification::centred, true);
    
    return image;
}

juce::Image LeftSidebar::createSliderImage()
{
    const int width = 160;
    const int height = 180; // Increased slider height
    
    juce::Image image(juce::Image::RGB, width, height, true);
    juce::Graphics g(image);
    
    // Fill background with a slight gradient for more dimension
    g.setGradientFill(juce::ColourGradient(
        juce::Colour::fromRGB(245, 245, 245),
        0.0f, 0.0f,
        juce::Colour::fromRGB(235, 235, 235),
        width, height,
        false
    ));
    g.fillAll();
    
    // Draw slider track - make it taller
    const int trackWidth = 24;
    const int trackHeight = height - 40;
    const int trackX = (width - trackWidth) / 2;
    const int trackY = 20;
    
    // Draw track with a gradient
    g.setGradientFill(juce::ColourGradient(
        juce::Colour::fromRGB(210, 210, 210),
        trackX, 0,
        juce::Colour::fromRGB(180, 180, 180),
        trackX + trackWidth, 0,
        false
    ));
    g.fillRoundedRectangle(trackX, trackY, trackWidth, trackHeight, 8.0f);
    
    // Add track border for definition
    g.setColour(juce::Colour::fromRGB(160, 160, 160));
    g.drawRoundedRectangle(trackX, trackY, trackWidth, trackHeight, 8.0f, 1.5f);
    
    // Draw slider thumb - make it more prominent
    const int thumbWidth = 40;
    const int thumbHeight = 20;
    const int thumbX = (width - thumbWidth) / 2;
    const int thumbY = trackY + trackHeight / 2 - thumbHeight / 2; // Position in the middle
    
    // Draw thumb with gradient for 3D effect
    g.setGradientFill(juce::ColourGradient(
        juce::Colour::fromRGB(110, 220, 110),
        thumbX, thumbY,
        juce::Colour::fromRGB(80, 180, 80),
        thumbX, thumbY + thumbHeight,
        false
    ));
    g.fillRoundedRectangle(thumbX, thumbY, thumbWidth, thumbHeight, 6.0f);
    
    // Add thumb border
    g.setColour(juce::Colour::fromRGB(60, 140, 60));
    g.drawRoundedRectangle(thumbX, thumbY, thumbWidth, thumbHeight, 6.0f, 1.5f);
    
    // Add horizontal lines to thumb for visual interest
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    for (int i = 0; i < 3; i++) {
        float lineY = thumbY + 5 + i * 5;
        g.drawLine(thumbX + 8, lineY, thumbX + thumbWidth - 8, lineY, 1.0f);
    }
    
    // Add tick marks on the track
    g.setColour(juce::Colours::white);
    for (int i = 0; i < 5; i++) {
        float tickY = trackY + (trackHeight * i / 4.0f);
        g.drawLine(trackX - 5, tickY, trackX, tickY, 1.0f);
        g.drawLine(trackX + trackWidth, tickY, trackX + trackWidth + 5, tickY, 1.0f);
    }
    
    // Draw label
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("SLIDER", 0, height - 25, width, 25, juce::Justification::centred, true);
    
    return image;
}

juce::Image LeftSidebar::createKnobImage()
{
    const int width = 160;
    const int height = 80;
    
    juce::Image image(juce::Image::RGB, width, height, true);
    juce::Graphics g(image);
    
    // Fill background
    g.fillAll(juce::Colour::fromRGB(240, 240, 240));
    
    // Draw knob
    const int knobSize = 50;
    const int knobX = (width - knobSize) / 2;
    const int knobY = 5;
    
    // Draw outer ring
    g.setColour(juce::Colour::fromRGB(180, 180, 180));
    g.fillEllipse(knobX, knobY, knobSize, knobSize);
    
    // Draw knob face
    g.setColour(juce::Colour::fromRGB(91, 187, 91));
    g.fillEllipse(knobX + 5, knobY + 5, knobSize - 10, knobSize - 10);
    
    // Draw indicator line
    g.setColour(juce::Colours::white);
    const float centerX = knobX + knobSize / 2.0f;
    const float centerY = knobY + knobSize / 2.0f;
    const float angle = -juce::MathConstants<float>::pi * 0.25f; // 45 degrees from top
    const float length = knobSize / 2.0f - 8.0f;
    
    g.drawLine(centerX, centerY, 
              centerX + length * std::sin(angle), 
              centerY - length * std::cos(angle), 
              2.0f);
    
    // Draw label
    g.setColour(juce::Colours::black);
    g.setFont(16.0f);
    g.drawText("KNOB", 0, knobY + knobSize + 5, width, 20, juce::Justification::centred, true);
    
    return image;
}

void LeftSidebar::updateCategoryButtonsVisibility()
{
    bool isCodeTabSelected = overarchingTabs.getCurrentTabIndex() == 0;
    
    guiButton->setVisible(isCodeTabSelected);
    eqButton->setVisible(isCodeTabSelected);
    dynamicsButton->setVisible(isCodeTabSelected);
    visualizerButton->setVisible(isCodeTabSelected);
    
    // Update the blocks list based on the selected tab and category
    if (isCodeTabSelected)
    {
        // Show programming blocks for Code tab
        if (guiButton->getToggleState())
            updateBlocksForCategory("GUI");
        else if (eqButton->getToggleState())
            updateBlocksForCategory("EQ");
        else if (dynamicsButton->getToggleState())
            updateBlocksForCategory("Dynamics");
        else if (visualizerButton->getToggleState())
            updateBlocksForCategory("Visualizer");
        else
            updateBlocksForCategory("GUI");  // Default to GUI category
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
    // Update toggle states - only one button should be toggled at a time
    guiButton->setToggleState(clickedButton == guiButton.get(), juce::dontSendNotification);
    eqButton->setToggleState(clickedButton == eqButton.get(), juce::dontSendNotification);
    dynamicsButton->setToggleState(clickedButton == dynamicsButton.get(), juce::dontSendNotification);
    visualizerButton->setToggleState(clickedButton == visualizerButton.get(), juce::dontSendNotification);
    
    // Update the blocks list based on the selected category
    updateBlocksForCategory(category);
}

void LeftSidebar::addBlockToCategory(const juce::String& name, juce::Colour colour)
{
    auto* blockComp = new BlockComponent(name, colour);
    blockListComponent.addAndMakeVisible(blockComp);
    blockComponents.add(blockComp);
}
