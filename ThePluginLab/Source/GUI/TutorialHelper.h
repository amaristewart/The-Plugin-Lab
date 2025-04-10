#pragma once
#include <JuceHeader.h>

/**
 * Helper class to show tutorial overlays for new users
 */
class TutorialHelper : public juce::Component
{
public:
    TutorialHelper()
    {
        setInterceptsMouseClicks(false, false);
        startTimer(100); // Check periodically for tutorial needs
    }
    
    void paint(juce::Graphics& g) override
    {
        if (!isVisible())
            return;
            
        if (shouldShowComponentPanelTip)
            drawComponentPanelTip(g);
            
        if (shouldShowConnectionTip)
            drawConnectionTip(g);
            
        if (shouldShowParametersTip)
            drawParametersTip(g);
    }
    
    void timerCallback()
    {
        checkTutorialState();
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        // Dismiss current tip
        if (shouldShowComponentPanelTip)
            shouldShowComponentPanelTip = false;
        else if (shouldShowConnectionTip)
            shouldShowConnectionTip = false;
        else if (shouldShowParametersTip)
            shouldShowParametersTip = false;
            
        repaint();
    }
    
private:
    bool shouldShowComponentPanelTip = true;
    bool shouldShowConnectionTip = false;
    bool shouldShowParametersTip = false;
    
    void checkTutorialState()
    {
        auto* parent = getParentComponent();
        if (parent == nullptr)
            return;
            
        // Check for nodes on canvas
        int nodeCount = 0;
        int connectionCount = 0;
        
        for (auto* child : parent->getChildren())
        {
            if (dynamic_cast<PluginNodeComponent*>(child) != nullptr)
                nodeCount++;
                
            if (dynamic_cast<ConnectionComponent*>(child) != nullptr)
                connectionCount++;
        }
        
        // Show connection tip after first node is added
        if (nodeCount > 0 && connectionCount == 0)
        {
            shouldShowComponentPanelTip = false;
            shouldShowConnectionTip = true;
            shouldShowParametersTip = false;
        }
        // Show parameters tip after connection is made
        else if (nodeCount > 1 && connectionCount > 0)
        {
            shouldShowComponentPanelTip = false;
            shouldShowConnectionTip = false;
            shouldShowParametersTip = true;
        }
        
        repaint();
    }
    
    void drawComponentPanelTip(juce::Graphics& g)
    {
        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.fillAll();
        
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(20.0f, juce::Font::bold));
        g.drawText("Step 1: Drag components from the panel to the canvas", 
                 getLocalBounds().reduced(100, 200),
                 juce::Justification::centred, true);
                 
        g.setFont(juce::Font(16.0f));
        g.drawText("Click anywhere to dismiss this tip", 
                 getLocalBounds().removeFromBottom(100),
                 juce::Justification::centredTop, true);
    }
    
    void drawConnectionTip(juce::Graphics& g)
    {
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.