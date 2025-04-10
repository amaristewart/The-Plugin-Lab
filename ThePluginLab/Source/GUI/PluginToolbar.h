#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"
#include "../Common/Features.h"

/**
 * Toolbar component with plugin tools and menu options
 */
class PluginToolbar : public juce::Component
{
public:
    PluginToolbar();
    ~PluginToolbar() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Set callbacks for tool selection and menu actions
    void setToolSelectedCallback(std::function<void(ComponentType)> callback);
    void setActiveTool(ComponentType toolType);
    
    // Menu action callbacks
    std::function<void(NodeType)> onPluginSelected;
    std::function<void()> onNewProject;
    std::function<void()> onOpenProject;
    std::function<void()> onSaveProject;
    std::function<void()> onSaveProjectAs;
    std::function<void()> onExportRequest;
    std::function<void(const juce::String&)> onLoadTemplate;
    std::function<void(const juce::String&)> onStartTutorial;
    std::function<void(bool)> onShowTips;
    
private:
    // Tool button class for the toolbar
    class ToolButton : public juce::DrawableButton
    {
    public:
        ToolButton(const juce::String& name, ComponentType type);
        ComponentType getType() const { return toolType; }
        
    private:
        ComponentType toolType;
    };
    
    // MenuBarModel implementation for the menu
    class ToolbarMenuModel : public juce::MenuBarModel
    {
    public:
        ToolbarMenuModel(PluginToolbar& owner) : toolbar(owner) {}
        
        juce::StringArray getMenuBarNames() override;
        
        juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& /*menuName*/) override;
        
        void menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) override;
        
    private:
        PluginToolbar& toolbar;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolbarMenuModel)
    };
    
    void createToolButtons();
    void addToolButton(const juce::String& name, ComponentType type);
    ToolButton* findToolButton(ComponentType type);
    
    juce::OwnedArray<ToolButton> toolButtons;
    ComponentType activeToolType;
    
    std::function<void(ComponentType)> onToolSelected;
    
    // Logo and menu
    juce::Image logo;
    std::unique_ptr<juce::MenuBarComponent> menuBar;
    std::unique_ptr<ToolbarMenuModel> menuModel;
    
    // Toolbar buttons
    juce::TextButton newButton {"New"};
    juce::TextButton openButton {"Open"};
    juce::TextButton saveButton {"Save"};
    juce::TextButton exportButton {"Export"};
    juce::TextButton settingsButton {"Settings"};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginToolbar)
};
