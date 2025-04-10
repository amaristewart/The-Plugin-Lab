#include "PluginNodeComponent.h"

PluginNodeComponent::PluginNodeComponent(const juce::String& name, const juce::Colour& color)
    : nodeName(name), nodeColor(color)
{
    setSize(150, 100); // Default size
    
    // Set constraints for dragging
    constrainer.setMinimumSize(100, 80);
    constrainer.setMaximumSize(400, 400);
}

PluginNodeComponent::~PluginNodeComponent()
{
    // Base class destructor - cleanup done by derived classes
}

void PluginNodeComponent::paint(juce::Graphics& g)
{
    // Draw node background with a gradient
    g.setGradientFill(juce::ColourGradient(
        nodeColor.brighter(0.2f), 0.0f, 0.0f,
        nodeColor.darker(0.3f), static_cast<float>(getWidth()), static_cast<float>(getHeight()),
        false
    ));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 8.0f);
    
    // Draw border
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 8.0f, 1.5f);
    
    // Draw title with better styling
    g.setFont(juce::Font(juce::Font::getDefaultSansSerifFontName(), 18.0f, juce::Font::bold));
    g.setColour(juce::Colours::white);
    g.drawText(nodeName, getLocalBounds().removeFromTop(30),
              juce::Justification::centred, true);
}

void PluginNodeComponent::resized()
{
    // Base implementation does nothing - derived classes should override
}

void PluginNodeComponent::mouseDown(const juce::MouseEvent& e)
{
    // Bring to front when clicked
    toFront(true);
    
    // Start dragging
    dragger.startDraggingComponent(this, e);
    isDragging = true;
}

void PluginNodeComponent::mouseDrag(const juce::MouseEvent& e)
{
    // Handle dragging within parent bounds
    dragger.dragComponent(this, e, &constrainer);
}

void PluginNodeComponent::mouseUp(const juce::MouseEvent& e)
{
    isDragging = false;
}
