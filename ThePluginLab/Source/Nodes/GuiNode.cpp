#include "GuiNode.h"

GuiNode::GuiNode(ComponentType type)
    : PluginNodeComponent("GUI Node", juce::Colours::lightblue)
    , componentType(type)
{
    setSize(120, 120);
    setupForType();
}

void GuiNode::paint(juce::Graphics& g)
{
    PluginNodeComponent::paint(g);
    if (control)
        control->paint(g);
}

void GuiNode::resized()
{
    PluginNodeComponent::resized();
    if (control)
        control->setBounds(getLocalBounds().reduced(10));
}

void GuiNode::setupForType()
{
    switch (componentType)
    {
        case ComponentType::Knob:
            addKnob();
            break;
        case ComponentType::Slider:
            addSlider();
            break;
        case ComponentType::Button:
            addButton();
            break;
        case ComponentType::Label:
            addLabel();
            break;
        default:
            break;
    }
}

void GuiNode::addKnob()
{
    auto knob = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag,
                                              juce::Slider::TextBoxBelow);
    knob->setSize(80, 80);
    knob->setCentrePosition(getWidth() / 2, getHeight() / 2);
    addAndMakeVisible(*knob);
    control = std::move(knob);
}

void GuiNode::addSlider()
{
    auto slider = std::make_unique<juce::Slider>(juce::Slider::LinearVertical,
                                                juce::Slider::TextBoxBelow);
    slider->setSize(40, 100);
    slider->setCentrePosition(getWidth() / 2, getHeight() / 2);
    addAndMakeVisible(*slider);
    control = std::move(slider);
}

void GuiNode::addButton()
{
    auto button = std::make_unique<juce::TextButton>("Button");
    button->setSize(80, 30);
    button->setCentrePosition(getWidth() / 2, getHeight() / 2);
    addAndMakeVisible(*button);
    control = std::move(button);
}

void GuiNode::addLabel()
{
    auto label = std::make_unique<juce::Label>();
    label->setText("Label", juce::dontSendNotification);
    label->setJustificationType(juce::Justification::centred);
    label->setSize(80, 30);
    label->setCentrePosition(getWidth() / 2, getHeight() / 2);
    addAndMakeVisible(*label);
    control = std::move(label);
}
