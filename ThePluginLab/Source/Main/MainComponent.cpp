#include "MainComponent.h"
#include "../GUI/PluginEditorCanvas.h"

MainComponent::MainComponent()
{
    // Set up component panel
    componentPanel = std::make_unique<ComponentPanel>();
    addAndMakeVisible(componentPanel.get());
    
    // Set up canvas area
    canvasArea = std::make_unique<PluginEditorCanvas>();
    addAndMakeVisible(canvasArea.get());
    
    // Set initial size
    setSize(1200, 800);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Left panel for components
    auto panelWidth = 250;
    componentPanel->setBounds(bounds.removeFromLeft(panelWidth));
    
    // Rest is canvas
    canvasArea->setBounds(bounds);
}

juce::AudioDeviceManager& MainComponent::getAudioDeviceManager()
{
    return deviceManager;
}
