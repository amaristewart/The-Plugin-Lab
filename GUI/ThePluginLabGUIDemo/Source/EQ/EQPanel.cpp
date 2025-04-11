#include "EQPanel.h"

EQPanel::EQPanel()
{
    // Add all components as children
    addAndMakeVisible(frequencyComponent);
    addAndMakeVisible(gainComponent);
    addAndMakeVisible(qComponent);
    addAndMakeVisible(highpassComponent);
    addAndMakeVisible(lowpassComponent);
    addAndMakeVisible(peakingComponent);
    
    // Hide all components initially
    frequencyComponent.setVisible(false);
    gainComponent.setVisible(false);
    qComponent.setVisible(false);
    highpassComponent.setVisible(false);
    lowpassComponent.setVisible(false);
    peakingComponent.setVisible(false);
    
    // Show a default component initially
    showComponent(&frequencyComponent);
}

EQPanel::~EQPanel()
{
    // Cleanup code if needed
}

void EQPanel::paint(juce::Graphics& g)
{
    // Draw a visible background to confirm the panel is present
    g.fillAll(juce::Colour(20, 20, 20));
    
    // Draw a border
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
    
    // If no component is visible, draw a message
    if (currentComponent == nullptr || !currentComponent->isVisible())
    {
        g.setColour(juce::Colours::red);
        g.setFont(24.0f);
        g.drawText("No EQ Component Visible!", getLocalBounds().reduced(50), 
                  juce::Justification::centred, true);
    }
}

void EQPanel::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // Set the components to the size of your images (580px width)
    const int componentWidth = 580;
    const int componentHeight = 427;  // Using minimum height, adjust if needed
    
    frequencyComponent.setBounds(bounds.withSizeKeepingCentre(componentWidth, componentHeight));
    gainComponent.setBounds(bounds.withSizeKeepingCentre(componentWidth, componentHeight));
    qComponent.setBounds(bounds.withSizeKeepingCentre(componentWidth, componentHeight));
    highpassComponent.setBounds(bounds.withSizeKeepingCentre(componentWidth, componentHeight));
    lowpassComponent.setBounds(bounds.withSizeKeepingCentre(componentWidth, componentHeight));
    peakingComponent.setBounds(bounds.withSizeKeepingCentre(componentWidth, componentHeight));
}

void EQPanel::showComponent(juce::Component* componentToShow)
{
    // Hide current component if there is one
    if (currentComponent != nullptr)
        currentComponent->setVisible(false);
    
    // Show the new component
    if (componentToShow != nullptr)
        componentToShow->setVisible(true);
    
    // Update current component pointer
    currentComponent = componentToShow;
}
