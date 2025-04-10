#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"
#include "../Common/Features.h"

/**
 * Tool palette with component creation tools
 */
class ToolPalette : public juce::Component
{
public:
    ToolPalette();
    ~ToolPalette() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Set active tool
    void setActiveTool(ComponentType toolType);
    
    // Tool selection callback
    std::function<void(ComponentType)> onToolSelected;
    
private:
    class ToolButton : public juce::DrawableButton
    {
    public:
        ToolButton(const juce::String& name, ComponentType type, const juce::Colour& color);
        ComponentType getType() const { return toolType; }
        
    private:
        ComponentType toolType;
        juce::Colour buttonColor;
    };
    
    void addToolButton(const juce::String& name, ComponentType type, const juce::Colour& color);
    void createToolButtons();
    
    juce::OwnedArray<ToolButton> toolButtons;
    ComponentType activeToolType;
    
    // Fix Label constructor by using proper initialization
    juce::Label guiSectionLabel;
    juce::Label eqSectionLabel;
    juce::Label dynamicsSectionLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolPalette)
};
