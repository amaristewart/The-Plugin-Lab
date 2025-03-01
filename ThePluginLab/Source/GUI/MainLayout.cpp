#include "MainLayout.h"

MainLayout::MainLayout()
{
    // Top bar
    projectNameLabel = std::make_unique<juce::Label>("Project Name", "Project Name:");
    projectNameLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(projectNameLabel.get());
    
    projectNameEditor = std::make_unique<juce::TextEditor>("Project Name Editor");
    projectNameEditor->setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey);
    projectNameEditor->setColour(juce::TextEditor::textColourId, juce::Colours::white);
    projectNameEditor->setText("Untitled Plugin");
    addAndMakeVisible(projectNameEditor.get());
    
    // Menu bar
    menuBar = std::make_unique<TopMenuBar>();
    addAndMakeVisible(menuBar.get());
    
    // Component panel
    componentPanel = std::make_unique<ComponentPanel>();
    addAndMakeVisible(componentPanel.get());
    
    // Canvas
    canvas = std::make_unique<PluginEditorCanvas>();
    addAndMakeVisible(canvas.get());
    
    setSize(1200, 800);
}

MainLayout::~MainLayout() = default;

void MainLayout::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColor);
    
    g.setColour(accentColor1.withAlpha(0.1f));
    g.fillRoundedRectangle(0.0f, 0.0f, 200.0f, 60.0f, 10.0f);
}

void MainLayout::resized()
{
    auto bounds = getLocalBounds();
    
    // Top bar elements
    auto topBar = bounds.removeFromTop(60);
    menuBar->setBounds(topBar);
    
    // Project name controls
    auto nameArea = topBar.removeFromRight(300);
    projectNameLabel->setBounds(nameArea.removeFromLeft(100));
    projectNameEditor->setBounds(nameArea.reduced(5));
    
    // Component panel and canvas
    auto contentArea = bounds;
    componentPanel->setBounds(contentArea.removeFromLeft(250));
    canvas->setBounds(contentArea);
} 
