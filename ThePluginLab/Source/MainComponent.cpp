#include "MainComponent.h"
#include "GUI/PluginToolbar.h"
#include "GUI/PluginEditorCanvas.h"
#include "Components/ConnectionComponent.h"
#include "Connections/AudioConnectionPoint.h"
#include "Audio/Graphs/AudioProcessingGraph.h"
#include "Nodes/EqualizerNode.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Toolbar with logo and menus
    toolbar = std::make_unique<PluginToolbar>();
    addAndMakeVisible(toolbar.get());
    
    // Component panel
    componentPanel = std::make_unique<ComponentPanel>();
    addAndMakeVisible(componentPanel.get());
    
    // Editor canvas
    editorCanvas = std::make_unique<PluginEditorCanvas>();
    addAndMakeVisible(editorCanvas.get());
    
    // Project name editor
    projectNameEditor = std::make_unique<juce::Label>("Project Name", "Untitled Plugin");
    projectNameEditor->setFont(juce::Font(18.0f, juce::Font::bold));
    projectNameEditor->setColour(juce::Label::textColourId, juce::Colours::white);
    projectNameEditor->setEditable(true);
    projectNameEditor->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(projectNameEditor.get());
    
    // Audio processing graph
    audioGraph = std::make_unique<AudioProcessingGraph>();
    
    // Toolbar callback
    toolbar->onPluginSelected = [this](NodeType type)
    {
        auto newNode = (type == NodeType::Equalizer) ?
            std::make_unique<EqualizerNode>() : nullptr;
            
        if (newNode != nullptr)
        {
            editorCanvas->addNode(std::move(newNode));
        }
    };
    
    editorCanvas->onNodeAdded = [this](PluginNodeComponent* node)
    {
        if (auto* processor = node->getProcessor())
        {
            // Add node to audio graph
            audioGraph->addNode(node);
            
            // Prepare the processor with current audio settings
            processor->prepareToPlay(currentSampleRate, currentBlockSize);
        }
    };
    
    editorCanvas->onNodeRemoved = [this](PluginNodeComponent* node)
    {
        // Remove node from audio graph
        audioGraph->removeNode(node);
    };
    
    editorCanvas->onConnectionMade = [this](AudioConnectionPoint* source, AudioConnectionPoint* destination)
    {
        // Connect nodes in audio graph
        auto& sourceNode = source->getOwner();
        auto& destNode = destination->getOwner();
        
        audioGraph->connectNodes(&sourceNode,
                               source->getPortIndex(),
                               &destNode,
                               destination->getPortIndex());
    };
    
    editorCanvas->onConnectionRemoved = [this](ConnectionComponent* connection)
    {
        // Disconnect nodes in audio graph
        auto* sourcePoint = connection->getSourcePoint();
        auto* destPoint = connection->getDestinationPoint();
        
        auto& sourceNode = sourcePoint->getOwner();
        auto& destNode = destPoint->getOwner();
        
        audioGraph->disconnectNodes(&sourceNode,
                                  sourcePoint->getPortIndex(),
                                  &destNode,
                                  destPoint->getPortIndex());
    };
    
    audioGraph->onProcessingChainChanged = [this]()
    {
        // Update visualizers or other UI elements that depend on the processing chain
        repaint();
    };
    
    setAudioChannels(2, 2);
    
    setupCallbacks();
    setSize(1200, 800);
}

MainComponent::~MainComponent()
{
    // Shut down audio before destroying components
    shutdownAudio();
    
    // Components will be automatically destroyed
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlockExpected;
    
    // Initialize the audio graph with the current settings
    if (audioGraph)
    {
        audioGraph->prepareToPlay(sampleRate, samplesPerBlockExpected);
    }
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Pass through the audio if no processing is needed
    if (!audioGraph || !audioGraph->hasActiveNodes())
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    // Process audio through the graph
    juce::AudioBuffer<float>& buffer = *bufferToFill.buffer;
    juce::MidiBuffer midiBuffer;
    
    // Make sure we don't process null buffers
    if (buffer.getNumChannels() > 0 && buffer.getNumSamples() > 0)
    {
        audioGraph->processBlock(buffer, midiBuffer);
    }
}

void MainComponent::releaseResources()
{
    if (audioGraph)
    {
        audioGraph->releaseResources();
    }
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Layout order is important for z-ordering
    if (editorCanvas)
    {
        editorCanvas->setBounds(bounds);
        editorCanvas->setAlwaysOnTop(false);
    }
    
    if (toolbar)
    {
        auto topBounds = bounds.removeFromTop(40);
        toolbar->setBounds(topBounds);
        toolbar->setAlwaysOnTop(true);
    }
    
    if (componentPanel)
    {
        auto leftBounds = bounds.removeFromLeft(200);
        componentPanel->setBounds(leftBounds);
        componentPanel->setAlwaysOnTop(true);
    }
}

void MainComponent::setupCallbacks()
{
    // Menu callbacks
    toolbar->onNewProject = [this]()
    {
        if (confirmDiscardChanges())
        {
            project.clear();
            editorCanvas->clear();
            updateProjectName();
        }
    };
    
    toolbar->onOpenProject = [this]()
    {
        if (confirmDiscardChanges())
        {
            auto chooser = std::make_unique<juce::FileChooser>(
                "Open Project",
                juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                "*.plproj");
                
            chooser->launchAsync(juce::FileBrowserComponent::openMode |
                               juce::FileBrowserComponent::canSelectFiles,
                [this](const juce::FileChooser& fc)
                {
                    if (fc.getResult().existsAsFile())
                    {
                        loadProject(fc.getResult());
                    }
                });
        }
    };
    
    toolbar->onSaveProject = [this]()
    {
        if (currentProjectFile.existsAsFile())
            saveProject(currentProjectFile);
        else
            toolbar->onSaveProjectAs();
    };
    
    toolbar->onSaveProjectAs = [this]()
    {
        auto chooser = std::make_unique<juce::FileChooser>(
            "Save Project",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.plproj");
            
        chooser->launchAsync(juce::FileBrowserComponent::saveMode |
                           juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.hasFileExtension("plproj"))
                    saveProject(file);
                else
                    saveProject(file.withFileExtension("plproj"));
            });
    };
    
    toolbar->onLoadTemplate = [this](const juce::String& templateName)
    {
        if (confirmDiscardChanges())
        {
            if (project.loadTemplate(templateName))
            {
                editorCanvas->loadFromProject(project);
                updateProjectName();
            }
        }
    };
}

bool MainComponent::confirmDiscardChanges()
{
    if (!hasUnsavedChanges)
        return true;
        
    int result = juce::NativeMessageBox::showYesNoCancelBox(
        juce::MessageBoxIconType::QuestionIcon,
        "Unsaved Changes",
        "You have unsaved changes. Do you want to continue and discard them?",
        this,
        nullptr);
        
    return result == 1;  // 1 = Yes, 0 = No, -1 = Cancel
}

void MainComponent::showErrorMessage(const juce::String& title, const juce::String& message)
{
    juce::NativeMessageBox::showMessageBoxAsync(
        juce::MessageBoxIconType::WarningIcon,
        title,
        message,
        this);
}

void MainComponent::saveProject(const juce::File& file)
{
    editorCanvas->saveToProject(project);
    
    if (project.saveToFile(file))
    {
        currentProjectFile = file;
        hasUnsavedChanges = false;
        updateProjectName();
    }
    else
    {
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Error",
            "Failed to save project file.",
            this);
    }
}

void MainComponent::updateProjectName()
{
    auto name = project.getName();
    if (hasUnsavedChanges)
        name += "*";
    projectNameEditor->setText(name, juce::dontSendNotification);
}

void MainComponent::loadProject(const juce::File& file)
{
    if (project.loadFromFile(file))
    {
        currentProjectFile = file;
        editorCanvas->loadFromProject(project);
        updateProjectName();
    }
    else
    {
        showErrorMessage("Error", "Failed to load project file.");
    }
}

void MainComponent::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    audioGraph->processBlock(buffer, midiMessages);
}
