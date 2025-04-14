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
    addAndMakeVisible(topNavBar);
    addAndMakeVisible(leftSidebar);
    addAndMakeVisible(centerCanvas);
    
    setInterceptsMouseClicks(true, true);
    
    setSize(1200, 800);
}

MainComponent::~MainComponent()
{
}

//==============================================================================

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    topNavBar.setBounds(area.removeFromTop(50));

    leftSidebar.setBounds(area.removeFromLeft(200));

    centerCanvas.setBounds(area);
}
