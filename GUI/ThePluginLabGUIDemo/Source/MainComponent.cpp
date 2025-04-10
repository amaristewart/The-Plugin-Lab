#include "MainComponent.h"
#include "Source/TopNavigationBar.h"
#include "Source/LeftSidebar.h"
#include "Source/CenterCanvas.h"

//==============================================================================
MainComponent::MainComponent() : 
    topNavBar(),
    leftSidebar(),
    centerCanvas()
{
    // Add this code to create a Resources folder and logo if needed
    juce::File resourcesDir = juce::File::getCurrentWorkingDirectory().getChildFile("Resources");
    if (!resourcesDir.exists())
        resourcesDir.createDirectory();
        
    juce::File logoFile = resourcesDir.getChildFile("logo.png");
    if (!logoFile.exists())
    {
        // Create a placeholder logo if it doesn't exist
        juce::Image placeholderLogo(juce::Image::RGB, 200, 50, true);
        juce::Graphics g(placeholderLogo);
        g.setColour(juce::Colours::lightblue);
        g.fillAll();
        g.setColour(juce::Colours::black);
        g.setFont(24.0f);
        g.drawText("The Plugin Lab", placeholderLogo.getBounds(), juce::Justification::centred, true);
        
        juce::PNGImageFormat format;
        juce::FileOutputStream stream(logoFile);
        if (stream.openedOk())
            format.writeImageToStream(placeholderLogo, stream);
    }

    // Initialize components
    addAndMakeVisible(topNavBar);
    addAndMakeVisible(leftSidebar);
    addAndMakeVisible(centerCanvas);
    
    // Set this component to receive mouse events
    setInterceptsMouseClicks(true, true);
    
    // Set size
    setSize(1200, 800);
}

MainComponent::~MainComponent()
{
    // Your code here
}

//==============================================================================

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    // Position the top navigation bar
    topNavBar.setBounds(area.removeFromTop(50));

    // Position the left sidebar
    leftSidebar.setBounds(area.removeFromLeft(200));

    // The remaining area is for the center canvas
    centerCanvas.setBounds(area);
}