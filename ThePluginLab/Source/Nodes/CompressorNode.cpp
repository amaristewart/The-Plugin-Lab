#include "CompressorNode.h"
#include "../Audio/AudioPort.h"
#include "../Visualizers/CompressorVisualizer.h"

CompressorNode::CompressorNode()
    : PluginNodeComponent("Compressor", juce::Colours::orange)
{
    // Initialize GUI components
    typeSelector = std::make_unique<juce::ComboBox>();
    bypassButton = std::make_unique<juce::ToggleButton>("Bypass");
    visualizer = std::make_unique<CompressorVisualizer>();

    typeSelector->addItem("Compressor", static_cast<int>(DynamicType::Compressor) + 1);
    typeSelector->addItem("Limiter", static_cast<int>(DynamicType::Limiter) + 1);
    typeSelector->addItem("Expander", static_cast<int>(DynamicType::Expander) + 1);
    typeSelector->addItem("Gate", static_cast<int>(DynamicType::Gate) + 1);
    typeSelector->setSelectedItemIndex(0);
    addAndMakeVisible(typeSelector.get());
    typeSelector->addListener(this);

    addAndMakeVisible(bypassButton.get());
    bypassButton->addListener(this);
    
    addAndMakeVisible(visualizer.get());

    // Setup initial state
    setSize(200, 300);
    addInputPort("Input");
    addOutputPort("Output");
    setupParameters();
}

CompressorNode::~CompressorNode()
{
    typeSelector->removeListener(this);
    bypassButton->removeListener(this);
}

// Setup initial parameters
void CompressorNode::setupParameters()
{
    // Clear and add default parameters for Compressor
    parameterSliders.clear();
    
    addParameter("Threshold", -60.0f, 0.0f, -20.0f);
    addParameter("Ratio", 1.0f, 20.0f, 4.0f);
    addParameter("Attack", 0.1f, 100.0f, 10.0f);
    addParameter("Release", 10.0f, 1000.0f, 100.0f);
    
    for (auto* slider : parameterSliders)
        slider->addListener(this);
}

void CompressorNode::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == typeSelector.get())
        typeChanged();
}

void CompressorNode::sliderValueChanged(juce::Slider* slider)
{
    updateProcessor();
    updateMeters();
}

void CompressorNode::buttonClicked(juce::Button* button)
{
    if (button == bypassButton.get())
        setBypassState(button->getToggleState());
}

void CompressorNode::typeChanged()
{
    auto type = static_cast<DynamicType>(typeSelector->getSelectedId() - 1);
    updateParametersForType(type);
}

void CompressorNode::updateParametersForType(DynamicType type)
{
    parameterSliders.clear();
    
    switch (type)
    {
        case DynamicType::Compressor:
            addParameter("Threshold", -60.0f, 0.0f, -20.0f);
            addParameter("Ratio", 1.0f, 20.0f, 4.0f);
            addParameter("Attack", 0.1f, 100.0f, 10.0f);
            addParameter("Release", 10.0f, 1000.0f, 100.0f);
            break;
            
        case DynamicType::Limiter:
            addParameter("Threshold", -60.0f, 0.0f, -0.1f);
            addParameter("Release", 10.0f, 1000.0f, 100.0f);
            break;
            
        case DynamicType::Expander:
            addParameter("Threshold", -60.0f, 0.0f, -40.0f);
            addParameter("Ratio", 1.0f, 20.0f, 2.0f);
            addParameter("Attack", 0.1f, 100.0f, 10.0f);
            addParameter("Release", 10.0f, 1000.0f, 100.0f);
            break;
            
        case DynamicType::Gate:
            addParameter("Threshold", -60.0f, 0.0f, -40.0f);
            addParameter("Attack", 0.1f, 100.0f, 1.0f);
            addParameter("Hold", 0.0f, 500.0f, 100.0f);
            addParameter("Release", 10.0f, 1000.0f, 100.0f);
            break;
    }
    
    for (auto* slider : parameterSliders)
        slider->addListener(this);
        
    resized();
}

void CompressorNode::updateProcessor()
{
    if (auto* proc = dynamic_cast<CompressorProcessor*>(processor.get()))
    {
        auto type = static_cast<DynamicType>(typeSelector->getSelectedId() - 1);
        proc->setDynamicsType(type);
        
        for (auto* slider : parameterSliders)
        {
            const auto& name = slider->getName();
            const auto value = slider->getValue();
            
            if (name == "Threshold")
                proc->setThreshold(value);
            else if (name == "Ratio")
                proc->setRatio(value);
            else if (name == "Attack")
                proc->setAttackTime(value);
            else if (name == "Release")
                proc->setReleaseTime(value);
            else if (name == "Hold")
                proc->setHoldTime(value);
        }
        
        proc->setBypassed(bypassButton->getToggleState());
    }
}

void CompressorNode::updateParameters()
{
    if (visualizer)
    {
        visualizer->setType(static_cast<DynamicType>(typeSelector->getSelectedId() - 1));
        
        for (auto* slider : parameterSliders)
        {
            if (slider->getName() == "Threshold")
                visualizer->setThreshold(slider->getValue());
            else if (slider->getName() == "Ratio")
                visualizer->setRatio(slider->getValue());
        }
        visualizer->repaint();
    }
}

void CompressorNode::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
    
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Dynamics", getLocalBounds().removeFromTop(30),
               juce::Justification::centred, true);
}

void CompressorNode::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    typeSelector->setBounds(bounds.removeFromTop(24));
    bounds.removeFromTop(6);

    visualizer->setBounds(bounds.removeFromTop(100));
    bounds.removeFromTop(6);

    const int sliderHeight = 24;
    for (auto* slider : parameterSliders)
    {
        slider->setBounds(bounds.removeFromTop(sliderHeight));
        bounds.removeFromTop(4);
    }

    bounds.removeFromBottom(4);
    bypassButton->setBounds(bounds.removeFromBottom(24));
    
    // Make sure ports stay in correct position
    for (auto* port : inputPorts)
        port->setBounds(0, getHeight() / 2 - 10, 20, 20);
    
    for (auto* port : outputPorts)
        port->setBounds(getWidth() - 20, getHeight() / 2 - 10, 20, 20);
}

void CompressorNode::updateMeters()
{
    if (auto* proc = dynamic_cast<CompressorProcessor*>(processor.get()))
    {
        if (visualizer)
        {
            visualizer->setLevels(proc->getInputLevel(), proc->getOutputLevel());
            visualizer->setGainReduction(proc->getGainReduction());
        }
    }
}

void CompressorNode::addInputPort(const juce::String& name)
{
    auto* port = new AudioPort(AudioPort::Type::Input);
    port->setBounds(0, getHeight() / 2 - 10, 20, 20);
    addAndMakeVisible(port);
    inputPorts.add(port);
}

void CompressorNode::addOutputPort(const juce::String& name)
{
    auto* port = new AudioPort(AudioPort::Type::Output);
    port->setBounds(getWidth() - 20, getHeight() / 2 - 10, 20, 20);
    addAndMakeVisible(port);
    outputPorts.add(port);
}

void CompressorNode::addParameter(const juce::String& name, float min, float max, float defaultValue)
{
    auto* slider = new juce::Slider();
    slider->setRange(min, max);
    slider->setValue(defaultValue);
    slider->setSliderStyle(juce::Slider::LinearVertical);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider->setName(name);
    slider->addListener(this);
    addAndMakeVisible(slider);
    parameterSliders.add(slider);
}

void CompressorNode::setBypassState(bool shouldBeBypassed)
{
    if (auto* proc = dynamic_cast<CompressorProcessor*>(processor.get()))
    {
        proc->setBypassed(shouldBeBypassed);
    }
}
