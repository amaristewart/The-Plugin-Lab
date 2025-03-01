#pragma once
#include <JuceHeader.h>
#include "../Common/Forward.h"

class PluginToolbar : public juce::Component,
                     public juce::DragAndDropContainer
{
public:
    PluginToolbar();
    ~PluginToolbar() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    std::function<void(NodeType)> onPluginSelected;
    
    std::function<void()> onNewProject;
    std::function<void()> onOpenProject;
    std::function<void()> onSaveProject;
    std::function<void()> onSaveProjectAs;
    std::function<void()> onExportPlugin;
    std::function<void(const juce::String&)> onLoadTemplate;
    std::function<void(const juce::String&)> onStartTutorial;
    std::function<void(bool)> onShowTips;
    
private:
    // MenuBarModel implementation
    class ToolbarMenuModel : public juce::MenuBarModel
    {
    public:
        ToolbarMenuModel(PluginToolbar& owner) : toolbar(owner) {}
        
        juce::StringArray getMenuBarNames() override
        {
            return { "File", "Templates", "Learn" };
        }
        
        juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& /*menuName*/) override
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
        
        void menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) override
        {
            switch (menuItemID)
            {
                case 1: if (toolbar.onNewProject) toolbar.onNewProject(); break;
                case 2: if (toolbar.onOpenProject) toolbar.onOpenProject(); break;
                case 3: if (toolbar.onSaveProject) toolbar.onSaveProject(); break;
                case 4: if (toolbar.onSaveProjectAs) toolbar.onSaveProjectAs(); break;
                case 5: if (toolbar.onExportPlugin) toolbar.onExportPlugin(); break;
                case 6: if (toolbar.onLoadTemplate) toolbar.onLoadTemplate("Basic EQ"); break;
                case 7: if (toolbar.onLoadTemplate) toolbar.onLoadTemplate("Basic Compressor"); break;
                case 8: if (toolbar.onStartTutorial) toolbar.onStartTutorial("Getting Started"); break;
                case 9: if (toolbar.onShowTips) toolbar.onShowTips(true); break;
            }
        }
        
    private:
        PluginToolbar& toolbar;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolbarMenuModel)
    };
    
    class PluginButton : public juce::TextButton
    {
    public:
        PluginButton(const juce::String& name, NodeType t)
            : juce::TextButton(name), type(t) {}
        
        NodeType getType() const { return type; }
        
    private:
        NodeType type;
    };
    
    void addPluginTemplate(const juce::String& name, NodeType type);
    NodeType getNodeTypeFromName(const juce::String& name);
    
    juce::OwnedArray<PluginButton> toolbarButtons;
    
    juce::Image logo;
    std::unique_ptr<juce::MenuBarComponent> menuBar;
    std::unique_ptr<ToolbarMenuModel> menuModel; 
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginToolbar)
};
