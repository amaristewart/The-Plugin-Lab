#include "ComponentPanel.h"
#include "../Components/DraggableComponent.h"

ComponentPanel::ComponentPanel()
{
    // Set up the main viewport for scrolling
    viewport.setViewedComponent(&contentContainer, false);
    viewport.setScrollBarsShown(true, false);
    addAndMakeVisible(viewport);
    
    // Create category sections
    guiSection = std::make_unique<CategorySection>("GUI Components", juce::Colour(0xFF4B77BE));
    guiSection->setDescription("Drag these components to create controls for your plugin interface. "
                              "These will control parameters of your audio processing.");
    
    eqSection = std::make_unique<CategorySection>("Equalizer Components", juce::Colour(0xFFD64541));
    eqSection->setDescription("Drag these to create filters that shape the frequency content of your audio. "
                             "Combine different filter types to create a complete EQ section.");
    
    dynamicsSection = std::make_unique<CategorySection>("Dynamics Components", juce::Colour(0xFF26A65B));
    dynamicsSection->setDescription("Use these components to control the dynamic range of your audio. "
                                  "Create compressors, limiters, and other dynamics processors.");
    
    // Add sections to content container
    contentContainer.addAndMakeVisible(guiSection.get());
    contentContainer.addAndMakeVisible(eqSection.get());
    contentContainer.addAndMakeVisible(dynamicsSection.get());
    
    // Add components to each section
    createComponents();
    
    // Set initial size
    setSize(300, 600);
}

void ComponentPanel::createComponents()
{
    guiSection->addComponent(new DraggableComponent("Knob", juce::Colours::lightblue, ComponentType::Knob));
    guiSection->addComponent(new DraggableComponent("Slider", juce::Colours::skyblue, ComponentType::Slider));
    guiSection->addComponent(new DraggableComponent("Button", juce::Colours::cornflowerblue, ComponentType::Button));
    guiSection->addComponent(new DraggableComponent("Label", juce::Colours::lightsteelblue, ComponentType::Label));
    
    eqSection->addComponent(new DraggableComponent("Low Pass", juce::Colours::indianred, ComponentType::EQLowpass));
    eqSection->addComponent(new DraggableComponent("High Pass", juce::Colours::lightcoral, ComponentType::EQHighpass));
    eqSection->addComponent(new DraggableComponent("Band Pass", juce::Colours::salmon, ComponentType::EQBandpass));
    eqSection->addComponent(new DraggableComponent("Notch", juce::Colours::tomato, ComponentType::EQNotch));
    eqSection->addComponent(new DraggableComponent("Peak EQ", juce::Colours::firebrick, ComponentType::EQPeaking));
    eqSection->addComponent(new DraggableComponent("Frequency", juce::Colours::rosybrown, ComponentType::EQFrequency));
    eqSection->addComponent(new DraggableComponent("Gain", juce::Colours::indianred, ComponentType::EQGain));
    eqSection->addComponent(new DraggableComponent("Q/Width", juce::Colours::lightcoral, ComponentType::EQQ));
    
    dynamicsSection->addComponent(new DraggableComponent("Compressor", juce::Colours::mediumseagreen, ComponentType::DynamicType));
    dynamicsSection->addComponent(new DraggableComponent("Threshold", juce::Colours::lightgreen, ComponentType::CompThreshold));
    dynamicsSection->addComponent(new DraggableComponent("Ratio", juce::Colours::palegreen, ComponentType::CompRatio));
    dynamicsSection->addComponent(new DraggableComponent("Attack", juce::Colours::limegreen, ComponentType::CompAttack));
    dynamicsSection->addComponent(new DraggableComponent("Release", juce::Colours::springgreen, ComponentType::CompRelease));
    dynamicsSection->addComponent(new DraggableComponent("Bypass", juce::Colours::yellowgreen, ComponentType::Bypass));
    
    resized();
}

void ComponentPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF2D2D2D));
}

void ComponentPanel::resized()
{
    auto bounds = getLocalBounds();
    viewport.setBounds(bounds);
    
    int scrollBarWidth = 30; 
    
    int contentWidth = juce::jmax(240, bounds.getWidth() - scrollBarWidth);
    
    // Position each section with the adjusted width
    guiSection->setBounds(0, 0, contentWidth, guiSection->getRequiredHeight());
    eqSection->setBounds(0, guiSection->getBottom() + 10, contentWidth, eqSection->getRequiredHeight());
    dynamicsSection->setBounds(0, eqSection->getBottom() + 10, contentWidth, dynamicsSection->getRequiredHeight());
    
    // Set content container size with extra padding
    contentContainer.setSize(contentWidth, dynamicsSection->getBottom() + 10);
}
