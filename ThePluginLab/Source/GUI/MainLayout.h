#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"
#include "../Components/ConcretePluginNode.h"
#include "../Audio/Graphs/AudioProcessingGraph.h"
#include "PluginEditorCanvas.h"
#include "../GUI/ComponentPanel.h"
#include "../Common/Features.h"
#include "../Components/ConnectionComponent.h"

// Forward declarations
class PluginEditorCanvas;
class ComponentPanel;
class PluginNodeComponent;
class ConcretePluginNode;
class AudioConnectionPoint;
class ConnectionComponent;
class AudioProcessingGraph;

/**
 * Main layout component that holds the editor canvas and component panel
 */
class MainLayout : public juce::Component
{
public:
    MainLayout();
    ~MainLayout() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    std::unique_ptr<PluginEditorCanvas> canvas;
    std::unique_ptr<ComponentPanel> componentPanel;
    
    // Handler methods
    void createNode(ComponentType type);
    void handleNodeAdded(PluginNodeComponent* node);
    void handleNodeRemoved(PluginNodeComponent* node);
    void handleConnectionMade(AudioConnectionPoint* source, AudioConnectionPoint* destination);
    void handleConnectionRemoved(ConnectionComponent* connection);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLayout)
};
