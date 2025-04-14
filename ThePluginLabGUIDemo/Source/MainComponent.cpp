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
    
    // Set size
    setSize(1200, 800);
    
    // Create the tooltip window properly to ensure it works
    // Use a unique_ptr and make sure it's attached to the desktop (not this component)
    tooltipWindow = std::make_unique<juce::TooltipWindow>(&juce::Desktop::getInstance());
    
    // Set a very short delay for tooltips by default (10ms)
    tooltipWindow->setMillisecondsBeforeTipAppears(800);
    
    // Register learn mode callback
    topNavBar.setLearnModeCallback([this](bool learnMode) {
        setLearnModeActive(learnMode);
    });
    
    // Set tooltips on main components
    leftSidebar.setTooltip("Sidebar: Contains blocks you can drag onto the canvas");
    centerCanvas.setTooltip("Canvas: Drag blocks here to build your plugin");
}

MainComponent::~MainComponent()
{
    // Your code here
}

//==============================================================================

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
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

void MainComponent::setLearnModeActive(bool active)
{
    // Configure tooltip window for learn mode
    if (tooltipWindow)
    {
        // Debug logging to verify this gets called
        juce::Logger::writeToLog("Setting learn mode: " + juce::String(active ? "ON" : "OFF"));
        
        // Show tooltips immediately in learn mode or with delay in normal mode
        tooltipWindow->setMillisecondsBeforeTipAppears(active ? 1 : 800);
    }
    
    // Apply tooltip delay recursively to all components in the application
    // This helps ensure consistent tooltip behavior throughout the app
    setTooltipDelay(this, active ? 1 : 800);
    
    // Force UI update to ensure tooltips appear
    if (active)
    {
        // Ensure tooltips are set on important elements
        leftSidebar.setTooltip("Sidebar: Contains blocks you can drag onto the canvas");
        centerCanvas.setTooltip("Canvas: Drag blocks here to build your plugin");
        
        // Show a temporary message to confirm that learn mode is activated
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                            "Learn Mode Activated",
                                            "Hover over components to see tooltips.",
                                            "OK");
        
        // Force tooltips to update by moving the mouse cursor slightly
        auto& desktop = juce::Desktop::getInstance();
        auto currentPosition = desktop.getMainMouseSource().getScreenPosition();
        auto& display = desktop.getDisplays().getDisplayContaining(currentPosition);
        
        // Move mouse cursor slightly to trigger tooltip update
        desktop.getMainMouseSource().setScreenPosition(currentPosition.translated(1, 1));
        desktop.getMainMouseSource().setScreenPosition(currentPosition);
    }
}

// Add a helper method to recursively set tooltip delay on all components
void MainComponent::setTooltipDelay(juce::Component* component, int delayMs)
{
    if (component == nullptr)
        return;
    
    // Set tooltip delay for this component
    component->setTooltipDelay(delayMs);
    
    // Recursively set for all child components
    for (int i = 0; i < component->getNumChildComponents(); ++i)
    {
        setTooltipDelay(component->getChildComponent(i), delayMs);
    }
}