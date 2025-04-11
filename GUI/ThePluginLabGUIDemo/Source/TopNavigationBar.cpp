#include "TopNavigationBar.h"
#include "Utilities.h"

//==============================================================================
// Constructor
TopNavigationBar::TopNavigationBar()
{
    juce::Logger::writeToLog("TopNavigationBar constructor called");
    
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
    
    // Add buttons and other components
    addAndMakeVisible(projectsButton);
    addAndMakeVisible(templatesButton);
    addAndMakeVisible(learnButton);
    addAndMakeVisible(pluginNameEditor);
    addAndMakeVisible(logoComponent);

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
    
    startTimerHz(1); // Check once per second
}

// Destructor
TopNavigationBar::~TopNavigationBar()
{

}

void TopNavigationBar::timerCallback()
{
    
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
    int editorX = startX + (remainingWidth - editorWidth) / 2;
    int editorY = (getHeight() - editorHeight) / 2;
    
    pluginNameEditor.setBounds(editorX, editorY, editorWidth, editorHeight);
    
    // Adjust left buttons area to start after the logo
    auto leftButtonsArea = bounds.withX(startX).removeFromLeft((remainingWidth - editorWidth) / 2);
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
}


