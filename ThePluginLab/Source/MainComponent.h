#pragma once
#include <JuceHeader.h>
#include "GUI/PluginToolbar.h"
#include "GUI/PluginEditorCanvas.h"
#include "GUI/ComponentPanel.h"
#include "Audio/Graphs/AudioProcessingGraph.h"
#include "Export/PluginProject.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    // Audio processing callbacks
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

private:
    void setupCallbacks();
    bool confirmDiscardChanges();
    void saveProject(const juce::File& file);
    void loadProject(const juce::File& file);
    void showErrorMessage(const juce::String& title, const juce::String& message);
    void updateProjectName();

    std::unique_ptr<PluginToolbar> toolbar;
    std::unique_ptr<ComponentPanel> componentPanel;
    std::unique_ptr<PluginEditorCanvas> editorCanvas;
    std::unique_ptr<AudioProcessingGraph> audioGraph;
    
    PluginProject project;
    juce::File currentProjectFile;
    bool hasUnsavedChanges = false;
    std::unique_ptr<juce::Label> projectNameEditor;

    double currentSampleRate = 0.0;
    int currentBlockSize = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
