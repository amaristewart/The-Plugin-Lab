#pragma once

#include <JuceHeader.h>
#include "../Audio/Processors/EQProcessor.h"

/**
 * GUI node representing an equalizer effect in The Plugin Lab
 * This is a simplified prototype focused on the user interface
 */
class EqualizerNode : public juce::Component,
                     public juce::Slider::Listener,
                     public juce::ComboBox::Listener,
                     public juce::Button::Listener
{
public:
    EqualizerNode();
    ~EqualizerNode() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Listener implementations
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void buttonClicked(juce::Button* button) override;
    
    // EQ parameter setters/getters
    void setType(int bandIndex, EQProcessor::FilterType type);
    void setFrequency(int bandIndex, float frequency);
    void setGain(int bandIndex, float gain);
    void setQ(int bandIndex, float q);
    
    float getFrequency(int bandIndex) const;
    float getGain(int bandIndex) const;
    float getQ(int bandIndex) const;
    
    // Selected band management
    void setSelectedBand(int bandIndex);
    int getSelectedBand() const { return selectedBand; }
    
private:
    // UI components
    juce::Slider frequencySlider;
    juce::Slider gainSlider;
    juce::Slider qSlider;
    juce::ComboBox typeSelector;
    juce::TextButton helpButton;
    juce::OwnedArray<juce::TextButton> bandButtons;
    
    // EQ parameters
    int selectedBand = 0;
    EQProcessor eqProcessor;
    
    // Helper methods
    void updateFilter();
    void updateControls();
    void showHelpPopup();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizerNode)
};
