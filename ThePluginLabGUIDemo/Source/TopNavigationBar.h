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

    // Method to check if learn mode is active
    bool isLearnModeActive() const { return learnModeActive; }
    
    // Method to set a callback for when learn mode changes
    using LearnModeCallback = std::function<void(bool)>;
    void setLearnModeCallback(LearnModeCallback callback) { learnModeCallback = std::move(callback); }

private:
    // Replace TextButtons with ImageButtons
    juce::ImageButton projectsButton;
    juce::ImageButton templatesButton;
    juce::ImageButton learnButton;
    
    juce::TextEditor pluginNameEditor;
    juce::ImageComponent logoComponent;
    juce::ImageComponent pluginNameIconComponent;
    
    // Track learn mode state
    bool learnModeActive = false;
    LearnModeCallback learnModeCallback;
    
    // Helper method to create placeholder button images if binary data isn't available
    juce::Image createPlaceholderButtonImage(const juce::String& label, juce::Colour colour);
    
    // Helper method to draw custom backgrounds for buttons
    void drawButtonBackground(juce::Graphics& g, const juce::ImageButton& button, juce::Colour baseColour);
    
    // Helper method to update tooltips for learn mode
    void setTooltipDelay(juce::Component* component, int delayMs);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopNavigationBar)
};
