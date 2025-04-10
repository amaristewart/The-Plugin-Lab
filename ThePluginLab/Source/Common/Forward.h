#pragma once

// Forward declare all major classes that might be involved in circular dependencies

// Components
class PluginNodeComponent;
class ConcretePluginNode;
class AudioConnectionPoint;
class ConnectionComponent;
class DraggableComponent;

// Nodes
class EqualizerNode;
class CompressorNode;
class GuiNode;

// GUI
class PluginEditorCanvas;
class ComponentPanel;
class ToolPalette;
class MainLayout;
class TopMenuBar;
class PluginToolbar;

// Audio
class AudioProcessingGraph;
class PluginAudioProcessor;

// Misc
class PluginProject;
class PluginExporter;

// Forward declarations of all key types to avoid circular dependencies
namespace juce {
    class Component;
    class DragAndDropTarget;
    class AudioProcessor;
}

// IMPORTANT: Remove enum forward declarations - these are fully defined in Types.h
// Keeping PortType since it doesn't conflict
enum class PortType
{
    AudioInput,
    AudioOutput,
    MidiInput,
    MidiOutput,
    ControlInput,
    ControlOutput
};
