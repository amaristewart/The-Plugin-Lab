#pragma once
#include <JuceHeader.h>

/**
 * Helper class to safely manage animation frames and display links
 * This helps prevent crashes related to DisplayLink destruction
 */
class AnimationHelper : public juce::Timer
{
public:
    AnimationHelper() 
    {
        // Use timer-based animation instead of DisplayLink for safety
        startTimerHz(60);
    }
    
    ~AnimationHelper() override
    {
        // Stop timer before destruction
        stopTimer();
    }
    
    void timerCallback() override
    {
        if (onAnimationFrame)
            onAnimationFrame();
    }
    
    // Set callback to be called on animation frame
    void setAnimationCallback(std::function<void()> callback)
    {
        onAnimationFrame = std::move(callback);
    }
    
    // Start/stop animation
    void startAnimation() { startTimerHz(60); }
    void stopAnimation() { stopTimer(); }
    
private:
    std::function<void()> onAnimationFrame;
};
