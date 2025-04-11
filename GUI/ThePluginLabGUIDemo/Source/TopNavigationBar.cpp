#include "TopNavigationBar.h"
#include "Utilities.h"

//==============================================================================
// Constructor
TopNavigationBar::TopNavigationBar()
{
    juce::Logger::writeToLog("TopNavigationBar constructor called");
    
    // Load logo image
    auto logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
    
    if (! logoImage.isNull())
    {
        logoComponent.setImage(logoImage, juce::Justification::centredLeft);
        int logoHeight = 60;
        logoComponent.setSize(logoImage.getWidth() * logoHeight / logoImage.getHeight(), logoHeight);
    }
    else
    {
        jassert (!logoImage.isNull()); // Ensure logo image is loaded correctly
        juce::Logger::writeToLog("Logo image is null - could not load from BinaryData");
    }
    
    // Load the plugin name icon
    auto pluginNameIcon = juce::ImageCache::getFromMemory(BinaryData::PluginNameIcon_png, BinaryData::PluginNameIcon_pngSize);
    
    if (!pluginNameIcon.isNull())
    {
        pluginNameIconComponent.setImage(pluginNameIcon, juce::Justification::centred);
        int iconHeight = 30;
        pluginNameIconComponent.setSize(pluginNameIcon.getWidth() * iconHeight / pluginNameIcon.getHeight(), iconHeight);
    }
    else
    {
        // Create a placeholder icon if BinaryData doesn't have it
        juce::Image placeholderIcon(juce::Image::RGB, 30, 30, true);
        juce::Graphics g(placeholderIcon);
        g.setColour(juce::Colours::blue);
        g.fillEllipse(2.0f, 2.0f, 26.0f, 26.0f);
        g.setColour(juce::Colours::white);
        g.drawText("?", 0, 0, 30, 30, juce::Justification::centred, true);
        
        pluginNameIconComponent.setImage(placeholderIcon, juce::Justification::centred);
        pluginNameIconComponent.setSize(30, 30);
    }
    
    // Load navigation button images from BinaryData
    auto projectsImage = juce::ImageCache::getFromMemory(BinaryData::ProjectsIcon_png, BinaryData::ProjectsIcon_pngSize);
    auto templatesImage = juce::ImageCache::getFromMemory(BinaryData::TemplatesIcon_png, BinaryData::TemplatesIcon_pngSize);
    auto learnImage = juce::ImageCache::getFromMemory(BinaryData::LearnIcon_png, BinaryData::LearnIcon_pngSize);
    
    // Create placeholder images if needed
    if (projectsImage.isNull())
        projectsImage = createPlaceholderButtonImage("Projects", juce::Colours::blue);
    
    if (templatesImage.isNull())
        templatesImage = createPlaceholderButtonImage("Templates", juce::Colours::green);
    
    if (learnImage.isNull())
        learnImage = createPlaceholderButtonImage("Learn", juce::Colours::orange);
    
    // Configure the image buttons with background highlight instead of image highlight
    projectsButton.setImages(
        false, true, true,          // resize, maintain aspect ratio, use alpha channel
        projectsImage, 1.0f, juce::Colours::transparentBlack,  // normal: full opacity image
        projectsImage, 1.0f, juce::Colours::transparentBlack,  // over: no image highlight
        projectsImage, 1.0f, juce::Colours::transparentBlack,  // down: no image highlight
        0.5f);
        
    templatesButton.setImages(
        false, true, true,
        templatesImage, 1.0f, juce::Colours::transparentBlack,
        templatesImage, 1.0f, juce::Colours::transparentBlack,
        templatesImage, 1.0f, juce::Colours::transparentBlack,
        0.5f);
        
    learnButton.setImages(
        false, true, true,
        learnImage, 1.0f, juce::Colours::transparentBlack,
        learnImage, 1.0f, juce::Colours::transparentBlack,
        learnImage, 1.0f, juce::Colours::transparentBlack,
        0.5f);
        
    // Set custom look and feel for the background highlight
    projectsButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    templatesButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    learnButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    
    // Add buttons and other components
    addAndMakeVisible(projectsButton);
    addAndMakeVisible(templatesButton);
    addAndMakeVisible(learnButton);
    addAndMakeVisible(pluginNameEditor);
    addAndMakeVisible(logoComponent);
    addAndMakeVisible(pluginNameIconComponent);

    pluginNameEditor.setText("Plugin Name");
    pluginNameEditor.setReadOnly(false); // Allow editing of the text field

    // Configure the plugin name editor for center alignment
    pluginNameEditor.setJustification(juce::Justification::centred);
    pluginNameEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white.withAlpha(0.8f));
    pluginNameEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::lightgrey);
    pluginNameEditor.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::blue);
    pluginNameEditor.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    
    // Also set the color for the text when not focused (default text color)
    pluginNameEditor.applyColourToAllText(juce::Colours::black, true);
    
    startTimerHz(1); // Check once per second
}

// Destructor
TopNavigationBar::~TopNavigationBar()
{

}

void TopNavigationBar::timerCallback()
{
    
}

// Helper method to create placeholder button images
juce::Image TopNavigationBar::createPlaceholderButtonImage(const juce::String& label, juce::Colour colour)
{
    juce::Image image(juce::Image::RGB, 100, 30, true);
    juce::Graphics g(image);
    
    // Fill with gradient background
    juce::ColourGradient gradient(colour.brighter(0.2f), 0, 0, colour, 0, 30, false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(0.0f, 0.0f, 100.0f, 30.0f, 6.0f);
    
    // Add text
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText(label, 0, 0, 100, 30, juce::Justification::centred, true);
    
    return image;
}

// Resized method
void TopNavigationBar::resized()
{
    auto bounds = getLocalBounds();
    const int buttonWidth = 100;
    const int buttonHeight = 30;
    
    // Position the logo at the left edge with proper alignment
    int logoY = (bounds.getHeight() - logoComponent.getHeight()) / 2;
    logoComponent.setBounds(0, logoY, logoComponent.getWidth(), logoComponent.getHeight());
    
    // Calculate the remaining width for positioning elements
    int remainingWidth = bounds.getWidth() - logoComponent.getWidth() - 20; // Account for logo width
    int startX = logoComponent.getWidth() + 20; // Start after the logo
    
    // Position the plugin name editor in the center of the remaining area
    int editorWidth = 200;
    int editorHeight = buttonHeight;
    int editorX = startX + (remainingWidth - editorWidth - pluginNameIconComponent.getWidth() - 5) / 2;
    int editorY = (getHeight() - editorHeight) / 2;
    
    // Position plugin name editor
    pluginNameEditor.setBounds(editorX, editorY, editorWidth, editorHeight);
    
    // Position plugin name icon to the right of the editor
    pluginNameIconComponent.setBounds(
        pluginNameEditor.getRight() + 5,
        editorY + (editorHeight - pluginNameIconComponent.getHeight()) / 2,
        pluginNameIconComponent.getWidth(), 
        pluginNameIconComponent.getHeight()
    );
    
    // Adjust left buttons area to start after the logo
    auto leftButtonsArea = bounds.withX(startX).removeFromLeft((remainingWidth - editorWidth - pluginNameIconComponent.getWidth() - 10) / 2);
    leftButtonsArea = leftButtonsArea.withSizeKeepingCentre(buttonWidth * 3 + 20, buttonHeight);
    
    projectsButton.setBounds(leftButtonsArea.removeFromLeft(buttonWidth).withHeight(buttonHeight));
    leftButtonsArea.removeFromLeft(10); // Spacing
    templatesButton.setBounds(leftButtonsArea.removeFromLeft(buttonWidth).withHeight(buttonHeight));
    leftButtonsArea.removeFromLeft(10); // Spacing
    learnButton.setBounds(leftButtonsArea.removeFromLeft(buttonWidth).withHeight(buttonHeight));
}

void TopNavigationBar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xffebfffd)); // Light cyan background with full alpha (ff prefix)
    
    // Draw custom backgrounds for buttons when hovered or clicked
    drawButtonBackground(g, projectsButton, juce::Colour(0xffc4d6d3));
    drawButtonBackground(g, templatesButton, juce::Colour(0xffc4d6d3));
    drawButtonBackground(g, learnButton, juce::Colour(0xffc4d6d3));
}

// Add this helper method to draw custom button backgrounds
void TopNavigationBar::drawButtonBackground(juce::Graphics& g, const juce::ImageButton& button, juce::Colour baseColour)
{
    if (button.isMouseOver() || button.isMouseButtonDown())
    {
        juce::Rectangle<float> bounds = button.getBounds().toFloat().reduced(2.0f);
        
        // Create a rounded rectangle background when hovered
        if (button.isMouseButtonDown())
        {
            // Darker background when clicked
            g.setColour(baseColour.withAlpha(0.3f));
        }
        else
        {
            // Lighter background when just hovering
            g.setColour(baseColour.withAlpha(0.15f));
        }
        
        // Draw the rounded rectangle background
        g.fillRoundedRectangle(bounds, 6.0f);
        
        // Add a subtle border
        g.setColour(baseColour.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
    }
}


