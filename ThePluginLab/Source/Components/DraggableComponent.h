#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

/**
 * A component that can be dragged and dropped to create nodes on the canvas
 */
class DraggableComponent : public juce::Component
{
public:
    DraggableComponent(const juce::String& name, const juce::Colour& color, ComponentType type)
        : componentName(name), backgroundColor(color), componentType(type)
    {
        setSize(100, 70);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Draw background
        g.setColour(backgroundColor);
        g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 6.0f);
        
        // Draw border
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 6.0f, 1.0f);
        
        // Draw component name
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText(componentName, getLocalBounds().reduced(5), juce::Justification::centred, true);
        
        // Draw icon based on component type
        drawComponentIcon(g);
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        // Start drag and drop operation
        juce::DragAndDropContainer* dragContainer = 
            juce::DragAndDropContainer::findParentDragContainerFor(this);
            
        if (dragContainer != nullptr)
        {
            // Create a thumbnail for dragging
            juce::Image dragImage(juce::Image::ARGB, getWidth(), getHeight(), true);
            juce::Graphics g(dragImage);
            paint(g); // Paint this component to the image
            
            // Start the drag with component type as string description
            dragContainer->startDragging(juce::String(static_cast<int>(componentType)), 
                                       this, 
                                       dragImage, 
                                       true);
            
            // Log for debugging
            juce::Logger::writeToLog("Started dragging component: " + componentName);
        }
    }
    
    ComponentType getComponentType() const { return componentType; }
    
private:
    void drawComponentIcon(juce::Graphics& g)
    {
        auto iconBounds = getLocalBounds().reduced(25, 25);
        
        switch (componentType)
        {
            case ComponentType::Knob:
                g.setColour(juce::Colours::white);
                g.fillEllipse(iconBounds.toFloat());
                g.setColour(backgroundColor);
                g.fillEllipse(iconBounds.toFloat().reduced(3.0f));
                g.setColour(juce::Colours::white);
                g.drawLine(iconBounds.getCentreX(), iconBounds.getY() + 5, 
                          iconBounds.getCentreX(), iconBounds.getCentre().getY(), 2.0f);
                break;
                
            case ComponentType::Slider:
                g.setColour(juce::Colours::white);
                g.fillRoundedRectangle(iconBounds.toFloat().withHeight(10.0f)
                                     .withCentre(iconBounds.getCentre().toFloat()), 4.0f);
                g.fillEllipse(iconBounds.getRight() - 10.0f, iconBounds.getCentreY() - 5.0f, 10.0f, 10.0f);
                break;
                
            case ComponentType::Button:
                g.setColour(juce::Colours::white);
                g.fillRoundedRectangle(iconBounds.toFloat(), 4.0f);
                g.setColour(backgroundColor);
                g.fillRoundedRectangle(iconBounds.toFloat().reduced(2.0f), 3.0f);
                break;
                
            case ComponentType::Label:
                g.setColour(juce::Colours::white);
                g.setFont(12.0f);
                g.drawText("Abc", iconBounds, juce::Justification::centred, true);
                break;
                
            // Add more component icons as needed
            
            default:
                break;
        }
    }
    
    juce::String componentName;
    juce::Colour backgroundColor;
    ComponentType componentType;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableComponent)
};
