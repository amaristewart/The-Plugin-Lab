#include "EqualizerNode.h"
#include "../Audio/Processors/EqualizerProcessor.h"

EqualizerNode::EqualizerNode() 
    : PluginNodeComponent("Equalizer", juce::Colours::orange)
{
    processor = std::make_unique<EqualizerProcessor>();
    visualizer = std::make_unique<EQVisualizer>();
    
    // Initialize UI components
    typeSelector = std::make_unique<juce::ComboBox>();
    typeSelector->addItem("Lowpass", static_cast<int>(EQType::Lowpass) + 1);
    typeSelector->addItem("Highpass", static_cast<int>(EQType::Highpass) + 1);
    typeSelector->addItem("Bandpass", static_cast<int>(EQType::Bandpass) + 1);
    typeSelector->addItem("Notch", static_cast<int>(EQType::Notch) + 1);
    typeSelector->addItem("Peaking", static_cast<int>(EQType::Peaking) + 1);
    typeSelector->setSelectedItemIndex(0);
    addAndMakeVisible(*typeSelector);
    typeSelector->addListener(this);
    
    bypassButton = std::make_unique<juce::ToggleButton>("Bypass");
    addAndMakeVisible(*bypassButton);
    bypassButton->addListener(this);
    
    addAndMakeVisible(*visualizer);
    
    // Add parameters with ranges based on type
    addParameter("Frequency", 20.0f, 20000.0f, 1000.0f);
    addParameter("Gain", -24.0f, 24.0f, 0.0f);
    addParameter("Q", 0.1f, 10.0f, 1.0f);
    
    for (auto* slider : parameterSliders)
        slider->addListener(this);
    
    setSize(200, 200);
    resized();
}

EqualizerNode::~EqualizerNode()
{
    typeSelector->removeListener(this);
    bypassButton->removeListener(this);
    
    for (auto* slider : parameterSliders)
        slider->removeListener(this);
}

void EqualizerNode::resized()
{
    // ... existing resized code ...
}

void EqualizerNode::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == typeSelector.get())
        typeChanged();
}

void EqualizerNode::sliderValueChanged(juce::Slider* slider)
{
    updateProcessor();
    updateMeters();
}

void EqualizerNode::buttonClicked(juce::Button* button)
{
    if (button == bypassButton.get())
        setBypassState(button->getToggleState());
}

void EqualizerNode::setBypassState(bool shouldBeBypassed)
{
    if (auto* proc = dynamic_cast<EqualizerProcessor*>(processor.get()))
    {
        proc->setBypassed(shouldBeBypassed);
        repaint();
    }
}

void EqualizerNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (auto* proc = dynamic_cast<EqualizerProcessor*>(processor.get()))
    {
        proc->processBlock(buffer, midiMessages);
    }
    
    if (visualizer)
    {
        auto* channelData = buffer.getReadPointer(0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            visualizer->pushSample(channelData[i], channelData[i]);
        }
    }
}

void EqualizerNode::typeChanged()
{
    if (auto* proc = dynamic_cast<EqualizerProcessor*>(processor.get()))
    {
        auto type = static_cast<EQType>(typeSelector->getSelectedId() - 1);
        proc->setFilterType(type);
        updateParametersForType(type);
        updateParameters();
    }
}

void EqualizerNode::updateParametersForType(EQType type)
{
    parameterSliders.clear();
    
    // Common parameters
    addParameter("Frequency", 20.0f, 20000.0f, 1000.0f);
    
    switch (type)
    {
        case EQType::Lowpass:
        case EQType::Highpass:
            addParameter("Slope", 6.0f, 48.0f, 12.0f);
            addParameter("Q", 0.1f, 10.0f, 0.707f);
            break;
            
        case EQType::Bandpass:
        case EQType::Notch:
            addParameter("Q", 0.1f, 10.0f, 1.0f);
            break;
            
        case EQType::Peaking:
            addParameter("Gain", -24.0f, 24.0f, 0.0f);
            addParameter("Q", 0.1f, 10.0f, 1.0f);
            break;
    }
    
    for (auto* slider : parameterSliders)
        slider->addListener(this);
        
    resized();
}

void EqualizerNode::updateProcessor()
{
    if (auto* proc = dynamic_cast<EqualizerProcessor*>(processor.get()))
    {
        // Update all parameters using processor's parameter interface
        for (auto* slider : parameterSliders)
        {
            const auto& name = slider->getName();
            const auto value = slider->getValue();
            
            if (name == "Frequency")
                proc->setParameter(0, value);
            else if (name == "Gain")
                proc->setParameter(1, value);
            else if (name == "Q")
                proc->setParameter(2, value);
            // Remove slope parameter as it's not supported
        }
    }
}

void EqualizerNode::updateParameters()
{
    if (!visualizer || !processor)
        return;
        
    if (auto* proc = dynamic_cast<EqualizerProcessor*>(processor.get()))
    {
        visualizer->setType(static_cast<EQType>(typeSelector->getSelectedId() - 1));
        
        // Update visualizer with current parameter values from sliders
        for (auto* slider : parameterSliders)
        {
            const auto& name = slider->getName();
            if (name == "Frequency")
                visualizer->setFrequency(slider->getValue());
            else if (name == "Gain")
                visualizer->setGain(slider->getValue());
            else if (name == "Q")
                visualizer->setQ(slider->getValue());
        }
        
        // Update level meters using available methods
        visualizer->setInputLevel(proc->getInputLevel());
        visualizer->setOutputLevel(proc->getOutputLevel());
        
        visualizer->repaint();
    }
}

void EqualizerNode::updateMeters()
{
    if (!visualizer || !processor)
        return;
        
    if (auto* proc = dynamic_cast<EqualizerProcessor*>(processor.get()))
    {
        // Update using meter values from processor
        visualizer->setInputLevel(proc->getInputLevel());
        visualizer->setOutputLevel(proc->getOutputLevel());
        
        // Update parameter display
        for (auto* slider : parameterSliders)
        {
            const auto& name = slider->getName();
            if (name == "Frequency")
                visualizer->setFrequency(slider->getValue());
            else if (name == "Gain")
                visualizer->setGain(slider->getValue());
            else if (name == "Q")
                visualizer->setQ(slider->getValue());
        }
        
        visualizer->repaint();
    }
}
