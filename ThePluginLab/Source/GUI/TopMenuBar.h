#pragma once
#include <JuceHeader.h>

class TopMenuBar : public juce::Component,
                  public juce::MenuBarModel
{
public:
    TopMenuBar();
    
    // MenuBarModel implementation
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int index, const juce::String& name) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Callback functions
    std::function<void()> onNewProject;
    std::function<void()> onOpenProject;
    std::function<void()> onSaveProject;
    std::function<void()> onSaveProjectAs;
    std::function<void()> onExportPlugin;
    std::function<void(const juce::String&)> onLoadTemplate;
    std::function<void(const juce::String&)> onStartTutorial;
    std::function<void(bool)> onShowTips;
    
private:
    std::unique_ptr<juce::MenuBarComponent> menuBar;
    juce::Image logo;
    bool tipsEnabled = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopMenuBar)
}; 