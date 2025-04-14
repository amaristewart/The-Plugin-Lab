#pragma once

#include <JuceHeader.h>

// Utility functions and constants used throughout the application

namespace Utilities
{
    // Color definitions
    const juce::Colour backgroundColor = juce::Colour(40, 40, 40);
    const juce::Colour textColor = juce::Colours::white;
    const juce::Colour highlightColor = juce::Colour(100, 100, 255);

    // Icon resource paths
    const juce::String iconPath = "Resources/Icons/";

    // Function to load an icon from the resources - fixed with proper path handling
    static juce::Image loadIcon(const juce::String& iconName)
    {
        return juce::ImageCache::getFromFile(juce::File::getCurrentWorkingDirectory()
                                               .getChildFile(iconPath)
                                               .getChildFile(iconName));
    }

    // Function to set up default styles for components
    static void setDefaultComponentStyle(juce::Component& component)
    {
        // Using the DocumentWindow's backgroundColourId instead of Component
        component.setColour(juce::DocumentWindow::backgroundColourId, backgroundColor);
        component.setColour(juce::Label::textColourId, textColor);
    }
}