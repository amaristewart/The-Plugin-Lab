#include "PluginToolbar.h"

PluginToolbar::PluginToolbar()
{
    // Logo
    logo = juce::ImageCache::getFromMemory(BinaryData::PluginLabLogo_png,
                                         BinaryData::PluginLabLogo_pngSize);
    
    // Menu bar model and component
    menuModel = std::make_unique<ToolbarMenuModel>(*this);
    menuBar = std::make_unique<juce::MenuBarComponent>(menuModel.get());
    addAndMakeVisible(menuBar.get());
    
    setSize(800, 40);
}

void PluginToolbar::paint(juce::Graphics& g)
{
    // Background gradient
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(0xFF61DAFB),  // Light blue
        0.0f, 0.0f,
        juce::Colour(0xFFFF6B6B),  // Pink
        (float)getWidth(), 0.0f,
        false));
    g.fillAll();
    
    // Logo
    if (logo.isValid())
    {
        g.drawImage(logo, 5, 5, 30, 30, 0, 0, logo.getWidth(), logo.getHeight());
    }
    
    // Border
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRect(getLocalBounds());
}

void PluginToolbar::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromLeft(40); // Space for logo
    menuBar->setBounds(bounds);
}

void PluginToolbar::addPluginTemplate(const juce::String& name, NodeType type)
{
    auto* button = new PluginButton(name, type);
    addAndMakeVisible(button);
    
    button->onClick = [this, button]()
    {
        if (onPluginSelected)
            onPluginSelected(button->getType());
    };
    
    button->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    button->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    
    button->addMouseListener(this, false);
    
    toolbarButtons.add(button);
}

NodeType PluginToolbar::getNodeTypeFromName(const juce::String& name)
{
    if (name == "Compressor") return NodeType::Compressor;
    if (name == "Equalizer") return NodeType::Equalizer;
    return NodeType::Compressor; 
}
