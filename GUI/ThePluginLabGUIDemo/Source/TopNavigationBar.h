#pragma once

#include <JuceHeader.h>

class TopNavigationBar : public juce::Component,
                         private juce::Timer
{
public:
    TopNavigationBar();
    ~TopNavigationBar() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Add a method to reload the logo after it's been created
    void reloadLogo();
    
private:
    // Timer callback to allow for asynchronous logo loading
    void timerCallback() override;
    
    // Helper methods for logo handling
    void loadLogo();
    void createFallbackLogo();
    
    juce::TextButton projectsButton;
    juce::TextButton templatesButton;
    juce::TextButton learnButton;
    juce::TextEditor pluginNameEditor;
    juce::Image logoImage; // Member variable to hold the logo image

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopNavigationBar)
};
