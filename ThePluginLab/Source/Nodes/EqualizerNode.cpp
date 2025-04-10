#include "EqualizerNode.h"

EqualizerNode::EqualizerNode()
{
    // Set up sliders
    frequencySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    frequencySlider.setRange(20.0, 20000.0, 1.0);
    frequencySlider.setSkewFactorFromMidPoint(1000.0); // Logarithmic frequency control
    frequencySlider.setValue(1000.0);
    frequencySlider.addListener(this);
    frequencySlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(frequencySlider);
    
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    gainSlider.setRange(-24.0, 24.0, 0.1);
    gainSlider.setValue(0.0);
    gainSlider.addListener(this);
    gainSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider);
    
    qSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    qSlider.setRange(0.1, 10.0, 0.01);
    qSlider.setValue(0.707);
    qSlider.addListener(this);
    qSlider.setSkewFactorFromMidPoint(1.0);
    addAndMakeVisible(qSlider);
    
    // Set up filter type selector
    typeSelector.addItem("Low Pass", EQProcessor::LowPass + 1);
    typeSelector.addItem("High Pass", EQProcessor::HighPass + 1);
    typeSelector.addItem("Low Shelf", EQProcessor::LowShelf + 1);
    typeSelector.addItem("High Shelf", EQProcessor::HighShelf + 1);
    typeSelector.addItem("Band Pass", EQProcessor::BandPass + 1);
    typeSelector.addItem("Notch", EQProcessor::Notch + 1);
    typeSelector.addItem("Peak", EQProcessor::Peak + 1);
    typeSelector.setSelectedId(EQProcessor::Peak + 1);
    typeSelector.addListener(this);
    addAndMakeVisible(typeSelector);
    
    // Set up help button
    helpButton.setButtonText("?");
    helpButton.addListener(this);
    addAndMakeVisible(helpButton);
    
    // Set up band selector buttons
    for (int i = 0; i < 5; ++i) {
        auto button = std::make_unique<juce::TextButton>("Band " + juce::String(i + 1));
        button->setClickingTogglesState(true);
        button->setRadioGroupId(1);
        button->addListener(this);
        button->setToggleState(i == 0, juce::dontSendNotification);
        addAndMakeVisible(*button);
        bandButtons.add(std::move(button));
    }
    
    // Set size
    setSize(500, 300);
    
    // Initialize with default values
    selectedBand = 0;
    updateControls();
}

EqualizerNode::~EqualizerNode()
{
    // No cleanup needed
}

void EqualizerNode::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colours::darkgrey);
    
    // Draw EQ curve - simplified for prototype
    auto responseArea = juce::Rectangle<int>(20, 20, getWidth() - 40, 100);
    g.setColour(juce::Colours::black);
    g.fillRect(responseArea);
    g.setColour(juce::Colours::white);
    g.drawRect(responseArea);
    
    // Draw a simple frequency response curve (just for visualization)
    g.setColour(juce::Colours::orange);
    juce::Path responseCurve;
    
    float startX = responseArea.getX();
    float endX = responseArea.getRight();
    float midY = responseArea.getCentreY();
    float height = responseArea.getHeight() * 0.8f;
    
    responseCurve.startNewSubPath(startX, midY);
    
    // Draw different curve based on selected filter type
    EQProcessor::FilterType type = eqProcessor.getFilterType(selectedBand);
    float freq = eqProcessor.getFrequency(selectedBand);
    float gain = eqProcessor.getGain(selectedBand);
    float q = eqProcessor.getQ(selectedBand);
    
    // Normalize frequency to x position
    float normFreq = (std::log10(freq) - std::log10(20.0f)) / (std::log10(20000.0f) - std::log10(20.0f));
    float freqX = startX + normFreq * responseArea.getWidth();
    
    // Just visual representation - not accurate filter response
    switch (type) {
        case EQProcessor::LowPass:
            responseCurve.quadraticTo(freqX, midY, endX, midY + height/2);
            break;
        case EQProcessor::HighPass:
            responseCurve.quadraticTo(freqX, midY, startX, midY + height/2);
            break;
        case EQProcessor::Peak:
        {
            float peakHeight = -gain * height / 48.0f; // normalize gain to height
            responseCurve.quadraticTo(freqX - responseArea.getWidth() * 0.1f, midY,
                                    freqX, midY + peakHeight);
            responseCurve.quadraticTo(freqX + responseArea.getWidth() * 0.1f, midY,
                                    endX, midY);
            break;
        }
        default:
            // Simple line for other types
            responseCurve.lineTo(endX, midY);
            break;
    }
    
    g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
    
    // Draw labels
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Frequency", frequencySlider.getX(), frequencySlider.getBottom(), frequencySlider.getWidth(), 20, juce::Justification::centred);
    g.drawText("Gain", gainSlider.getX(), gainSlider.getBottom(), gainSlider.getWidth(), 20, juce::Justification::centred);
    g.drawText("Q", qSlider.getX(), qSlider.getBottom(), qSlider.getWidth(), 20, juce::Justification::centred);
    g.drawText("Filter Type", typeSelector.getX(), typeSelector.getY() - 20, typeSelector.getWidth(), 20, juce::Justification::centred);
    
    // Title
    g.setFont(22.0f);
    g.drawText("Equalizer", 0, 0, getWidth(), 20, juce::Justification::centred);
    
    // Selected band indicator
    g.setFont(18.0f);
    g.drawText("Band: " + juce::String(selectedBand + 1), getWidth() - 100, 0, 100, 20, juce::Justification::centred);
}

void EqualizerNode::resized()
{
    // Position controls
    auto area = getLocalBounds().reduced(10);
    
    // Response area at top
    auto responseArea = area.removeFromTop(120);
    
    // Band buttons at bottom
    auto buttonArea = area.removeFromBottom(40);
    int buttonWidth = buttonArea.getWidth() / bandButtons.size();
    for (int i = 0; i < bandButtons.size(); ++i) {
        bandButtons[i]->setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(4));
    }
    
    // Position sliders and selector
    area.removeFromBottom(10); // spacing
    
    int controlWidth = area.getWidth() / 3;
    frequencySlider.setBounds(area.removeFromLeft(controlWidth).reduced(5));
    gainSlider.setBounds(area.removeFromLeft(controlWidth).reduced(5));
    qSlider.setBounds(area.removeFromLeft(controlWidth).reduced(5));
    
    typeSelector.setBounds(responseArea.getX() + 10, responseArea.getBottom() - 30, 
                          responseArea.getWidth() - 20, 24);
    helpButton.setBounds(getWidth() - 30, 10, 20, 20);
}

void EqualizerNode::setType(int bandIndex, EQProcessor::FilterType type)
{
    if (bandIndex >= 0 && bandIndex < eqProcessor.getNumBands()) {
        eqProcessor.setFilterType(bandIndex, type);
        updateFilter();
    }
}

void EqualizerNode::setFrequency(int bandIndex, float frequency)
{
    if (bandIndex >= 0 && bandIndex < eqProcessor.getNumBands()) {
        eqProcessor.setFrequency(bandIndex, frequency);
        updateFilter();
    }
}

void EqualizerNode::setGain(int bandIndex, float gain)
{
    if (bandIndex >= 0 && bandIndex < eqProcessor.getNumBands()) {
        eqProcessor.setGain(bandIndex, gain);
        updateFilter();
    }
}

void EqualizerNode::setQ(int bandIndex, float q)
{
    if (bandIndex >= 0 && bandIndex < eqProcessor.getNumBands()) {
        eqProcessor.setQ(bandIndex, q);
        updateFilter();
    }
}

float EqualizerNode::getFrequency(int bandIndex) const
{
    return eqProcessor.getFrequency(bandIndex);
}

float EqualizerNode::getGain(int bandIndex) const
{
    return eqProcessor.getGain(bandIndex);
}

float EqualizerNode::getQ(int bandIndex) const
{
    return eqProcessor.getQ(bandIndex);
}

void EqualizerNode::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &frequencySlider)
        setFrequency(selectedBand, static_cast<float>(frequencySlider.getValue()));
    else if (slider == &gainSlider)
        setGain(selectedBand, static_cast<float>(gainSlider.getValue()));
    else if (slider == &qSlider)
        setQ(selectedBand, static_cast<float>(qSlider.getValue()));
        
    repaint();
}

void EqualizerNode::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &typeSelector)
    {
        int filterTypeId = typeSelector.getSelectedId() - 1;
        setType(selectedBand, static_cast<EQProcessor::FilterType>(filterTypeId));
        repaint();
    }
}

void EqualizerNode::buttonClicked(juce::Button* button)
{
    if (button == &helpButton)
    {
        showHelpPopup();
    }
    else
    {
        // Check if it's one of our band buttons
        for (int i = 0; i < bandButtons.size(); ++i)
        {
            // Fix: Use -> instead of . for pointer access and remove get() call since bandButtons[i] already returns a pointer
            if (button == bandButtons[i] && button->getToggleState())
            {
                setSelectedBand(i);
                break;
            }
        }
    }
}

void EqualizerNode::setSelectedBand(int bandIndex)
{
    if (bandIndex >= 0 && bandIndex < eqProcessor.getNumBands())
    {
        selectedBand = bandIndex;
        updateControls();
        repaint();
    }
}

void EqualizerNode::updateControls()
{
    // Update UI controls to reflect the current band settings
    frequencySlider.setValue(eqProcessor.getFrequency(selectedBand), juce::dontSendNotification);
    gainSlider.setValue(eqProcessor.getGain(selectedBand), juce::dontSendNotification);
    qSlider.setValue(eqProcessor.getQ(selectedBand), juce::dontSendNotification);
    typeSelector.setSelectedId(eqProcessor.getFilterType(selectedBand) + 1, juce::dontSendNotification);
}

void EqualizerNode::updateFilter()
{
    // In a real implementation, this would update the audio processing filter
    // For the prototype, we'll just update the UI
    repaint();
}

void EqualizerNode::showHelpPopup()
{
    // Create and show a popup explaining current filter settings
    EQProcessor::FilterType currentType = eqProcessor.getFilterType(selectedBand);
    juce::String message = "Filter Type: " + eqProcessor.getFilterTypeDescription(currentType) + "\n\n";
    message += eqProcessor.getFrequencyDescription() + "\n\n";
    message += eqProcessor.getQDescription() + "\n\n";
    message += eqProcessor.getGainDescription();
    
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                          "Equalizer Help",
                                          message,
                                          "OK");
}

