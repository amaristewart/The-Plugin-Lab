#include "ToolPalette.h"

ToolPalette::ToolPalette()
    : activeToolType(ComponentType::Knob),
      guiSectionLabel("guiSectionLabel", "GUI Controls"),
      eqSectionLabel("eqSectionLabel", "EQ Components"),
      dynamicsSectionLabel("dynSectionLabel", "Dynamics")
{
    // Set up section labels
    guiSectionLabel.setFont(juce::Font(juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::bold));
    eqSectionLabel.setFont(juce::Font(juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::bold));
    dynamicsSectionLabel.setFont(juce::Font(juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::bold));
    
    guiSectionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    eqSectionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    dynamicsSectionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    
    addAndMakeVisible(guiSectionLabel);
    addAndMakeVisible(eqSectionLabel);
    addAndMakeVisible(dynamicsSectionLabel);
    
    // Create the tool buttons
    createToolButtons();
    
    // Set fixed size
    setSize(60, 400);
}

ToolPalette::~ToolPalette()
{
    // Clean up resources
}

void ToolPalette::paint(juce::Graphics& g)
{
    // Draw background
    g.fillAll(juce::Colour(0xFF303030));
    
    // Draw separator lines
    g.setColour(juce::Colours::darkgrey);
    
    int y1 = guiSectionLabel.getBottom() + 5;
    int y2 = eqSectionLabel.getY() - 5;
    g.drawLine(5.0f, static_cast<float>(y1), getWidth() - 5.0f, static_cast<float>(y1), 1.0f);
    g.drawLine(5.0f, static_cast<float>(y2), getWidth() - 5.0f, static_cast<float>(y2), 1.0f);
    
    y1 = eqSectionLabel.getBottom() + 5;
    y2 = dynamicsSectionLabel.getY() - 5;
    g.drawLine(5.0f, static_cast<float>(y1), getWidth() - 5.0f, static_cast<float>(y1), 1.0f);
    g.drawLine(5.0f, static_cast<float>(y2), getWidth() - 5.0f, static_cast<float>(y2), 1.0f);
}

void ToolPalette::resized()
{
    const int margin = 5;
    const int buttonSize = 44;
    const int labelHeight = 20;
    
    // Position section labels
    guiSectionLabel.setBounds(margin, margin, getWidth() - 2 * margin, labelHeight);
    
    // Position GUI tool buttons
    int y = guiSectionLabel.getBottom() + margin;
    int numGuiButtons = 0;
    
    for (auto* button : toolButtons)
    {
        ComponentType type = button->getType();
        if (type == ComponentType::Knob || type == ComponentType::Slider ||
            type == ComponentType::Button || type == ComponentType::Label)
        {
            button->setBounds((getWidth() - buttonSize) / 2, y, buttonSize, buttonSize);
            y += buttonSize + margin;
            numGuiButtons++;
        }
    }
    
    // Position EQ section label
    eqSectionLabel.setBounds(margin, y + margin, getWidth() - 2 * margin, labelHeight);
    
    // Position EQ tool buttons
    y = eqSectionLabel.getBottom() + margin;
    int numEqButtons = 0;
    
    for (auto* button : toolButtons)
    {
        ComponentType type = button->getType();
        if (type == ComponentType::EQLowpass || type == ComponentType::EQHighpass ||
            type == ComponentType::EQBandpass || type == ComponentType::EQNotch ||
            type == ComponentType::EQPeaking)
        {
            button->setBounds((getWidth() - buttonSize) / 2, y, buttonSize, buttonSize);
            y += buttonSize + margin;
            numEqButtons++;
        }
    }
    
    // Position dynamics section label
    dynamicsSectionLabel.setBounds(margin, y + margin, getWidth() - 2 * margin, labelHeight);
    
    // Position dynamics tool buttons
    y = dynamicsSectionLabel.getBottom() + margin;
    
    for (auto* button : toolButtons)
    {
        ComponentType type = button->getType();
        if (type == ComponentType::CompThreshold || type == ComponentType::CompRatio)
        {
            button->setBounds((getWidth() - buttonSize) / 2, y, buttonSize, buttonSize);
            y += buttonSize + margin;
        }
    }
}

void ToolPalette::createToolButtons()
{
    // Add GUI components
    addToolButton("Knob", ComponentType::Knob, Features::guiColor);
    addToolButton("Slider", ComponentType::Slider, Features::guiColor);
    addToolButton("Button", ComponentType::Button, Features::guiColor);
    addToolButton("Label", ComponentType::Label, Features::guiColor);
    
    // Add EQ components
    addToolButton("Lowpass", ComponentType::EQLowpass, Features::eqColor);
    addToolButton("Highpass", ComponentType::EQHighpass, Features::eqColor);
    addToolButton("Bandpass", ComponentType::EQBandpass, Features::eqColor);
    addToolButton("Notch", ComponentType::EQNotch, Features::eqColor);
    addToolButton("Peak", ComponentType::EQPeaking, Features::eqColor);
    
    // Add compressor components
    addToolButton("Compressor", ComponentType::CompThreshold, Features::compColor);
    addToolButton("Limiter", ComponentType::CompRatio, Features::compColor);
    
    // Set up button group
    for (auto* button : toolButtons)
    {
        button->setRadioGroupId(1);
        button->setClickingTogglesState(true);
        button->setToggleState(button->getType() == activeToolType, juce::dontSendNotification);
        
        button->onClick = [this, button]()
        {
            activeToolType = button->getType();
            if (onToolSelected)
                onToolSelected(activeToolType);
        };
    }
}

void ToolPalette::addToolButton(const juce::String& name, ComponentType type, const juce::Colour& color)
{
    auto* button = new ToolButton(name, type, color);
    addAndMakeVisible(button);
    toolButtons.add(button);
}

void ToolPalette::setActiveTool(ComponentType toolType)
{
    activeToolType = toolType;
    
    for (auto* button : toolButtons)
    {
        button->setToggleState(button->getType() == toolType, juce::dontSendNotification);
    }
}

// Tool button implementation
ToolPalette::ToolButton::ToolButton(const juce::String& name, ComponentType type, const juce::Colour& color)
    : juce::DrawableButton(name, juce::DrawableButton::ImageOnButtonBackground),
      toolType(type),
      buttonColor(color)
{
    // Create icons based on type
    std::unique_ptr<juce::DrawablePath> normalIcon = std::make_unique<juce::DrawablePath>();
    juce::Path p;
    
    // Common icon size
    const float iconSize = 20.0f;
    
    switch (type)
    {
        case ComponentType::Knob:
            p.addEllipse(0.0f, 0.0f, iconSize, iconSize);
            p.addRectangle(9.0f, 0.0f, 2.0f, 10.0f);
            break;
            
        case ComponentType::Slider:
            p.addRectangle(0.0f, 8.0f, iconSize, 4.0f);
            p.addEllipse(5.0f, 5.0f, 10.0f, 10.0f);
            break;
            
        case ComponentType::Button:
            p.addRoundedRectangle(0.0f, 0.0f, iconSize, iconSize, 4.0f);
            break;
            
        case ComponentType::Label:
            p.addRectangle(0.0f, 0.0f, iconSize, iconSize);
            p.startNewSubPath(2.0f, 10.0f);
            p.lineTo(18.0f, 10.0f);
            break;
            
        case ComponentType::EQLowpass:
            // Draw a low-pass filter curve
            p.startNewSubPath(0.0f, 0.0f);
            p.lineTo(10.0f, 0.0f);
            p.quadraticTo(15.0f, 0.0f, iconSize, iconSize);
            p.lineTo(0.0f, iconSize);
            p.closeSubPath();
            break;
            
        case ComponentType::EQHighpass:
            // Draw a high-pass filter curve
            p.startNewSubPath(0.0f, iconSize);
            p.lineTo(10.0f, iconSize);
            p.quadraticTo(15.0f, iconSize, iconSize, 0.0f);
            p.lineTo(iconSize, 0.0f);
            p.lineTo(iconSize, iconSize);
            p.closeSubPath();
            break;
            
        case ComponentType::EQBandpass:
            // Draw a bandpass filter curve
            p.startNewSubPath(0.0f, iconSize);
            p.lineTo(5.0f, iconSize);
            p.quadraticTo(8.0f, 0.0f, 12.0f, 0.0f);
            p.quadraticTo(16.0f, iconSize, iconSize, iconSize);
            p.closeSubPath();
            break;
            
        case ComponentType::EQNotch:
            // Draw a notch filter curve
            p.startNewSubPath(0.0f, 5.0f);
            p.lineTo(8.0f, 5.0f);
            p.lineTo(8.0f, 15.0f);
            p.lineTo(12.0f, 15.0f);
            p.lineTo(12.0f, 5.0f);
            p.lineTo(iconSize, 5.0f);
            break;
            
        case ComponentType::EQPeaking:
            // Draw a peaking filter curve
            p.startNewSubPath(0.0f, 10.0f);
            p.cubicTo(5.0f, 10.0f, 8.0f, 0.0f, 10.0f, 0.0f);
            p.cubicTo(12.0f, 0.0f, 15.0f, 10.0f, iconSize, 10.0f);
            break;
            
        case ComponentType::CompThreshold:
            // Draw a compression threshold curve
            p.startNewSubPath(0.0f, iconSize);
            p.lineTo(10.0f, 10.0f);
            p.lineTo(iconSize, 5.0f);
            break;
            
        case ComponentType::CompRatio:
            // Draw a compression ratio curve
            p.startNewSubPath(0.0f, iconSize);
            p.lineTo(10.0f, 10.0f);
            p.lineTo(iconSize, 5.0f);
            break;
            
        default:
            // Default rectangle
            p.addRectangle(0.0f, 0.0f, iconSize, iconSize);
            break;
    }
    
    normalIcon->setPath(p);
    normalIcon->setFill(juce::Colours::white);
    
    setImages(normalIcon.get());
    
    // Set button colors
    setColour(juce::DrawableButton::backgroundColourId, buttonColor.withAlpha(0.6f));
    setColour(juce::DrawableButton::backgroundOnColourId, buttonColor);
}
