#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"

class DraggableComponent : public juce::Component,
                          public juce::DragAndDropContainer,
                          public juce::Timer,
                          public juce::TooltipClient
{
public:
    DraggableComponent(const juce::String& name, const juce::Colour& color, ComponentType type)
        : componentName(name), componentColor(color), componentType(type)
    {
        setName(name);
        tooltipText = getTooltipForType(componentType);
        
        // Make sure tooltip display doesn't cause layout issues
        setSize(90, 90);
        
        // Start animation timer
        startTimerHz(60);
    }
    
    // Override the getTooltip method from TooltipClient
    juce::String getTooltip() override
    {
        return tooltipText;
    }
    
    void paint(juce::Graphics& g) override
    {
        // Use a smaller bounds to ensure no clipping
        auto bounds = getLocalBounds().toFloat().reduced(5.0f); 
        float iconSize = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.50f;
        
        // Draw component background with rounded corners
        g.setColour(componentColor);
        g.fillRoundedRectangle(bounds, 6.0f);
        
        // Draw border
        g.setColour(isMouseOver() ? juce::Colours::white : componentColor.brighter());
        g.drawRoundedRectangle(bounds, 6.0f, 1.5f);
        
        // Draw icon based on component type - position it higher in the component
        float yOffset = -8; // Move icon up slightly to make more room for text
        drawComponentIcon(g, bounds.getCentreX() - iconSize / 2, 
                         bounds.getCentreY() - iconSize / 2 + yOffset,
                         iconSize, componentType);
        
        // Draw name - adjust font size based on name length and component width
        g.setColour(juce::Colours::white);
        float fontSize = componentName.length() > 8 ? 10.0f : 
                        (getWidth() < 80 ? 9.0f : 11.0f);
        g.setFont(fontSize);
        
        // Calculate text area height based on component size
        float textAreaHeight = bounds.getHeight() * 0.35f;
        auto textArea = bounds.removeFromBottom(textAreaHeight).translated(0, 8); // Move text down
        
        // Draw text at bottom, possibly over two lines if needed
        if (componentName.length() > 8 && getWidth() < 100)
        {
            auto words = juce::StringArray::fromTokens(componentName, " ", "");
            juce::String line1, line2;
            
            if (words.size() == 1)
            {
                // For a single long word
                line1 = componentName;
            }
            else
            {
                // Split into two lines
                int halfPoint = words.size() / 2;
                for (int i = 0; i < words.size(); ++i)
                {
                    if (i < halfPoint)
                        line1 += words[i] + " ";
                    else
                        line2 += words[i] + " ";
                }
                line1 = line1.trim();
                line2 = line2.trim();
            }
            
            // Calculate line heights
            float lineHeight = textAreaHeight / 2.0f;
            
            // Draw two lines of text
            g.drawText(line1, textArea.withHeight(lineHeight), 
                      juce::Justification::centred, true);
            if (!line2.isEmpty())
                g.drawText(line2, textArea.translated(0, lineHeight).withHeight(lineHeight), 
                          juce::Justification::centred, true);
        }
        else
        {
            // Draw single line text
            g.drawText(componentName, textArea, juce::Justification::centred, true);
        }
    }
    
    void mouseEnter(const juce::MouseEvent& e) override
    {
        isHovered = true;
        repaint();
    }
    
    void mouseExit(const juce::MouseEvent& e) override
    {
        isHovered = false;
        repaint();
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        if (e.mods.isLeftButtonDown())
        {
            // Create drag data
            auto* obj = new juce::DynamicObject();
            obj->setProperty("type", static_cast<int>(componentType));
            obj->setProperty("name", componentName);
            
            // Create drag image
            juce::Image dragImage(juce::Image::ARGB, 100, 100, true);
            juce::Graphics g(dragImage);
            drawComponentIcon(g, 25, 25, 50, componentType);
            g.setColour(componentColor.withAlpha(0.8f));
            g.fillRoundedRectangle(10, 10, 80, 80, 5);
            
            // Start drag
            startDragging(componentName, this, dragImage, true, nullptr, &e.source);
        }
    }
    
    bool isMouseOver() const
    {
        return isHovered;
    }
    
    void timerCallback() override
    {
        // Animation logic can be added here
    }

private:
    juce::String getTooltipForType(ComponentType type)
    {
        switch (type)
        {
            case ComponentType::Knob: 
                return "A rotary control for adjusting parameters with a circular motion.";
            case ComponentType::Slider:
                return "A linear control for precise adjustment of parameters.";
            case ComponentType::Button: 
                return "A toggle button to turn features on or off.";
            case ComponentType::Label: 
                return "Text display for naming sections or showing values.";
            case ComponentType::EQLowpass:
                return "Removes frequencies above a cutoff point, letting low frequencies pass through.";
            case ComponentType::EQHighpass:
                return "Removes frequencies below a cutoff point, letting high frequencies pass through.";
            case ComponentType::EQBandpass:
                return "Allows only frequencies within a specified range to pass through.";
            case ComponentType::EQNotch:
                return "Rejects frequencies within a specified range, creating a 'notch' in the spectrum.";
            case ComponentType::EQPeaking:
                return "Boosts or cuts frequencies around a center point, creating a peak or dip.";
            case ComponentType::EQFrequency:
                return "Controls which frequency the filter affects.";
            case ComponentType::EQGain:
                return "Controls how much boost or cut to apply to the affected frequencies.";
            case ComponentType::EQQ:
                return "Controls the filter's width or resonance - how narrow or wide the effect is.";
            case ComponentType::DynamicType:
                return "Choose between compression, limiting, expansion, or gating.";
            case ComponentType::CompThreshold:
                return "Sets the level at which the compressor starts working.";
            case ComponentType::CompRatio:
                return "Sets how aggressively the compressor reduces the signal above threshold.";
            case ComponentType::CompAttack: 
                return "Controls how quickly the compressor responds to signals above threshold.";
            case ComponentType::CompRelease: 
                return "Controls how quickly the compressor stops working when signals fall below threshold.";
            case ComponentType::Bypass:
                return "Toggles an effect on or off for comparison.";
            default:
                return {};
        }
    }
    
    void drawComponentIcon(juce::Graphics& g, float x, float y, float size, ComponentType type)
    {
        g.setColour(juce::Colours::white);
        
        switch (type)
        {
            case ComponentType::Knob:
                g.fillEllipse(x, y, size, size);
                g.setColour(componentColor);
                g.fillEllipse(x + size * 0.2f, y + size * 0.2f, size * 0.6f, size * 0.6f);
                g.setColour(juce::Colours::white);
                g.drawLine(x + size * 0.5f, y + size * 0.2f, x + size * 0.5f, y + size * 0.3f, 2.0f);
                break;
                
            case ComponentType::Slider:
                g.fillRoundedRectangle(x + size * 0.35f, y, size * 0.3f, size, 3.0f);
                g.setColour(componentColor);
                g.fillEllipse(x + size * 0.25f, y + size * 0.4f, size * 0.5f, size * 0.2f);
                break;
                
            case ComponentType::Button:
                g.drawRoundedRectangle(x, y, size, size * 0.5f, 5.0f, 2.0f);
                g.setColour(componentColor);
                g.fillRoundedRectangle(x + 4, y + 4, size - 8, size * 0.5f - 8, 3.0f);
                break;
                
            case ComponentType::Label:
                g.drawRoundedRectangle(x, y, size, size * 0.4f, 3.0f, 2.0f);
                g.setFont(size * 0.25f);
                g.drawText("Text", x, y, size, size * 0.4f, juce::Justification::centred);
                break;
            
            // Draw EQ-related icons
            case ComponentType::EQLowpass:
            case ComponentType::EQHighpass:
            case ComponentType::EQBandpass:
            case ComponentType::EQNotch:
            case ComponentType::EQPeaking:
                drawFilterResponseCurve(g, x, y, size, type);
                break;
            
            // Draw dynamics-related icons  
            case ComponentType::DynamicType:
            case ComponentType::CompThreshold:
            case ComponentType::CompRatio:
            case ComponentType::CompAttack:
            case ComponentType::CompRelease:
                drawDynamicsIcon(g, x, y, size, type);
                break;
                
            default:
                g.setFont(size * 0.5f);
                g.drawText("?", x, y, size, size, juce::Justification::centred);
                break;
        }
    }
    
    void drawFilterResponseCurve(juce::Graphics& g, float x, float y, float size, ComponentType type)
    {
        juce::Path path;
        path.startNewSubPath(x, y + size / 2);
        
        switch (type)
        {
            case ComponentType::EQLowpass:
                path.startNewSubPath(x, y + size * 0.8f);
                path.quadraticTo(x + size * 0.5f, y + size * 0.8f, x + size * 0.7f, y + size * 0.5f);
                path.lineTo(x + size, y + size * 0.2f);
                break;
                
            case ComponentType::EQHighpass:
                path.startNewSubPath(x, y + size * 0.2f);
                path.lineTo(x + size * 0.3f, y + size * 0.5f);
                path.quadraticTo(x + size * 0.5f, y + size * 0.8f, x + size, y + size * 0.8f);
                break;
                
            case ComponentType::EQBandpass:
                path.startNewSubPath(x, y + size * 0.8f);
                path.lineTo(x + size * 0.3f, y + size * 0.8f);
                path.quadraticTo(x + size * 0.4f, y + size * 0.2f, x + size * 0.6f, y + size * 0.2f);
                path.quadraticTo(x + size * 0.7f, y + size * 0.8f, x + size, y + size * 0.8f);
                break;
                
            case ComponentType::EQNotch:
                path.startNewSubPath(x, y + size * 0.2f);
                path.lineTo(x + size * 0.3f, y + size * 0.2f);
                path.quadraticTo(x + size * 0.4f, y + size * 0.8f, x + size * 0.6f, y + size * 0.8f);
                path.quadraticTo(x + size * 0.7f, y + size * 0.2f, x + size, y + size * 0.2f);
                break;
                
            case ComponentType::EQPeaking:
                path.startNewSubPath(x, y + size * 0.5f);
                path.lineTo(x + size * 0.3f, y + size * 0.5f);
                path.quadraticTo(x + size * 0.4f, y + size * 0.2f, x + size * 0.5f, y + size * 0.2f);
                path.quadraticTo(x + size * 0.6f, y + size * 0.2f, x + size * 0.7f, y + size * 0.5f);
                path.lineTo(x + size, y + size * 0.5f);
                break;
                
            default:
                break;
        }
        
        g.strokePath(path, juce::PathStrokeType(2.0f));
    }
    
    void drawDynamicsIcon(juce::Graphics& g, float x, float y, float size, ComponentType type)
    {
        switch (type)
        {
            case ComponentType::DynamicType:
            {
                // Draw a simplified compressor transfer function
                juce::Path path;
                path.startNewSubPath(x, y + size);
                path.lineTo(x + size * 0.4f, y + size * 0.6f);
                path.lineTo(x + size, y + size * 0.4f);
                g.strokePath(path, juce::PathStrokeType(2.0f));
                
                // Add a small "type" indicator
                g.setFont(size * 0.2f);
                g.drawText("Type", x, y + size * 0.8f, size, size * 0.2f, juce::Justification::centred);
                break;
            }
            
            case ComponentType::CompThreshold:
            {
                // Draw a threshold line
                g.drawLine(x, y + size * 0.5f, x + size, y + size * 0.5f, 2.0f);
                g.drawArrow({x + size * 0.5f, y, x + size * 0.5f, y + size * 0.45f}, 2.0f, 8.0f, 8.0f);
                g.setFont(size * 0.2f);
                g.drawText("Threshold", x, y + size * 0.8f, size, size * 0.2f, juce::Justification::centred);
                break;
            }
            
            case ComponentType::CompRatio:
            {
                // Draw ratio representation
                juce::Path path;
                path.startNewSubPath(x, y + size);
                path.lineTo(x + size * 0.5f, y + size * 0.5f);
                path.lineTo(x + size, y + size * 0.4f);
                g.strokePath(path, juce::PathStrokeType(2.0f));
                
                g.setFont(size * 0.2f);
                g.drawText("Ratio", x, y + size * 0.8f, size, size * 0.2f, juce::Justification::centred);
                break;
            }
            
            case ComponentType::CompAttack:
            {
                // Draw attack curve
                juce::Path path;
                path.startNewSubPath(x, y + size * 0.8f);
                path.quadraticTo(x + size * 0.3f, y + size * 0.3f, x + size, y + size * 0.2f);
                g.strokePath(path, juce::PathStrokeType(2.0f));
                
                g.setFont(size * 0.2f);
                g.drawText("Attack", x, y + size * 0.8f, size, size * 0.2f, juce::Justification::centred);
                break;
            }
            
            case ComponentType::CompRelease:
            {
                // Draw release curve
                juce::Path path;
                path.startNewSubPath(x, y + size * 0.2f);
                path.quadraticTo(x + size * 0.7f, y + size * 0.4f, x + size, y + size * 0.8f);
                g.strokePath(path, juce::PathStrokeType(2.0f));
                
                g.setFont(size * 0.2f);
                g.drawText("Release", x, y + size * 0.8f, size, size * 0.2f, juce::Justification::centred);
                break;
            }
            
            default:
                break;
        }
    }
    
    juce::String componentName;
    juce::Colour componentColor;
    ComponentType componentType;
    juce::String tooltipText; // Store tooltip text
    bool isHovered = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableComponent)
};
