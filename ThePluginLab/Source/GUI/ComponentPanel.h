#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"
#include "../Components/DraggableComponent.h"
#include "../Common/Features.h"

class DraggableComponent;

class ComponentPanel : public juce::Component,
                     public juce::DragAndDropContainer
{
public:
    ComponentPanel();
    ~ComponentPanel() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Add components to the panel
    void addDraggableComponent(const juce::String& name, const juce::Colour& color, ComponentType type);
    
private:
    // Component for category headers
    class CategoryHeader : public juce::Component
    {
    public:
        CategoryHeader(const juce::String& text, const juce::Colour& color);
        void paint(juce::Graphics& g) override;
        
    private:
        juce::String title;
        juce::Colour backgroundColor;
    };
    
    void addCategoryHeader(const juce::String& title, const juce::Colour& color);
    
    juce::OwnedArray<DraggableComponent> draggableComponents;
    juce::OwnedArray<CategoryHeader> categoryHeaders;
    
    // Add a viewport to enable scrolling
    std::unique_ptr<juce::Viewport> viewport;
    std::unique_ptr<juce::Component> contentComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComponentPanel)
};
