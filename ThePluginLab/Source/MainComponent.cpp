#include "MainComponent.h"

namespace MainApp {

MainComponent::MainComponent()
{
    // Create logo
    logo = juce::ImageCache::getFromMemory(BinaryData::PluginLabLogo_png, BinaryData::PluginLabLogo_pngSize);
    
    // Create main components
    topMenuBar = std::make_unique<TopMenuBar>();
    toolPalette = std::make_unique<ToolPalette>();
    canvas = std::make_unique<PluginEditorCanvas>();
    
    // Add components to the layout
    addAndMakeVisible(*topMenuBar);
    addAndMakeVisible(*toolPalette);
    addAndMakeVisible(*canvas);
    
    // Connect signals and slots
    topMenuBar->onExportPlugin = [this]() { exportPlugin(); };
    topMenuBar->onNewProject = [this]() { newProject(); };
    topMenuBar->onOpenProject = [this]() { openProject(); };
    topMenuBar->onSaveProject = [this]() { saveProject(); };
    
    toolPalette->onToolSelected = [this](ComponentType type) {
        // Handle tool selection
        currentTool = type;
    };
    
    // Configure window
    setSize(1200, 800);
    setWantsKeyboardFocus(true);
}

MainComponent::~MainComponent()
{
    // Clean up resources
}

void MainComponent::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(Features::backgroundColor);
    
    // Draw logo in top left corner
    if (!logo.isNull())
    {
        g.drawImageAt(logo, 10, 10);
        
        // Draw app name next to logo
        g.setColour(juce::Colours::white);
        // Fix deprecated Font constructor
        juce::Font textFont(juce::Font::getDefaultSansSerifFontName(), 18.0f, juce::Font::bold);
        g.setFont(textFont);
        g.drawText("The Plugin Lab", 60, 10, 200, 40, juce::Justification::centredLeft, true);
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Top menu bar at the top
    topMenuBar->setBounds(bounds.removeFromTop(Features::toolbarHeight));
    
    // Tool palette on the left
    toolPalette->setBounds(bounds.removeFromLeft(60));
    
    // Canvas takes the rest
    canvas->setBounds(bounds);
}

void MainComponent::exportPlugin()
{
    // Placeholder for plugin export
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, 
                                         "Export Plugin", 
                                         "Plugin export functionality will be implemented soon.");
}

void MainComponent::newProject()
{
    // Create new project
    canvas->clear();
}

void MainComponent::openProject()
{
    // Open project file dialog
    fileChooser = std::make_unique<juce::FileChooser>(
        "Open Project", juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.plproj");
        
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | 
                           juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file.existsAsFile())
            {
                // Load project from file
                loadProject(file);
            }
        });
}

void MainComponent::saveProject()
{
    if (currentProjectFile.existsAsFile())
    {
        // Save to existing file
        saveProjectToFile(currentProjectFile);
    }
    else
    {
        // Choose where to save
        fileChooser = std::make_unique<juce::FileChooser>(
            "Save Project", juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.plproj");
            
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | 
                               juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser)
            {
                auto file = chooser.getResult();
                if (file != juce::File{})
                {
                    // Ensure proper extension
                    currentProjectFile = file.withFileExtension("plproj");
                    saveProjectToFile(currentProjectFile);
                }
            });
    }
}

void MainComponent::saveProjectToFile(const juce::File& file)
{
    // Create project data directly as ValueTree
    juce::ValueTree projectData("PLUGIN_LAB_PROJECT");
    
    // Save canvas state to project data
    if (canvas != nullptr)
    {
        // Pass the ValueTree directly instead of requiring a PluginProject object
        canvas->saveToProject(projectData);
    }
    
    // Write to file
    if (auto xml = projectData.createXml())
    {
        if (xml->writeTo(file))
        {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, 
                                                "Project Saved", 
                                                "Project saved to: " + file.getFullPathName());
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, 
                                                "Save Error", 
                                                "Failed to save project file.");
        }
    }
}

void MainComponent::loadProject(const juce::File& file)
{
    // Read XML from file
    if (auto xml = juce::XmlDocument::parse(file))
    {
        // Convert to ValueTree
        juce::ValueTree projectData = juce::ValueTree::fromXml(*xml);
        
        if (projectData.isValid() && canvas != nullptr)
        {
            // Clear the canvas first
            canvas->clear();
            
            // Load project data directly from the ValueTree
            canvas->loadFromProject(projectData);
            
            // Store the file reference
            currentProjectFile = file;
            
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, 
                                                "Project Loaded", 
                                                "Project loaded from: " + file.getFullPathName());
        }
    }
    else
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, 
                                            "Load Error", 
                                            "Failed to load project file.");
    }
}

} // namespace MainApp
