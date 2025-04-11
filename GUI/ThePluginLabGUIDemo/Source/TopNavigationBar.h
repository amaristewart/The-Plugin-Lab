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
    juce::TextButton projectsButton { "Projects" };
    juce::TextButton templatesButton { "Templates" };
    juce::TextButton learnButton { "Learn" };
    juce::TextEditor pluginNameEditor;
    juce::ImageComponent logoComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopNavigationBar)
};
