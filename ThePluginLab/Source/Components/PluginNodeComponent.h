#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

// Forward declarations
class AudioConnectionPoint;

/**
 * Base class for all plugin node components displayed on the canvas
 */
class PluginNodeComponent : public juce::Component
{
public:
    PluginNodeComponent(const juce::String& name, const juce::Colour& color);
    virtual ~PluginNodeComponent() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    
    // Type and processor accessors - pure virtual methods
    virtual NodeType getType() const = 0;
    virtual juce::AudioProcessor* getProcessor() = 0;
    
    // Connection points - pure virtual methods
    virtual void addInputPort(const juce::String& name) = 0;
    virtual void addOutputPort(const juce::String& name) = 0;
    
    // Get port accessors - pure virtual methods
    virtual AudioConnectionPoint* getInputPort(int index) const = 0;
    virtual AudioConnectionPoint* getOutputPort(int index) const = 0;
    
    // Get node properties
    juce::String getName() const { return nodeName; }
    juce::Colour getColour() const { return nodeColor; }
    void setColour(const juce::Colour& color) { nodeColor = color; repaint(); }

    // Node ID method
    int getNodeId() const 
    {
        // Generate a simple ID based on the component's pointer value
        return static_cast<int>(reinterpret_cast<std::uintptr_t>(this) & 0xFFFF);
    }
    
protected:
    // Node properties
    juce::String nodeName;
    juce::Colour nodeColor;
    
    // Dragging behavior
    juce::ComponentDragger dragger;
    juce::ComponentBoundsConstrainer constrainer;
    bool isDragging = false;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginNodeComponent)
};
