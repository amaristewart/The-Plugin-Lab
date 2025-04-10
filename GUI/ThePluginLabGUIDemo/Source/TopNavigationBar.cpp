#include "TopNavigationBar.h"
#include "Utilities.h"

//==============================================================================
// Constructor
TopNavigationBar::TopNavigationBar()
{
    juce::Logger::writeToLog("TopNavigationBar constructor called");
    
    // Try to load the logo with a more focused approach
    loadLogo();
    
    // Add buttons and other components
    addAndMakeVisible(projectsButton);
    addAndMakeVisible(templatesButton);
    addAndMakeVisible(learnButton);
    addAndMakeVisible(pluginNameEditor);

    projectsButton.setButtonText("Projects");
    templatesButton.setButtonText("Templates");
    learnButton.setButtonText("Learn");

    pluginNameEditor.setText("Plugin Name");
    pluginNameEditor.setReadOnly(false); // Allow editing of the text field

    // Configure the plugin name editor for center alignment
    pluginNameEditor.setJustification(juce::Justification::centred);
    pluginNameEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white.withAlpha(0.8f));
    pluginNameEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::lightgrey);
    pluginNameEditor.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::blue);
    
    // Start a timer to reload the logo if it wasn't available initially
    startTimerHz(1); // Check once per second
}

// Destructor
TopNavigationBar::~TopNavigationBar()
{
    stopTimer();
}

void TopNavigationBar::timerCallback()
{
    // If we don't have a valid logo yet, try again
    if (!logoImage.isValid())
    {
        loadLogo();
        if (logoImage.isValid())
        {
            // Stop the timer once we have a valid logo
            stopTimer();
            repaint();
        }
    }
    else
    {
        // We have a logo, stop checking
        stopTimer();
    }
}

void TopNavigationBar::loadLogo()
{
    try {
        juce::Array<juce::File> possiblePaths;
        
        // First try the exact specified path
        possiblePaths.add(juce::File("/Users/amaristewart/Documents/GitHub/The-Plugin-Lab/GUI/ThePluginLabGUIDemo/Resources/logo.png"));
        
        // Then try other possible locations as fallbacks
        possiblePaths.add(juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                        .getParentDirectory().getChildFile("Resources/logo.png"));
        possiblePaths.add(juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                        .getParentDirectory().getChildFile("Resources/logo.png"));  
        possiblePaths.add(juce::File::getCurrentWorkingDirectory().getChildFile("Resources/logo.png"));
        
        // Log the paths we're checking
        static bool firstAttempt = true;
        if (firstAttempt)
        {
            juce::Logger::writeToLog("Looking for logo in:");
            for (auto& path : possiblePaths)
            {
                juce::Logger::writeToLog("  " + path.getFullPathName() + 
                                      (path.existsAsFile() ? " (exists)" : " (not found)"));
            }
            firstAttempt = false;
        }
        
        // Try to load from any of these locations
        for (auto& path : possiblePaths)
        {
            if (path.existsAsFile())
            {
                logoImage = juce::ImageFileFormat::loadFrom(path);
                if (logoImage.isValid())
                {
                    juce::Logger::writeToLog("Successfully loaded logo from: " + path.getFullPathName());
                    return;
                }
            }
        }
        
        // If still not loaded, create an in-memory logo as fallback
        if (!logoImage.isValid())
        {
            // Only log this once, not every timer check
            static bool fallbackCreated = false;
            if (!fallbackCreated)
            {
                juce::Logger::writeToLog("Creating in-memory logo as fallback");
                fallbackCreated = true;
            }
            
            // Create a larger fallback logo with appropriate dimensions
            logoImage = juce::Image(juce::Image::RGB, 240, 46, true);
            juce::Graphics g(logoImage);
            
            // Add a gradient background for a nicer look
            juce::ColourGradient gradient(juce::Colour(100, 150, 255), 0, 0,
                                        juce::Colour(50, 100, 200), 240, 46, false);
            g.setGradientFill(gradient);
            g.fillRoundedRectangle(0, 0, 240, 46, 8.0f);
            
            g.setColour(juce::Colours::white);
            g.setFont(26.0f);  // Larger font
            g.drawText("The Plugin Lab", logoImage.getBounds().reduced(10, 0), 
                      juce::Justification::centredLeft, true);  // Left-aligned text
        }
    }
    catch (const std::exception& e) {
        juce::Logger::writeToLog("Exception loading logo: " + juce::String(e.what()));
        createFallbackLogo();
    }
}

void TopNavigationBar::createFallbackLogo()
{
    // Create a fallback logo with a different color to indicate an error
    logoImage = juce::Image(juce::Image::RGB, 240, 46, true);
    juce::Graphics g(logoImage);
    g.fillAll(juce::Colour::fromRGB(255, 200, 200)); // Pink to indicate error
    g.setColour(juce::Colours::black);
    g.setFont(26.0f);  // Larger font
    g.drawText("The Plugin Lab", logoImage.getBounds().reduced(10, 0), 
              juce::Justification::centredLeft, true);  // Left-aligned text
}

//==============================================================================
// Paint method
void TopNavigationBar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(200, 240, 255)); // Light blue background

    // Draw the logo or text based on image validity
    if (logoImage.isValid())
    {
        // Draw the logo image - MUCH bigger and positioned in the very top-left corner
        auto logoBounds = juce::Rectangle<int>(5, 2, 240, 46);  // Increased size, positioned in top-left corner
        g.drawImageWithin(logoImage, logoBounds.getX(), logoBounds.getY(), logoBounds.getWidth(), logoBounds.getHeight(),
                          juce::RectanglePlacement::centred | juce::RectanglePlacement::xLeft); // Fix: use centred with xLeft flag
    }
    else
    {
        // Fallback text if image loading fails - also adjusted for top left positioning
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(22.0f, juce::Font::bold));
        g.drawText("The Plugin Lab", juce::Rectangle<int>(5, 2, 240, 46), juce::Justification::centredLeft, true);
    }
}

// Resized method
void TopNavigationBar::resized()
{
    auto bounds = getLocalBounds();
    const int buttonWidth = 100;
    const int buttonHeight = 30;
    
    // Reserve space for logo in the left - significantly increased space for larger logo
    auto logoBounds = bounds.removeFromLeft(260); 
    
    // Calculate the remaining width for positioning elements
    int remainingWidth = bounds.getWidth();
    
    // Position the plugin name editor in the center of the bar
    int editorWidth = 200;
    int editorHeight = buttonHeight;
    int editorX = bounds.getX() + (remainingWidth - editorWidth) / 2;
    int editorY = (getHeight() - editorHeight) / 2;
    
    pluginNameEditor.setBounds(editorX, editorY, editorWidth, editorHeight);
    
    // Position the buttons to the right of the logo
    auto leftButtonsArea = bounds.removeFromLeft((remainingWidth - editorWidth) / 2);
    leftButtonsArea = leftButtonsArea.withSizeKeepingCentre(buttonWidth * 3 + 20, buttonHeight);
    
    projectsButton.setBounds(leftButtonsArea.removeFromLeft(buttonWidth).withHeight(buttonHeight));
    leftButtonsArea.removeFromLeft(10); // Spacing
    templatesButton.setBounds(leftButtonsArea.removeFromLeft(buttonWidth).withHeight(buttonHeight));
    leftButtonsArea.removeFromLeft(10); // Spacing
    learnButton.setBounds(leftButtonsArea.removeFromLeft(buttonWidth).withHeight(buttonHeight));
}

// Expose this to be called from MainComponent after creating the logo
void TopNavigationBar::reloadLogo()
{
    loadLogo();
    repaint();
}
