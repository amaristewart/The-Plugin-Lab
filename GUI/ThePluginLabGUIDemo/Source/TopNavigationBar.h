#pragma once

#include <JuceHeader.h>

class TopNavigationBar : public juce::Component,
                         public juce::Timer
{
public:
    TopNavigationBar();
    ~TopNavigationBar() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Timer callback - implement this to satisfy the non-abstract requirement
    void timerCallback() override;
    
    // Method to set the plugin name
    void setPluginName(const juce::String& name);
    
    // Method to get the plugin name
    juce::String getPluginName() const;

private:
    // Replace TextButtons with ImageButtons
    juce::ImageButton projectsButton;
    juce::ImageButton templatesButton;
    juce::ImageButton learnButton;
    
    juce::TextEditor pluginNameEditor;
    juce::ImageComponent logoComponent;
    juce::ImageComponent pluginNameIconComponent;
    
    // Helper method to create placeholder button images if binary data isn't available
    juce::Image createPlaceholderButtonImage(const juce::String& label, juce::Colour colour);
    
    // Helper method to draw custom backgrounds for buttons
    void drawButtonBackground(juce::Graphics& g, const juce::ImageButton& button, juce::Colour baseColour);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopNavigationBar)
};
