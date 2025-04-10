#pragma once
#include <JuceHeader.h>
#include "../Common/Features.h"

/**
 * Top menu bar component with main menu and logo display
 */
class TopMenuBar : public juce::Component,
                  public juce::MenuBarModel
{
public:
    TopMenuBar();
    ~TopMenuBar() override;
    
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
    // Simple styled button for the top menu
    class StyledButton : public juce::TextButton
    {
    public:
        StyledButton(const juce::String& buttonText) : juce::TextButton(buttonText)
        {
            setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF404040));
            setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        }
    };
    
    std::unique_ptr<juce::MenuBarComponent> menuBar;
    juce::Image logo;
    bool tipsEnabled = false;
    
    // Top menu buttons
    StyledButton newButton{"New"};
    StyledButton openButton{"Open"};
    StyledButton saveButton{"Save"};
    StyledButton exportButton{"Export"};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopMenuBar)
};
