#pragma once

#include <JuceHeader.h>

/**
 * Class representing a Block in the audio processing chain.
 * This could be an effect, generator, or another audio processing unit.
 */
class Block
{
public:
    //==============================================================================
    Block();
    Block(const juce::String& name, const juce::String& type);
    ~Block();
    
    //==============================================================================
    /** Returns the name of the block */
    juce::String getName() const { return name; }
    
    /** Sets the name of the block */
    void setName(const juce::String& newName) { name = newName; }
    
    /** Returns the type of the block */
    juce::String getType() const { return type; }
    
    /** Gets the ID of the block */
    int getId() const { return id; }
    
    /** Sets the ID of the block */
    void setId(int newId) { id = newId; }
    
    /** Gets the position of the block in the editor */
    juce::Point<int> getPosition() const { return position; }
    
    /** Sets the position of the block in the editor */
    void setPosition(juce::Point<int> newPosition) { position = newPosition; }
    
    /** Gets a list of parameters for this block */
    juce::Array<juce::var> getParameters() const { return parameters; }
    
    /** Sets parameters for this block */
    void setParameters(const juce::Array<juce::var>& newParameters) { parameters = newParameters; }
    
    /** Adds a parameter to this block */
    void addParameter(const juce::var& parameter) { parameters.add(parameter); }
    
private:
    //==============================================================================
    juce::String name;
    juce::String type;
    int id = 0;
    juce::Point<int> position;
    juce::Array<juce::var> parameters;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Block)
};
