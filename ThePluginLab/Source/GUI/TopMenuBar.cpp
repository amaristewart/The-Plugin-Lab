#include "TopMenuBar.h"
#include "BinaryData.h"

TopMenuBar::TopMenuBar()
{
    menuBar = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menuBar.get());
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
   
}

void TopMenuBar::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromLeft(200); // Space for logo
    menuBar->setBounds(bounds);
} 
