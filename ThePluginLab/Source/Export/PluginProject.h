#pragma once
#include <JuceHeader.h>
#include <vector>
#include <string>

/**
 * Represents a plugin project that can be exported
 */
class PluginProject
{
public:
    PluginProject()
    {
        // Default settings
        name = "MyPlugin";
        description = "A plugin created with The Plugin Lab";
        manufacturer = "YourName";
        pluginCode = "Plal";
        manufacturerCode = "Yrmf";
        version = "1.0.0";
        formatFlags = 3; // VST3 + AU
    }
    
    // Project metadata
    juce::String name;
    juce::String description;
    juce::String manufacturer;
    juce::String pluginCode;
    juce::String manufacturerCode;
    juce::String version;
    int formatFlags;
    
    // Plugin settings
    bool isSynth = false;
    bool needsMidiInput = false;
    bool needsMidiOutput = false;
    int numInputs = 2;
    int numOutputs = 2;
    
    // Plugin state
    juce::MemoryBlock processorState;
    
    // Project management
    void clear()
    {
        // Reset to default values
        name = "MyPlugin";
        description = "A plugin created with The Plugin Lab";
        processorState.reset();
    }
    
    juce::String getName() const { return name; }
    
    // Template loading
    bool loadTemplate(const juce::String& templateName)
    {
        // Load a predefined template
        if (templateName == "Basic EQ")
        {
            name = "Basic EQ";
            description = "A simple equalizer plugin";
            return true;
        }
        else if (templateName == "Basic Compressor")
        {
            name = "Basic Compressor";
            description = "A simple compressor plugin";
            return true;
        }
        
        return false;
    }
    
    // Save to JSON file
    bool saveToFile(const juce::File& file) 
    {
        auto projectObject = new juce::DynamicObject();
        
        // Metadata
        projectObject->setProperty("name", name);
        projectObject->setProperty("description", description);
        projectObject->setProperty("manufacturer", manufacturer);
        projectObject->setProperty("pluginCode", pluginCode);
        projectObject->setProperty("manufacturerCode", manufacturerCode);
        projectObject->setProperty("version", version);
        projectObject->setProperty("formatFlags", formatFlags);
        
        // Plugin settings
        projectObject->setProperty("isSynth", isSynth);
        projectObject->setProperty("needsMidiInput", needsMidiInput);
        projectObject->setProperty("needsMidiOutput", needsMidiOutput);
        projectObject->setProperty("numInputs", numInputs);
        projectObject->setProperty("numOutputs", numOutputs);
        
        // Processor state
        if (processorState.getSize() > 0)
            projectObject->setProperty("processorState", processorState.toBase64Encoding());
        
        juce::var projectVar(projectObject);
        juce::String jsonString = juce::JSON::toString(projectVar);
        
        if (file.replaceWithText(jsonString))
            return true;
        
        return false;
    }
    
    // Load from JSON file
    bool loadFromFile(const juce::File& file)
    {
        if (!file.exists())
            return false;
            
        juce::String jsonContent = file.loadFileAsString();
        auto result = juce::JSON::parse(jsonContent);
        
        if (result.isObject())
        {
            auto obj = result.getDynamicObject();
            
            // Metadata
            if (obj->hasProperty("name")) name = obj->getProperty("name").toString();
            if (obj->hasProperty("description")) description = obj->getProperty("description").toString();
            if (obj->hasProperty("manufacturer")) manufacturer = obj->getProperty("manufacturer").toString();
            if (obj->hasProperty("pluginCode")) pluginCode = obj->getProperty("pluginCode").toString();
            if (obj->hasProperty("manufacturerCode")) manufacturerCode = obj->getProperty("manufacturerCode").toString();
            if (obj->hasProperty("version")) version = obj->getProperty("version").toString();
            if (obj->hasProperty("formatFlags")) formatFlags = obj->getProperty("formatFlags");
            
            // Plugin settings
            if (obj->hasProperty("isSynth")) isSynth = obj->getProperty("isSynth");
            if (obj->hasProperty("needsMidiInput")) needsMidiInput = obj->getProperty("needsMidiInput");
            if (obj->hasProperty("needsMidiOutput")) needsMidiOutput = obj->getProperty("needsMidiOutput");
            if (obj->hasProperty("numInputs")) numInputs = obj->getProperty("numInputs");
            if (obj->hasProperty("numOutputs")) numOutputs = obj->getProperty("numOutputs");
            
            // Processor state
            if (obj->hasProperty("processorState"))
            {
                juce::String stateBase64 = obj->getProperty("processorState").toString();
                processorState.fromBase64Encoding(stateBase64);
            }
            
            return true;
        }
        
        return false;
    }
    
    // Getters and setters for processor state
    juce::MemoryBlock getProcessorState() const { return processorState; }
    void setProcessorState(const juce::MemoryBlock& state) { processorState = state; }
    
    // Generate JUCE project files
    bool generateProjectFiles(const juce::File& directory)
    {
        // Implementation would generate Projucer files or CMake files
        return false; // Placeholder
    }
    
    // Export compiled plugin
    bool exportPlugin(const juce::File& destinationDirectory, const juce::Array<int>& formats)
    {
        // Implementation would build and package the plugin
        return false; // Placeholder
    }
};
