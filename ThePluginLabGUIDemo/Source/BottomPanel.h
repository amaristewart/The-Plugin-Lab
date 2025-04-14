#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This class represents the educational feedback and visualization area of the application.
*/
class BottomPanel : public juce::Component
{
public:
    //==============================================================================
    BottomPanel();
    ~BottomPanel() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BottomPanel)
};