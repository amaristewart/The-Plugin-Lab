#pragma once

#include <JuceHeader.h>
#include "Source/TopNavigationBar.h"
#include "Source/LeftSidebar.h"
#include "Source/CenterCanvas.h"

//==============================================================================
class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Helper function to create placeholder EQ component images
    void createPlaceholderEQImage(const juce::File& directory, const juce::String& filename, const juce::String& label);

    TopNavigationBar topNavBar;
    LeftSidebar leftSidebar;
    CenterCanvas centerCanvas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};