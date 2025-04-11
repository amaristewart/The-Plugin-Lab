#pragma once

#include <JuceHeader.h>

// Forward declarations
class BlockComponent;

// Custom LookAndFeel class for image tabs
class ImageTabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void setTabImage(int tabIndex, const juce::Image& image)
    {
        // Make sure we have enough space in our array
        while (tabImages.size() <= tabIndex)
            tabImages.add(juce::Image());
            
        tabImages.set(tabIndex, image);
    }
    
    void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        const juce::Rectangle<int> activeArea (button.getActiveArea());
        int tabIndex = button.getIndex();
        
        // Make the background completely transparent to avoid covering the tab image
        g.setColour(juce::Colours::transparentBlack);
        g.fillRect(activeArea);
        
        if (tabIndex >= 0 && tabIndex < tabImages.size() && tabImages[tabIndex].isValid())
        {
            // Draw the image scaled to fit
            bool isSelected = button.getToggleState();
            float alpha = isSelected ? 1.0f : (isMouseOver ? 0.8f : 0.6f);
            
            g.setOpacity(alpha);
            
            // Create a custom drawing area based on tab index
            juce::Rectangle<int> imageArea;
            
            if (tabIndex == 0) // Code tab
            {
                // Keep the code tab in its original position
                imageArea = activeArea;
            }
            else if (tabIndex == 1) // Customization tab
            {
                // Move customization tab to the left by -2 pixels from the original position
                juce::Rectangle<int> adjustedArea = activeArea;
                adjustedArea.setX(activeArea.getX() - 2);
                imageArea = adjustedArea;
            }
            else
            {
                imageArea = activeArea; // Default for other tabs
            }
            
            // Add rounded rectangle behind the tab image for better appearance
            if (isSelected || isMouseOver)
            {
                g.setColour(isSelected ? juce::Colours::white.withAlpha(0.9f) : juce::Colours::lightgrey.withAlpha(0.6f));
                g.fillRoundedRectangle(imageArea.toFloat().reduced(-5, -2), 12.0f);
            }
            
            g.drawImageWithin(tabImages[tabIndex], 
                             imageArea.getX(), 
                             imageArea.getY(), 
                             imageArea.getWidth(), 
                             imageArea.getHeight(),
                             juce::RectanglePlacement::centred);
        }
        else
        {
            // Fallback to default tab drawing if image isn't available
            LookAndFeel_V4::drawTabButton(button, g, isMouseOver, isMouseDown);
        }
    }
    
    // Override the tab background drawing to make it fully transparent
    void drawTabAreaBehindFrontButton(juce::TabbedButtonBar& bar, juce::Graphics& g, int w, int h) override
    {
        // Draw nothing, completely transparent background
    }
    
private:
    juce::Array<juce::Image> tabImages;
};

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

    // Custom look and feel for image tabs
    ImageTabLookAndFeel tabLookAndFeel;
    
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
    
    // Helper method to create placeholder tab images
    juce::Image createTabImage(const juce::String& label, juce::Colour colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeftSidebar)
};
