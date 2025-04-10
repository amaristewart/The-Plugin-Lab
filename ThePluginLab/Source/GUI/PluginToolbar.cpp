#include "PluginToolbar.h"

PluginToolbar::PluginToolbar() : activeToolType(ComponentType::Knob)
{
    setSize(400, 50);
    createToolButtons();
    
    // Set up menu
    menuModel = std::make_unique<ToolbarMenuModel>(*this);
    menuBar = std::make_unique<juce::MenuBarComponent>(menuModel.get());
    addAndMakeVisible(menuBar.get());

    // Add buttons to the toolbar
    addAndMakeVisible(newButton);
    addAndMakeVisible(openButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(exportButton);
    addAndMakeVisible(settingsButton);
    
    // Set up button callbacks
    newButton.onClick = [this]() { if (onNewProject) onNewProject(); };
    openButton.onClick = [this]() { if (onOpenProject) onOpenProject(); };
    saveButton.onClick = [this]() { if (onSaveProject) onSaveProject(); };
    exportButton.onClick = [this]() { if (onExportRequest) onExportRequest(); };
    
    // Configure button appearance
    juce::Array<juce::TextButton*> buttons = { &newButton, &openButton, &saveButton, &exportButton, &settingsButton };
    for (auto* button : buttons)
    {
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF404040));
        button->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }
    
    // Set fixed size for toolbar
    setSize(800, Features::toolbarHeight);
}

PluginToolbar::~PluginToolbar()
{
    // Clean up
}

void PluginToolbar::paint(juce::Graphics& g)
{
    // Draw toolbar gradient background
    g.setGradientFill(juce::ColourGradient(
        Features::toolbarGradientStart, 0.0f, 0.0f,
        Features::toolbarGradientEnd, static_cast<float>(getWidth()), 0.0f,
        false));
    g.fillRect(getLocalBounds());
}

void PluginToolbar::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // Set up menu bar at the top
    if (menuBar != nullptr)
    {
        menuBar->setBounds(bounds.removeFromTop(30));
        bounds.removeFromTop(5);
    }
    
    const int buttonWidth = 40;
    const int spacing = 5;
    
    // Layout tool buttons
    for (auto* button : toolButtons)
    {
        button->setBounds(bounds.removeFromLeft(buttonWidth));
        bounds.removeFromLeft(spacing);
    }

    // Reserve space for logo
    bounds = getLocalBounds();
    bounds.removeFromLeft(200); // Space for logo
    
    // Layout buttons
    const int newButtonWidth = 80;
    const int margin = 10;
    
    newButton.setBounds(bounds.removeFromLeft(newButtonWidth));
    bounds.removeFromLeft(margin);
    
    openButton.setBounds(bounds.removeFromLeft(newButtonWidth));
    bounds.removeFromLeft(margin);
    
    saveButton.setBounds(bounds.removeFromLeft(newButtonWidth));
    bounds.removeFromLeft(margin);
    
    exportButton.setBounds(bounds.removeFromLeft(newButtonWidth));
    bounds.removeFromLeft(margin);
    
    // Settings button on the far right
    settingsButton.setBounds(bounds.removeFromRight(newButtonWidth));
}

void PluginToolbar::setToolSelectedCallback(std::function<void(ComponentType)> callback)
{
    onToolSelected = std::move(callback);
}

void PluginToolbar::setActiveTool(ComponentType toolType)
{
    activeToolType = toolType;
    
    for (auto* button : toolButtons)
    {
        button->setToggleState(button->getType() == toolType, juce::dontSendNotification);
    }
}

void PluginToolbar::createToolButtons()
{
    // Add GUI components
    addToolButton("Knob", ComponentType::Knob);
    addToolButton("Slider", ComponentType::Slider);
    addToolButton("Button", ComponentType::Button);
    addToolButton("Label", ComponentType::Label);
    
    // Add EQ components
    addToolButton("LowPass", ComponentType::EQLowpass);
    addToolButton("HighPass", ComponentType::EQHighpass);
    
    // Add compressor components
    addToolButton("Compressor", ComponentType::CompThreshold);
    
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

void PluginToolbar::addToolButton(const juce::String& name, ComponentType type)
{
    auto* button = new ToolButton(name, type);
    addAndMakeVisible(button);
    toolButtons.add(button);
}

PluginToolbar::ToolButton* PluginToolbar::findToolButton(ComponentType type)
{
    for (auto* button : toolButtons)
    {
        if (button->getType() == type)
            return button;
    }
    return nullptr;
}

// Tool button implementation
PluginToolbar::ToolButton::ToolButton(const juce::String& name, ComponentType type)
    : juce::DrawableButton(name, juce::DrawableButton::ImageOnButtonBackground),
      toolType(type)
{
    // Create icons based on type
    std::unique_ptr<juce::DrawablePath> normalIcon = std::make_unique<juce::DrawablePath>();
    juce::Path p;
    
    switch (type)
    {
        case ComponentType::Knob:
            p.addEllipse(0.0f, 0.0f, 20.0f, 20.0f);
            p.addRectangle(9.0f, 0.0f, 2.0f, 10.0f);
            break;
            
        case ComponentType::Slider:
            p.addRectangle(0.0f, 8.0f, 20.0f, 4.0f);
            p.addEllipse(5.0f, 5.0f, 10.0f, 10.0f);
            break;
            
        case ComponentType::Button:
            p.addRoundedRectangle(0.0f, 0.0f, 20.0f, 20.0f, 4.0f);
            break;
            
        case ComponentType::Label:
            p.addRectangle(0.0f, 0.0f, 20.0f, 20.0f);
            p.startNewSubPath(2.0f, 10.0f);
            p.lineTo(18.0f, 10.0f);
            break;
            
        case ComponentType::EQLowpass:
            // Draw a low-pass filter curve
            p.startNewSubPath(0.0f, 0.0f);
            p.lineTo(10.0f, 0.0f);
            p.quadraticTo(15.0f, 0.0f, 20.0f, 20.0f);
            p.lineTo(0.0f, 20.0f);
            p.closeSubPath();
            break;
            
        case ComponentType::EQHighpass:
            // Draw a high-pass filter curve
            p.startNewSubPath(0.0f, 20.0f);
            p.lineTo(10.0f, 20.0f);
            p.quadraticTo(15.0f, 20.0f, 20.0f, 0.0f);
            p.lineTo(20.0f, 0.0f);
            p.lineTo(20.0f, 20.0f);
            p.closeSubPath();
            break;
            
        case ComponentType::CompThreshold:
            // Draw a compression threshold curve
            p.startNewSubPath(0.0f, 20.0f);
            p.lineTo(10.0f, 10.0f);
            p.lineTo(20.0f, 5.0f);
            break;
            
        default:
            // Default rectangle
            p.addRectangle(0.0f, 0.0f, 20.0f, 20.0f);
            break;
    }
    
    normalIcon->setPath(p);
    normalIcon->setFill(juce::Colours::white);
    
    setImages(normalIcon.get());
}

juce::StringArray PluginToolbar::ToolbarMenuModel::getMenuBarNames() 
{
    return { "File", "Templates", "Learn" };
}

juce::PopupMenu PluginToolbar::ToolbarMenuModel::getMenuForIndex(int menuIndex, const juce::String& /*menuName*/)
{
    juce::PopupMenu menu;
    
    if (menuIndex == 0) // File menu
    {
        menu.addItem(1, "New Project", true, false);
        menu.addItem(2, "Open Project...", true, false);
        menu.addSeparator();
        menu.addItem(3, "Save", true, false);
        menu.addItem(4, "Save As...", true, false);
        menu.addSeparator();
        menu.addItem(5, "Export Plugin...", true, false);
    }
    else if (menuIndex == 1) // Templates menu
    {
        menu.addItem(6, "Basic EQ", true, false);
        menu.addItem(7, "Basic Compressor", true, false);
    }
    else if (menuIndex == 2) // Learn menu
    {
        menu.addItem(8, "Start Tutorial", true, false);
        menu.addItem(9, "Show Tips", true, false);
    }
    
    return menu;
}

void PluginToolbar::ToolbarMenuModel::menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) 
{
    switch (menuItemID)
    {
        case 1: if (toolbar.onNewProject) toolbar.onNewProject(); break;
        case 2: if (toolbar.onOpenProject) toolbar.onOpenProject(); break;
        case 3: if (toolbar.onSaveProject) toolbar.onSaveProject(); break;
        case 4: if (toolbar.onSaveProjectAs) toolbar.onSaveProjectAs(); break;
        case 5: if (toolbar.onExportRequest) toolbar.onExportRequest(); break;
        case 6: if (toolbar.onLoadTemplate) toolbar.onLoadTemplate("Basic EQ"); break;
        case 7: if (toolbar.onLoadTemplate) toolbar.onLoadTemplate("Basic Compressor"); break;
        case 8: if (toolbar.onStartTutorial) toolbar.onStartTutorial("Getting Started"); break;
        case 9: if (toolbar.onShowTips) toolbar.onShowTips(true); break;
    }
}
