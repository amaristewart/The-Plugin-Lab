#include "ComponentPanel.h"

ComponentPanel::ComponentPanel()
{
    setOpaque(true);
    
    // Create content component for the viewport
    contentComponent = std::make_unique<juce::Component>();
    contentComponent->setOpaque(true);
    
    // Create viewport for scrolling
    viewport = std::make_unique<juce::Viewport>();
    viewport->setViewedComponent(contentComponent.get(), false);
    viewport->setScrollBarsShown(true, false);
    addAndMakeVisible(viewport.get());
    
    // Create category headers
    addCategoryHeader("GUI Components", Features::guiColor);
    
    // Add GUI components
    addDraggableComponent("Knob", Features::guiColor, ComponentType::Knob);
    addDraggableComponent("Slider", Features::guiColor, ComponentType::Slider);
    addDraggableComponent("Button", Features::guiColor, ComponentType::Button);
    addDraggableComponent("Label", Features::guiColor, ComponentType::Label);
    
    // Add EQ components
    addCategoryHeader("EQ Components", Features::eqColor);
    addDraggableComponent("Lowpass", Features::eqColor, ComponentType::EQLowpass);
    addDraggableComponent("Highpass", Features::eqColor, ComponentType::EQHighpass);
    addDraggableComponent("Bandpass", Features::eqColor, ComponentType::EQBandpass);
    addDraggableComponent("Notch", Features::eqColor, ComponentType::EQNotch);
    addDraggableComponent("Peaking", Features::eqColor, ComponentType::EQPeaking);
    
    // Add compressor components
    addCategoryHeader("Dynamics", Features::compColor);
    addDraggableComponent("Compressor", Features::compColor, ComponentType::DynamicType);
    addDraggableComponent("Threshold", Features::compColor, ComponentType::CompThreshold);
    addDraggableComponent("Ratio", Features::compColor, ComponentType::CompRatio);
    addDraggableComponent("Attack", Features::compColor, ComponentType::CompAttack);
    addDraggableComponent("Release", Features::compColor, ComponentType::CompRelease);
    
    // Enable drag and drop container
    setSize(Features::componentPanelWidth, 800);
}

ComponentPanel::~ComponentPanel()
{
    // Clean up resources
    draggableComponents.clear();
}

void ComponentPanel::paint(juce::Graphics& g)
{
    // Draw background
    g.fillAll(juce::Colour(0xFF202020));
    
    // Draw border
    g.setColour(juce::Colours::darkgrey);
    g.drawRect(getLocalBounds(), 1);
}

void ComponentPanel::resized()
{
    // Set viewport to fill the entire component
    viewport->setBounds(getLocalBounds());
    
    // Layout components in the content component
    auto bounds = contentComponent->getLocalBounds().reduced(5);
    
    // Position each component
    int y = 10;
    
    for (auto* header : categoryHeaders)
    {
        // Position category headers
        header->setBounds(bounds.getX(), y, bounds.getWidth(), Features::categoryHeaderHeight);
        y += Features::categoryHeaderHeight + 5;
        
        // Find components for this category
        juce::Colour headerColor = header->findColour(juce::Label::backgroundColourId);
        int componentsInCategory = 0;
        
        for (auto* draggable : draggableComponents)
        {
            if (draggable->findColour(juce::Label::backgroundColourId) == headerColor)
            {
                // Position draggable components in a grid
                int row = componentsInCategory % 2;
                int column = componentsInCategory / 2;
                
                int itemWidth = (bounds.getWidth() - 10) / 2;
                int itemHeight = 70;
                
                draggable->setBounds(bounds.getX() + row * (itemWidth + 10), 
                                   y + column * (itemHeight + 10), 
                                   itemWidth, itemHeight);
                
                componentsInCategory++;
            }
        }
        
        // Move to next section
        y += ((componentsInCategory + 1) / 2) * (70 + 10);
        y += 20; // Extra space between categories
    }
    
    // Set content component height based on the last component
    contentComponent->setSize(bounds.getWidth(), y);
    
    juce::Logger::writeToLog("ComponentPanel resized. Content height: " + juce::String(y));
}

void ComponentPanel::addDraggableComponent(const juce::String& name, const juce::Colour& color, ComponentType type)
{
    auto* component = new DraggableComponent(name, color, type);
    contentComponent->addAndMakeVisible(component);
    draggableComponents.add(component);
    
    // Store the color as a property for category grouping
    component->setColour(juce::Label::backgroundColourId, color);
}

void ComponentPanel::addCategoryHeader(const juce::String& title, const juce::Colour& color)
{
    auto* header = new CategoryHeader(title, color);
    contentComponent->addAndMakeVisible(header);
    categoryHeaders.add(header);
    
    // Store the color as a property for category grouping
    header->setColour(juce::Label::backgroundColourId, color);
}

ComponentPanel::CategoryHeader::CategoryHeader(const juce::String& text, const juce::Colour& color)
    : title(text), backgroundColor(color)
{
    setSize(200, Features::categoryHeaderHeight);
}

void ComponentPanel::CategoryHeader::paint(juce::Graphics& g)
{
    g.setColour(backgroundColor);
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 4.0f);
    
    g.setColour(juce::Colours::white);
    // Fix deprecated Font constructor
    juce::Font textFont(juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::bold);
    g.setFont(textFont);
    g.drawText(title, getLocalBounds().reduced(5, 0), juce::Justification::centredLeft, true);
}
