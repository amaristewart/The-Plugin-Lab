#pragma once
#include <JuceHeader.h>

class PluginToolbar;
class PluginEditorCanvas;
class ComponentPanel;
class PluginProject;

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
    std::unique_ptr<PluginToolbar> toolbar;
    std::unique_ptr<ComponentPanel> componentPanel;
    std::unique_ptr<PluginEditorCanvas> canvas;
    
    // File handling
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::File currentProjectFile;
    
    // Logo
    juce::Image logo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
