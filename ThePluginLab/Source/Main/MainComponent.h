#pragma once
#include <JuceHeader.h>
#include "../GUI/ComponentPanel.h"

class PluginEditorCanvas;

class MainComponent : public juce::Component,
                     public juce::DragAndDropContainer
{
public:
    MainComponent();
    ~MainComponent() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    juce::AudioDeviceManager& getAudioDeviceManager();
    
private:
    std::unique_ptr<ComponentPanel> componentPanel;
    std::unique_ptr<PluginEditorCanvas> canvasArea;
    
    juce::AudioDeviceManager deviceManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};