#pragma once

namespace Features
{
    // Color scheme
    const juce::Colour backgroundColor = juce::Colour(0xFF1E1E1E);
    const juce::Colour guiColor = juce::Colour(0xFF61DAFB);     // Blue
    const juce::Colour eqColor = juce::Colour(0xFFFF6B6B);      // Pink
    const juce::Colour compColor = juce::Colour(0xFF4CAF50);    // Green
    
    // Toolbar gradient
    const juce::Colour toolbarGradientStart = juce::Colour(0xFF61DAFB);  // Light blue
    const juce::Colour toolbarGradientEnd = juce::Colour(0xFFFF6B6B);    // Pink
    
    // Component sizes
    const int toolbarHeight = 40;
    const int componentPanelWidth = 200;
    const int categoryHeaderHeight = 30;
    
    // Animation settings
    const float hoverScaleAmount = 1.1f;
    const int animationFPS = 60;
    
    // Grid settings
    const int gridSize = 20;
    const float gridOpacity = 0.1f;
} 