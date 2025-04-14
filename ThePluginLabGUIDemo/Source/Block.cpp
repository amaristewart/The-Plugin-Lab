#include "Block.h"

Block::Block()
    : name("Untitled Block"), type("Unknown")
{
    // Default constructor implementation
}

Block::Block(const juce::String& blockName, const juce::String& blockType)
    : name(blockName), type(blockType)
{
    // Named constructor implementation
}

Block::~Block()
{
    // Destructor implementation
    // Clean up any resources if needed
}
