#pragma once
#include <JuceHeader.h>
#include "Common/Types.h"
#include "GUI/TopMenuBar.h"
#include "GUI/ToolPalette.h"
#include "GUI/PluginToolbar.h"
#include "GUI/PluginEditorCanvas.h"
#include "GUI/ComponentPanel.h"
#include "Common/Features.h"

class PluginEditorCanvas;

/**
 * Main application component containing toolbar, component panel, and canvas
 */
class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    // Project management functions
    void exportPlugin();
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectToFile(const juce::File& file);
    void loadProject(const juce::File& file);
    
    // Main components
    std::unique_ptr<TopMenuBar> topMenuBar;
    std::unique_ptr<ToolPalette> toolPalette;
    std::unique_ptr<PluginEditorCanvas> canvas;
    
    // File handling
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::File currentProjectFile;
    
    // Current selected tool
    ComponentType currentTool = ComponentType::Knob;
    
    // Logo
    juce::Image logo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
