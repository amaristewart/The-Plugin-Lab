#pragma once
#include <JuceHeader.h>

/**
 * Defines constants and features for the application
 */
namespace Features
{
    // Component sizes
    inline constexpr int toolbarHeight = 40;
    inline constexpr int componentPanelWidth = 200;
    inline constexpr int categoryHeaderHeight = 30;
    
    // Grid settings
    inline constexpr int gridSize = 20;
    inline constexpr float gridOpacity = 0.1f;
    
    // Color scheme
    inline const juce::Colour guiColor = juce::Colour(0xFF61DAFB);     // Blue
    inline const juce::Colour eqColor = juce::Colour(0xFFFF6B6B);      // Pink
    inline const juce::Colour compColor = juce::Colour(0xFF4CAF50);    // Green
    
    // Background colors
    inline const juce::Colour backgroundColor = juce::Colour(0xFF1E1E1E);
    inline const juce::Colour canvasBackgroundColor = juce::Colour(0xFF121212);
    
    // Toolbar colors
    inline const juce::Colour toolbarGradientStart = juce::Colour(0xFF61DAFB);  // Light blue
    inline const juce::Colour toolbarGradientEnd = juce::Colour(0xFFFF6B6B);    // Pink
    
    // Connection colors
    inline const juce::Colour connectionColor = juce::Colours::white;
    inline const juce::Colour connectionHighlightColor = juce::Colours::orange;
}