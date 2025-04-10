#pragma once
#include <JuceHeader.h>
#include "../GUI/AnimationHelper.h"

/**
 * Base class for all visualizers with safe animation handling
 */
class BaseVisualizer : public juce::Component
{
public:
    BaseVisualizer()
    {
        // Set up animation
        animationHelper.setAnimationCallback([this]() { repaint(); });
    }
    
    ~BaseVisualizer() override
    {
        // Stop animations before destruction
        stopAnimations();
    }
    
    // Animation control
    void startAnimations() { animationHelper.startAnimation(); }
    void stopAnimations() { animationHelper.stopAnimation(); }
    
protected:
    AnimationHelper animationHelper;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseVisualizer)
};
