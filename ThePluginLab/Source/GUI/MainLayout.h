#pragma once
#include <JuceHeader.h>
#include "PluginEditorCanvas.h"
#include "ComponentPanel.h"
#include "TopMenuBar.h"

class MainLayout : public juce::Component
{
public:
    MainLayout();
    ~MainLayout() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Top bar components
    std::unique_ptr<juce::Label> projectNameLabel;
    std::unique_ptr<juce::TextEditor> projectNameEditor;
    
    // Menu components
    std::unique_ptr<TopMenuBar> menuBar;
    std::unique_ptr<ComponentPanel> componentPanel;
    std::unique_ptr<PluginEditorCanvas> canvas;
    
    // Theme colors
    const juce::Colour backgroundColor = juce::Colour(0xFF2D2D2D);
    const juce::Colour accentColor1 = juce::Colour(0xFF61DAFB);    // Bright blue
    const juce::Colour accentColor2 = juce::Colour(0xFFFF6B6B);    // Coral pink
    const juce::Colour accentColor3 = juce::Colour(0xFF4CAF50);    // Green
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLayout)
}; 
