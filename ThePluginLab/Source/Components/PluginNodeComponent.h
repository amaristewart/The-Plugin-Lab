#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

class AudioConnectionPoint;

class PluginNodeComponent : public juce::Component,
                          public juce::DragAndDropTarget,
                          public juce::Timer
{
public:
    PluginNodeComponent(const juce::String& name, const juce::Colour& color);
    virtual ~PluginNodeComponent() = default;
    
    virtual NodeType getType() const = 0;
    virtual juce::AudioProcessor* getProcessor() = 0;
    virtual void paint(juce::Graphics&) override;
    virtual void resized() override;
    
    // Mouse handling
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    
    // Port and parameter management
    virtual void addInputPort(const juce::String& name);
    virtual void addOutputPort(const juce::String& name);
    virtual void addParameter(const juce::String& name, float min, float max, float defaultValue);
    void layoutParameters();
    
    // Position getters
    juce::Point<float> getInputPosition(int index) const;
    juce::Point<float> getOutputPosition(int index) const;
    
    // Connection management
    void createConnectionPoints();
    void addConnectionPoint(AudioConnectionPoint* point);
    void removeConnectionPoint(AudioConnectionPoint* point);
    
    // DragAndDropTarget implementation
    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& details) override;
    void itemDropped(const juce::DragAndDropTarget::SourceDetails& details) override;
    
protected:
    juce::String nodeName;
    juce::Colour nodeColor;
    std::unique_ptr<juce::AudioProcessor> processor;
    juce::OwnedArray<juce::Slider> parameterSliders;
    juce::Array<AudioConnectionPoint*> connectionPoints;
    
    juce::ComponentDragger dragger;
    juce::ComponentBoundsConstrainer constrainer;
    bool isDragging = false;
    
    void layoutControls();
    
    // Add meter update method
    virtual void updateMeters() {}
    
    // Update timer
    void startMeterUpdates() 
    { 
        startTimerHz(30); 
    }
    
    void timerCallback() override
    {
        updateMeters();
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginNodeComponent)
};
