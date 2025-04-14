#pragma once

#include <JuceHeader.h>
#include "Source/TopNavigationBar.h"
#include "Source/LeftSidebar.h"
#include "Source/CenterCanvas.h"

//==============================================================================
class MainComponent : public juce::Component,
                      public juce::DragAndDropContainer // Add DragAndDropContainer to main component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    // Add a method to update learn mode state across components
    void setLearnModeActive(bool active);

private:
    TopNavigationBar topNavBar;
    LeftSidebar leftSidebar;
    CenterCanvas centerCanvas;
    
    // Add a tooltip window for the learn mode
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;
    
    // Helper method to recursively set tooltip delay
    void setTooltipDelay(juce::Component* component, int delayMs);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};