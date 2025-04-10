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
    // Initialize components
    addAndMakeVisible(topNavBar);
    addAndMakeVisible(leftSidebar);
    addAndMakeVisible(centerCanvas);
    
    // Set this component to receive mouse events
    setInterceptsMouseClicks(true, true);
    
    // Tell the top nav bar to reload the logo (it should be available now)
    topNavBar.reloadLogo();
    
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
