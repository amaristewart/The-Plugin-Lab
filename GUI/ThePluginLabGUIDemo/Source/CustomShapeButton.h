#pragma once

#include <JuceHeader.h>

// Base class for custom shape buttons
class CustomShapeButton : public juce::TextButton
{
public:
    CustomShapeButton(const juce::String& buttonName)
        : juce::TextButton(buttonName)
    {
        setClickingTogglesState(true);
    }
};

//==============================================================================
// Circle shaped button
class CircleButton : public CustomShapeButton
{
public:
    CircleButton(const juce::String& buttonName)
        : CustomShapeButton(buttonName)
    {
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        auto baseColour = findColour(getToggleState() ? buttonOnColourId : buttonColourId);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.2f);

        g.setColour(baseColour);
        g.fillEllipse(bounds);
        
        g.setColour(findColour(getToggleState() ? textColourOnId : textColourOffId));
        g.setFont(getHeight() * 0.6f);
        g.drawText(getButtonText(), bounds, juce::Justification::centred, true);
    }
};

//==============================================================================
// Square button with rounded corners
class RoundedSquareButton : public CustomShapeButton
{
public:
    RoundedSquareButton(const juce::String& buttonName)
        : CustomShapeButton(buttonName)
    {
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        auto baseColour = findColour(getToggleState() ? buttonOnColourId : buttonColourId);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.2f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, bounds.getHeight() * 0.2f);
        
        g.setColour(findColour(getToggleState() ? textColourOnId : textColourOffId));
        g.setFont(getHeight() * 0.6f);
        g.drawText(getButtonText(), bounds, juce::Justification::centred, true);
    }
};

//==============================================================================
// Triangle shaped button
class TriangleButton : public CustomShapeButton
{
public:
    TriangleButton(const juce::String& buttonName)
        : CustomShapeButton(buttonName)
    {
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(4.0f);
        auto baseColour = findColour(getToggleState() ? buttonOnColourId : buttonColourId);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.2f);

        g.setColour(baseColour);
        
        juce::Path triangle;
        triangle.addTriangle(bounds.getX(), bounds.getBottom(),
                         bounds.getCentreX(), bounds.getY(),
                         bounds.getRight(), bounds.getBottom());
        g.fillPath(triangle);
        
        g.setColour(findColour(getToggleState() ? textColourOnId : textColourOffId));
        g.setFont(getHeight() * 0.5f);
        g.drawText(getButtonText(), bounds, juce::Justification::centred, true);
    }
};

//==============================================================================
// Hexagon shaped button
class HexagonButton : public CustomShapeButton
{
public:
    HexagonButton(const juce::String& buttonName)
        : CustomShapeButton(buttonName)
    {
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(4.0f);
        auto baseColour = findColour(getToggleState() ? buttonOnColourId : buttonColourId);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.2f);

        g.setColour(baseColour);
        
        juce::Path hexagon;
        float w = bounds.getWidth();
        float h = bounds.getHeight();
        float x = bounds.getX();
        float y = bounds.getY();
        
        hexagon.startNewSubPath(x + w * 0.25f, y);
        hexagon.lineTo(x + w * 0.75f, y);
        hexagon.lineTo(x + w, y + h * 0.5f);
        hexagon.lineTo(x + w * 0.75f, y + h);
        hexagon.lineTo(x + w * 0.25f, y + h);
        hexagon.lineTo(x, y + h * 0.5f);
        hexagon.closeSubPath();
        
        g.fillPath(hexagon);
        
        g.setColour(findColour(getToggleState() ? textColourOnId : textColourOffId));
        g.setFont(getHeight() * 0.5f);
        g.drawText(getButtonText(), bounds, juce::Justification::centred, true);
    }
};
