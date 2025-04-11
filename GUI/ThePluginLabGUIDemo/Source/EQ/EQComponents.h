#pragma once
#include <JuceHeader.h>

// Helper function to find image file
static juce::Image findImageFile(const juce::String& filename)
{
    juce::Array<juce::File> possiblePaths;
    
    // Try multiple locations
    possiblePaths.add(juce::File::getCurrentWorkingDirectory().getChildFile(filename));
    possiblePaths.add(juce::File::getCurrentWorkingDirectory().getChildFile("Resources").getChildFile(filename));
    possiblePaths.add(juce::File::getCurrentWorkingDirectory().getParentDirectory().getChildFile(filename));
    possiblePaths.add(juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                    .getParentDirectory().getChildFile(filename));
    
    // Log possible paths
    juce::String pathsMessage = "Looking for " + filename + " in:";
    for (auto& path : possiblePaths)
    {
        pathsMessage += "\n  " + path.getFullPathName() + (path.existsAsFile() ? " (exists)" : " (not found)");
    }
    juce::Logger::writeToLog(pathsMessage);
    
    // Try to load from any of these locations
    for (auto& path : possiblePaths)
    {
        if (path.existsAsFile())
        {
            juce::Image img = juce::ImageCache::getFromFile(path);
            if (img.isValid())
            {
                juce::Logger::writeToLog("Successfully loaded EQ image from: " + path.getFullPathName());
                return img;
            }
            else
            {
                juce::Logger::writeToLog("File exists but image is invalid: " + path.getFullPathName());
            }
        }
    }
    
    juce::Logger::writeToLog("Failed to load image: " + filename);
    return juce::Image(); // Return an invalid image if not found
}

struct FrequencyComponent : juce::Component
{
    FrequencyComponent()
    {
        image = findImageFile("FrequencyComponent.png");
        
        // Create fallback if image couldn't be loaded
        if (!image.isValid())
        {
            image = juce::Image(juce::Image::RGB, 580, 427, true);
            juce::Graphics g(image);
            g.fillAll(juce::Colours::lightgrey);
            g.setColour(juce::Colours::black);
            g.drawText("Frequency", image.getBounds().reduced(20), juce::Justification::centred, true);
        }
    }

    void paint(juce::Graphics& g) override
    {
        if (image.isValid())
            g.drawImageWithin(image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
    
    juce::Image image;
};

struct GainComponent : juce::Component
{
    GainComponent()
    {
        image = findImageFile("GainComponent.png");
        
        // Create fallback if image couldn't be loaded
        if (!image.isValid())
        {
            image = juce::Image(juce::Image::RGB, 580, 427, true);
            juce::Graphics g(image);
            g.fillAll(juce::Colours::lightgrey);
            g.setColour(juce::Colours::black);
            g.drawText("Gain", image.getBounds().reduced(20), juce::Justification::centred, true);
        }
    }

    void paint(juce::Graphics& g) override
    {
        if (image.isValid())
            g.drawImageWithin(image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
    
    juce::Image image;
};

struct HighpassComponent : juce::Component
{
    HighpassComponent()
    {
        image = findImageFile("HighpassComponent.png");
        
        // Create fallback if image couldn't be loaded
        if (!image.isValid())
        {
            image = juce::Image(juce::Image::RGB, 580, 427, true);
            juce::Graphics g(image);
            g.fillAll(juce::Colours::lightgrey);
            g.setColour(juce::Colours::black);
            g.drawText("Highpass", image.getBounds().reduced(20), juce::Justification::centred, true);
        }
    }

    void paint(juce::Graphics& g) override
    {
        if (image.isValid())
            g.drawImageWithin(image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
    
    juce::Image image;
};

struct LowpassComponent : juce::Component
{
    LowpassComponent()
    {
        image = findImageFile("LowpassComponent.png");
        
        // Create fallback if image couldn't be loaded
        if (!image.isValid())
        {
            image = juce::Image(juce::Image::RGB, 580, 427, true);
            juce::Graphics g(image);
            g.fillAll(juce::Colours::lightgrey);
            g.setColour(juce::Colours::black);
            g.drawText("Lowpass", image.getBounds().reduced(20), juce::Justification::centred, true);
        }
    }

    void paint(juce::Graphics& g) override
    {
        if (image.isValid())
            g.drawImageWithin(image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
    
    juce::Image image;
};

struct PeakingComponent : juce::Component
{
    PeakingComponent()
    {
        image = findImageFile("PeakingComponent.png");
        
        // Create fallback if image couldn't be loaded
        if (!image.isValid())
        {
            image = juce::Image(juce::Image::RGB, 580, 427, true);
            juce::Graphics g(image);
            g.fillAll(juce::Colours::lightgrey);
            g.setColour(juce::Colours::black);
            g.drawText("Peaking", image.getBounds().reduced(20), juce::Justification::centred, true);
        }
    }

    void paint(juce::Graphics& g) override
    {
        if (image.isValid())
            g.drawImageWithin(image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
    
    juce::Image image;
};

struct QComponent : juce::Component
{
    QComponent()
    {
        image = findImageFile("QComponent.png");
        
        // Create fallback if image couldn't be loaded
        if (!image.isValid())
        {
            image = juce::Image(juce::Image::RGB, 580, 427, true);
            juce::Graphics g(image);
            g.fillAll(juce::Colours::lightgrey);
            g.setColour(juce::Colours::black);
            g.drawText("Q", image.getBounds().reduced(20), juce::Justification::centred, true);
        }
    }

    void paint(juce::Graphics& g) override
    {
        if (image.isValid())
            g.drawImageWithin(image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
    
    juce::Image image;
};
