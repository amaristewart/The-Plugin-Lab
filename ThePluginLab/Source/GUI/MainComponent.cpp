#include "MainComponent.h"
#include "PluginToolbar.h"
#include "PluginEditorCanvas.h"
#include "../Components/ComponentPanel.h"
#include "../Common/Features.h"

MainComponent::MainComponent()
{
    // Create logo
    logo = juce::ImageCache::getFromMemory(BinaryData::PluginLabLogo_png, BinaryData::PluginLabLogo_pngSize);
    
    // Create main components
    toolbar = std::make_unique<PluginToolbar>();
    componentPanel = std::make_unique<ComponentPanel>();
    canvas = std::make_unique<PluginEditorCanvas>();
    
    // Add components to the layout
    addAndMakeVisible(toolbar.get());
    addAndMakeVisible(componentPanel.get());
    addAndMakeVisible(canvas.get());
    
    // Connect signals and slots
    toolbar->onExportRequest = [this]() { exportPlugin(); };
    toolbar->onNewProject = [this]() { newProject(); };
    toolbar->onOpenProject = [this]() { openProject(); };
    toolbar->onSaveProject = [this]() { saveProject(); };
    
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
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Toolbar at the top
    toolbar->setBounds(bounds.removeFromTop(Features::toolbarHeight));
    
    // Component panel on the left
    componentPanel->setBounds(bounds.removeFromLeft(Features::componentPanelWidth));
    
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
    // Create project data
    juce::ValueTree projectData("PLUGIN_LAB_PROJECT");
    
    // Add project to ValueTree
    canvas->saveToProject(projectData);
    
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
        
        if (projectData.isValid())
        {
            // Load project data
            canvas->clear();
            canvas->loadFromProject(projectData);
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
