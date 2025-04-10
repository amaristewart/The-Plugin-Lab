#include "TopMenuBar.h"

TopMenuBar::TopMenuBar()
{
    // Load logo
    logo = juce::ImageCache::getFromMemory(BinaryData::PluginLabLogo_png, BinaryData::PluginLabLogo_pngSize);
    
    if (logo.isNull())
    {
        juce::Logger::writeToLog("Failed to load logo from binary data!");
    }
    else
    {
        juce::Logger::writeToLog("Logo loaded successfully. Size: " + 
                               juce::String(logo.getWidth()) + "x" + juce::String(logo.getHeight()));
    }
    
    // Create menu bar
    menuBar = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menuBar.get());
    
    // Configure and add buttons
    addAndMakeVisible(newButton);
    addAndMakeVisible(openButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(exportButton);
    
    // Set up button callbacks
    newButton.onClick = [this]() { if (onNewProject) onNewProject(); };
    openButton.onClick = [this]() { if (onOpenProject) onOpenProject(); };
    saveButton.onClick = [this]() { if (onSaveProject) onSaveProject(); };
    exportButton.onClick = [this]() { if (onExportPlugin) onExportPlugin(); };
    
    // Style buttons to be more kid-friendly
    juce::Colour buttonColor = juce::Colour(0xFF5D9CEC);
    
    for (auto* button : { &newButton, &openButton, &saveButton, &exportButton })
    {
        button->setColour(juce::TextButton::buttonColourId, buttonColor);
        button->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        button->setColour(juce::TextButton::buttonOnColourId, buttonColor.brighter(0.2f));
    }
    
    // Set fixed size
    setSize(800, Features::toolbarHeight);
}

TopMenuBar::~TopMenuBar()
{
    // Clean up resources
}

juce::StringArray TopMenuBar::getMenuBarNames()
{
    return { "Project", "Templates", "Learning" };
}

juce::PopupMenu TopMenuBar::getMenuForIndex(int /*index*/, const juce::String& name)
{
    juce::PopupMenu menu;
    
    if (name == "Project")
    {
        menu.addItem(1, "New Project");
        menu.addItem(2, "Open Project...");
        menu.addSeparator();
        menu.addItem(3, "Save");
        menu.addItem(4, "Save As...");
        menu.addSeparator();
        menu.addItem(5, "Export Plugin...");
    }
    else if (name == "Templates")
    {
        menu.addItem(101, "Basic EQ");
        menu.addItem(102, "Basic Compressor");
        menu.addItem(103, "Multi-band EQ");
    }
    else if (name == "Learning")
    {
        menu.addItem(201, "EQ Tutorial");
        menu.addItem(202, "Compression Tutorial");
        menu.addItem(203, "Plugin Basics");
        menu.addSeparator();
        menu.addItem(204, "Show Tips", true, tipsEnabled);
    }
    
    return menu;
}

void TopMenuBar::menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/)
{
    switch (menuItemID)
    {
        case 1: if (onNewProject) onNewProject(); break;
        case 2: if (onOpenProject) onOpenProject(); break;
        case 3: if (onSaveProject) onSaveProject(); break;
        case 4: if (onSaveProjectAs) onSaveProjectAs(); break;
        case 5: if (onExportPlugin) onExportPlugin(); break;
        
        case 101: if (onLoadTemplate) onLoadTemplate("Basic EQ"); break;
        case 102: if (onLoadTemplate) onLoadTemplate("Basic Compressor"); break;
        case 103: if (onLoadTemplate) onLoadTemplate("Multi-band EQ"); break;
        
        case 201: if (onStartTutorial) onStartTutorial("EQ Tutorial"); break;
        case 202: if (onStartTutorial) onStartTutorial("Compression Tutorial"); break;
        case 203: if (onStartTutorial) onStartTutorial("Plugin Basics"); break;
        case 204: 
            tipsEnabled = !tipsEnabled;
            if (onShowTips) onShowTips(tipsEnabled); 
            break;
    }
}

void TopMenuBar::paint(juce::Graphics& g)
{
    // Draw gradient background
    g.setGradientFill(juce::ColourGradient(
        Features::toolbarGradientStart, 0.0f, 0.0f,
        Features::toolbarGradientEnd, static_cast<float>(getWidth()), 0.0f,
        false));
    g.fillRect(getLocalBounds());
    
    // Draw logo
    if (!logo.isNull())
    {
        g.drawImageAt(logo, 10, (getHeight() - logo.getHeight()) / 2);
        juce::Logger::writeToLog("Drawing logo at position: 10, " + 
                               juce::String((getHeight() - logo.getHeight()) / 2));
    }
    else
    {
        juce::Logger::writeToLog("Logo image is null!");
    }
}

void TopMenuBar::resized()
{
    auto bounds = getLocalBounds();
    
    // Space for logo
    bounds.removeFromLeft(180);
    
    // Menu bar at the top
    menuBar->setBounds(bounds.removeFromLeft(300));
    
    // Layout buttons
    const int buttonWidth = 80;
    const int margin = 10;
    
    // Push buttons to the right side of the menu bar
    auto buttonBounds = getLocalBounds().removeFromRight(4 * buttonWidth + 3 * margin);
    
    newButton.setBounds(buttonBounds.removeFromLeft(buttonWidth));
    buttonBounds.removeFromLeft(margin);
    
    openButton.setBounds(buttonBounds.removeFromLeft(buttonWidth));
    buttonBounds.removeFromLeft(margin);
    
    saveButton.setBounds(buttonBounds.removeFromLeft(buttonWidth));
    buttonBounds.removeFromLeft(margin);
    
    exportButton.setBounds(buttonBounds.removeFromLeft(buttonWidth));
}
