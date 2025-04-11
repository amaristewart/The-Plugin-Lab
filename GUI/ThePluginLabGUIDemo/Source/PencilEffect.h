#pragma once

#include <JuceHeader.h>

/**
 * A utility class that provides methods to create pencil-drawn, sketchy effects
 * for various graphical elements used throughout the application.
 */
class PencilEffect
{
public:
    /** Draws a sketchy line between two points with pencil-like appearance */
    static void drawSketchyLine(juce::Graphics& g, float startX, float startY, float endX, float endY, 
                               float thickness = 1.0f, juce::Colour colour = juce::Colours::black)
    {
        juce::Random random;
        
        // Save original color settings
        juce::Graphics::ScopedSaveState saveState(g);
        
        // Draw multiple overlapping strokes with varying opacity and slight offsets
        for (int i = 0; i < 3; ++i)
        {
            float alpha = (i == 0) ? 0.7f : (i == 1 ? 0.5f : 0.3f);
            g.setColour(colour.withAlpha(alpha));
            
            juce::Path stroke;
            stroke.startNewSubPath(startX + random.nextFloat() * 1.0f - 0.5f, 
                                startY + random.nextFloat() * 1.0f - 0.5f);
            
            // Calculate the number of segments based on line length
            float dx = endX - startX;
            float dy = endY - startY;
            float length = std::sqrt(dx * dx + dy * dy);
            int segments = juce::jmax(4, static_cast<int>(length / 3.0f));
            
            // Create a jagged path with more pronounced randomness
            for (int j = 1; j <= segments; ++j)
            {
                float t = static_cast<float>(j) / segments;
                float jitter = (thickness * 0.8f) * (1.0f - t * t); // More jitter at start
                
                float x = startX + dx * t + random.nextFloat() * jitter - jitter/2;
                float y = startY + dy * t + random.nextFloat() * jitter - jitter/2;
                
                stroke.lineTo(x, y);
            }
            
            // Add some small "pencil scratch" deviations
            for (int j = 0; j < 2; ++j)
            {
                float t = random.nextFloat() * 0.6f + 0.2f; // Position along the line (20-80%)
                float scratchSize = random.nextFloat() * thickness * 0.8f;
                float angle = random.nextFloat() * juce::MathConstants<float>::twoPi;
                
                float baseX = startX + dx * t;
                float baseY = startY + dy * t;
                
                float scratchX = baseX + std::cos(angle) * scratchSize;
                float scratchY = baseY + std::sin(angle) * scratchSize;
                
                stroke.lineTo(scratchX, scratchY);
                stroke.lineTo(baseX + random.nextFloat() * thickness - thickness/2, 
                           baseY + random.nextFloat() * thickness - thickness/2);
            }
            
            // End at the destination with slight randomness
            stroke.lineTo(endX + random.nextFloat() * 0.5f - 0.25f, 
                        endY + random.nextFloat() * 0.5f - 0.25f);
            
            // Draw with varying thickness
            float strokeThickness = thickness * (i == 0 ? 1.0f : (i == 1 ? 0.7f : 0.4f));
            g.strokePath(stroke, juce::PathStrokeType(strokeThickness, juce::PathStrokeType::curved));
        }
    }
    
    /** Draws a sketchy rectangle with pencil-like appearance */
    static void drawSketchyRect(juce::Graphics& g, juce::Rectangle<float> rect, 
                               float thickness = 1.0f, juce::Colour colour = juce::Colours::black)
    {
        // Draw with intentional imperfections in the corners for a more hand-drawn look
        juce::Random random;
        
        // For truly hand-drawn look, slightly adjust each corner randomly
        float topLeftX = rect.getX() + random.nextFloat() * thickness * 0.5f - thickness * 0.25f;
        float topLeftY = rect.getY() + random.nextFloat() * thickness * 0.5f - thickness * 0.25f;
        
        float topRightX = rect.getRight() + random.nextFloat() * thickness * 0.5f - thickness * 0.25f;
        float topRightY = rect.getY() + random.nextFloat() * thickness * 0.5f - thickness * 0.25f;
        
        float bottomRightX = rect.getRight() + random.nextFloat() * thickness * 0.5f - thickness * 0.25f;
        float bottomRightY = rect.getBottom() + random.nextFloat() * thickness * 0.5f - thickness * 0.25f;
        
        float bottomLeftX = rect.getX() + random.nextFloat() * thickness * 0.5f - thickness * 0.25f;
        float bottomLeftY = rect.getBottom() + random.nextFloat() * thickness * 0.5f - thickness * 0.25f;
        
        // Draw each edge with extra jitter
        drawSketchyLine(g, topLeftX, topLeftY, topRightX, topRightY, thickness, colour); // top
        drawSketchyLine(g, topRightX, topRightY, bottomRightX, bottomRightY, thickness, colour); // right
        drawSketchyLine(g, bottomRightX, bottomRightY, bottomLeftX, bottomLeftY, thickness, colour); // bottom
        drawSketchyLine(g, bottomLeftX, bottomLeftY, topLeftX, topLeftY, thickness, colour); // left
    }
    
    /** Draws a sketchy rounded rectangle with pencil-like appearance */
    static void drawSketchyRoundedRect(juce::Graphics& g, juce::Rectangle<float> rect, 
                                      float cornerSize, float thickness = 1.0f, 
                                      juce::Colour colour = juce::Colours::black)
    {
        juce::Random random;
        
        // For a truly hand-drawn rounded rect, we'll draw each component separately
        float left = rect.getX();
        float right = rect.getRight();
        float top = rect.getY();
        float bottom = rect.getBottom();
        
        // Add random variations to make it look sketched
        float variation = thickness * 0.5f;
        
        // Top-left corner
        juce::Path topLeftCorner;
        float tlStartX = left + cornerSize + random.nextFloat() * variation - variation/2;
        float tlStartY = top + random.nextFloat() * variation - variation/2;
        topLeftCorner.startNewSubPath(tlStartX, tlStartY);
        topLeftCorner.quadraticTo(
            left + random.nextFloat() * variation - variation/2,
            top + random.nextFloat() * variation - variation/2,
            left + random.nextFloat() * variation - variation/2,
            top + cornerSize + random.nextFloat() * variation - variation/2
        );
        
        // Top-right corner
        juce::Path topRightCorner;
        float trStartX = right - cornerSize + random.nextFloat() * variation - variation/2;
        float trStartY = top + random.nextFloat() * variation - variation/2;
        topRightCorner.startNewSubPath(trStartX, trStartY);
        topRightCorner.quadraticTo(
            right + random.nextFloat() * variation - variation/2,
            top + random.nextFloat() * variation - variation/2,
            right + random.nextFloat() * variation - variation/2,
            top + cornerSize + random.nextFloat() * variation - variation/2
        );
        
        // Bottom-right corner
        juce::Path bottomRightCorner;
        float brStartX = right + random.nextFloat() * variation - variation/2;
        float brStartY = bottom - cornerSize + random.nextFloat() * variation - variation/2;
        bottomRightCorner.startNewSubPath(brStartX, brStartY);
        bottomRightCorner.quadraticTo(
            right + random.nextFloat() * variation - variation/2,
            bottom + random.nextFloat() * variation - variation/2,
            right - cornerSize + random.nextFloat() * variation - variation/2,
            bottom + random.nextFloat() * variation - variation/2
        );
        
        // Bottom-left corner
        juce::Path bottomLeftCorner;
        float blStartX = left + cornerSize + random.nextFloat() * variation - variation/2;
        float blStartY = bottom + random.nextFloat() * variation - variation/2;
        bottomLeftCorner.startNewSubPath(blStartX, blStartY);
        bottomLeftCorner.quadraticTo(
            left + random.nextFloat() * variation - variation/2,
            bottom + random.nextFloat() * variation - variation/2,
            left + random.nextFloat() * variation - variation/2,
            bottom - cornerSize + random.nextFloat() * variation - variation/2
        );
        
        // Draw all corners with sketchy effect
        drawSketchyPath(g, topLeftCorner, thickness, colour);
        drawSketchyPath(g, topRightCorner, thickness, colour);
        drawSketchyPath(g, bottomRightCorner, thickness, colour);
        drawSketchyPath(g, bottomLeftCorner, thickness, colour);
        
        // Draw the straight segments with sketchy lines
        drawSketchyLine(g, tlStartX, tlStartY, trStartX, trStartY, thickness, colour); // top line
        drawSketchyLine(g, brStartX, brStartY, right - cornerSize, bottom, thickness, colour); // right line
        drawSketchyLine(g, blStartX, blStartY, left + cornerSize, bottom, thickness, colour); // bottom line
        drawSketchyLine(g, left, top + cornerSize, left, bottom - cornerSize, thickness, colour); // left line
    }
    
    /** Fills a rectangle with a sketchy appearance */
    static void fillSketchyRect(juce::Graphics& g, juce::Rectangle<float> rect, 
                               juce::Colour colour = juce::Colours::black)
    {
        juce::Random random;
        
        // First, create a solid but semi-transparent fill
        g.setColour(colour.withAlpha(0.2f));
        g.fillRect(rect);
        
        // Then add sketchy cross-hatching with varying density
        g.setColour(colour.withAlpha(0.05f));
        
        // Create hatching pattern - diagonal lines in both directions
        int hatchCount = juce::jmax(10, static_cast<int>(rect.getWidth() * rect.getHeight() / 300.0f));
        
        // First set of diagonal lines (top-left to bottom-right)
        for (int i = 0; i < hatchCount; ++i)
        {
            float x1 = rect.getX() + random.nextFloat() * rect.getWidth() * 0.3f;
            float y1 = rect.getY() + random.nextFloat() * rect.getHeight() * 0.3f;
            float x2 = x1 + rect.getWidth() * (0.5f + random.nextFloat() * 0.5f);
            float y2 = y1 + rect.getHeight() * (0.5f + random.nextFloat() * 0.5f);
            
            // Adjust points to stay within rectangle
            x2 = juce::jmin(x2, rect.getRight());
            y2 = juce::jmin(y2, rect.getBottom());
            
            drawSketchyLine(g, x1, y1, x2, y2, 0.7f, colour.withAlpha(0.07f));
        }
        
        // Second set of diagonal lines (top-right to bottom-left)
        for (int i = 0; i < hatchCount; ++i)
        {
            float x1 = rect.getRight() - random.nextFloat() * rect.getWidth() * 0.3f;
            float y1 = rect.getY() + random.nextFloat() * rect.getHeight() * 0.3f;
            float x2 = x1 - rect.getWidth() * (0.5f + random.nextFloat() * 0.5f);
            float y2 = y1 + rect.getHeight() * (0.5f + random.nextFloat() * 0.5f);
            
            // Adjust points to stay within rectangle
            x2 = juce::jmax(x2, rect.getX());
            y2 = juce::jmin(y2, rect.getBottom());
            
            drawSketchyLine(g, x1, y1, x2, y2, 0.7f, colour.withAlpha(0.07f));
        }
        
        // Add some random spots and dots for texture
        g.setColour(colour.withAlpha(0.03f));
        for (int i = 0; i < hatchCount * 2; ++i)
        {
            float x = rect.getX() + random.nextFloat() * rect.getWidth();
            float y = rect.getY() + random.nextFloat() * rect.getHeight();
            float size = random.nextFloat() * 2.0f + 0.5f;
            g.fillEllipse(x, y, size, size);
        }
    }
    
    /** Draws a sketchy ellipse with pencil-like appearance */
    static void drawSketchyEllipse(juce::Graphics& g, juce::Rectangle<float> rect, 
                                  float thickness = 1.0f, juce::Colour colour = juce::Colours::black)
    {
        juce::Random random;
        
        // Save original settings
        juce::Graphics::ScopedSaveState saveState(g);
        
        // Draw multiple ellipses with slight variations for a sketchy effect
        for (int i = 0; i < 3; ++i)
        {
            // Create subtle variations in the ellipse shape
            float xOffset = random.nextFloat() * thickness - thickness * 0.5f;
            float yOffset = random.nextFloat() * thickness - thickness * 0.5f;
            float widthVar = random.nextFloat() * thickness - thickness * 0.5f;
            float heightVar = random.nextFloat() * thickness - thickness * 0.5f;
            
            juce::Rectangle<float> variedRect(
                rect.getX() + xOffset,
                rect.getY() + yOffset,
                rect.getWidth() + widthVar,
                rect.getHeight() + heightVar
            );
            
            // Add some "wobble" to the ellipse by creating a custom path
            juce::Path ellipsePath;
            float centerX = variedRect.getCentreX();
            float centerY = variedRect.getCentreY();
            float radiusX = variedRect.getWidth() * 0.5f;
            float radiusY = variedRect.getHeight() * 0.5f;
            
            int numPoints = 20; // Number of points to define the ellipse
            float angle = 0.0f;
            float angleIncrement = juce::MathConstants<float>::twoPi / numPoints;
            
            // Start at the rightmost point
            ellipsePath.startNewSubPath(
                centerX + radiusX + random.nextFloat() * thickness - thickness * 0.5f,
                centerY
            );
            
            // Add points around the ellipse with random variations
            for (int j = 1; j <= numPoints; ++j)
            {
                angle += angleIncrement;
                
                float wobble = random.nextFloat() * thickness - thickness * 0.5f;
                float x = centerX + (radiusX + wobble) * std::cos(angle);
                float y = centerY + (radiusY + wobble) * std::sin(angle);
                
                ellipsePath.lineTo(x, y);
            }
            
            // Close the path
            ellipsePath.closeSubPath();
            
            // Draw with varying opacity and thickness
            g.setColour(colour.withAlpha(i == 0 ? 0.8f : i == 1 ? 0.5f : 0.3f));
            g.strokePath(ellipsePath, juce::PathStrokeType(thickness * (i == 0 ? 1.0f : i == 1 ? 0.7f : 0.4f)));
        }
    }
    
    /** Draws any path with a sketchy pencil-like appearance */
    static void drawSketchyPath(juce::Graphics& g, const juce::Path& originalPath, 
                               float thickness = 1.0f, juce::Colour colour = juce::Colours::black)
    {
        juce::Random random;
        
        // Draw multiple overlapping versions with slight variations
        for (int i = 0; i < 3; ++i)
        {
            juce::Path variedPath;
            juce::Path::Iterator iterator(originalPath);
            
            while (iterator.next())
            {
                float jitterAmount = thickness * (0.7f - i * 0.2f);
                
                switch (iterator.elementType)
                {
                    case juce::Path::Iterator::startNewSubPath:
                    {
                        float x = iterator.x1 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float y = iterator.y1 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        variedPath.startNewSubPath(x, y);
                        break;
                    }
                    
                    case juce::Path::Iterator::lineTo:
                    {
                        float x = iterator.x1 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float y = iterator.y1 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        
                        // For longer lines, add intermediate points with more jitter
                        if (variedPath.getCurrentPosition().getDistanceFrom(juce::Point<float>(x, y)) > 20.0f)
                        {
                            juce::Point<float> start = variedPath.getCurrentPosition();
                            juce::Point<float> end(x, y);
                            
                            // Add 1-3 intermediate points
                            int numPoints = random.nextInt(3) + 1;
                            for (int j = 1; j <= numPoints; ++j)
                            {
                                float t = static_cast<float>(j) / (numPoints + 1);
                                float midX = start.x + (end.x - start.x) * t;
                                float midY = start.y + (end.y - start.y) * t;
                                
                                // Add more pronounced jitter to intermediate points
                                midX += random.nextFloat() * jitterAmount * 1.5f - jitterAmount * 0.75f;
                                midY += random.nextFloat() * jitterAmount * 1.5f - jitterAmount * 0.75f;
                                
                                variedPath.lineTo(midX, midY);
                            }
                        }
                        
                        variedPath.lineTo(x, y);
                        break;
                    }
                    
                    case juce::Path::Iterator::quadraticTo:
                    {
                        float x1 = iterator.x1 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float y1 = iterator.y1 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float x2 = iterator.x2 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float y2 = iterator.y2 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        
                        variedPath.quadraticTo(x1, y1, x2, y2);
                        break;
                    }
                    
                    case juce::Path::Iterator::cubicTo:
                    {
                        float x1 = iterator.x1 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float y1 = iterator.y1 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float x2 = iterator.x2 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float y2 = iterator.y2 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float x3 = iterator.x3 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        float y3 = iterator.y3 + random.nextFloat() * jitterAmount - jitterAmount * 0.5f;
                        
                        variedPath.cubicTo(x1, y1, x2, y2, x3, y3);
                        break;
                    }
                    
                    case juce::Path::Iterator::closePath:
                        variedPath.closeSubPath();
                        break;
                }
            }
            
            // Draw with varying opacity and thickness
            g.setColour(colour.withAlpha(i == 0 ? 0.7f : i == 1 ? 0.4f : 0.2f));
            g.strokePath(variedPath, juce::PathStrokeType(thickness * (i == 0 ? 1.0f : i == 1 ? 0.6f : 0.3f)));
            
            // For the first pass, optionally add some "pencil scratches" for texture
            if (i == 0)
            {
                g.setColour(colour.withAlpha(0.2f));
                
                juce::Path::Iterator scratchIterator(originalPath);
                while (scratchIterator.next())
                {
                    if (scratchIterator.elementType == juce::Path::Iterator::startNewSubPath ||
                        scratchIterator.elementType == juce::Path::Iterator::lineTo)
                    {
                        if (random.nextFloat() < 0.3f)  // Only add scratches occasionally
                        {
                            float x = scratchIterator.x1;
                            float y = scratchIterator.y1;
                            
                            // Create a small scratch near this point
                            float scratchLength = random.nextFloat() * 3.0f + 1.0f;
                            float angle = random.nextFloat() * juce::MathConstants<float>::twoPi;
                            
                            float endX = x + std::cos(angle) * scratchLength;
                            float endY = y + std::sin(angle) * scratchLength;
                            
                            g.drawLine(x, y, endX, endY, thickness * 0.4f);
                        }
                    }
                }
            }
        }
    }
    
    /** Draws text with a sketchy, hand-drawn appearance */
    static void drawSketchyText(juce::Graphics& g, const juce::String& text, 
                               float x, float y, float width, float height,
                               juce::Justification justification = juce::Justification::centred,
                               float fontSize = 12.0f, 
                               juce::Colour colour = juce::Colours::black)
    {
        juce::Random random;
        juce::Font font(fontSize);
        
        // First draw a slightly darker base layer
        g.setColour(colour.withAlpha(0.8f));
        g.setFont(font);
        g.drawText(text, x, y, width, height, justification, true);
        
        // Calculate bounds of the text for the current justification
        juce::Rectangle<int> textBounds(static_cast<int>(x), static_cast<int>(y), 
                                       static_cast<int>(width), static_cast<int>(height));
        
        // Create a temporary image to determine actual text bounds
        juce::Image tempImage(juce::Image::ARGB, static_cast<int>(width), static_cast<int>(height), true);
        juce::Graphics tempG(tempImage);
        tempG.setFont(font);
        tempG.setColour(juce::Colours::white);
        tempG.drawText(text, 0, 0, static_cast<int>(width), static_cast<int>(height), justification, true);
        
        // Add multiple semi-transparent overlapping layers with slight offsets for fuzziness
        for (int i = 0; i < 5; ++i)
        {
            float offsetX = random.nextFloat() * 1.0f - 0.5f;
            float offsetY = random.nextFloat() * 1.0f - 0.5f;
            
            g.setColour(colour.withAlpha(0.15f));
            g.drawText(text, x + offsetX, y + offsetY, width, height, justification, true);
        }
        
        // Add scattered dots around text for a pencil texture effect
        g.setColour(colour.withAlpha(0.1f));
        for (int i = 0; i < 40; ++i)
        {
            int dotX = random.nextInt(static_cast<int>(width));
            int dotY = random.nextInt(static_cast<int>(height));
            
            // Only add dots if they're near actual text pixels
            if (tempImage.getPixelAt(dotX, dotY).getBrightness() > 0.1f)
            {
                float dotSize = random.nextFloat() * 0.8f + 0.2f;
                g.fillEllipse(x + dotX, y + dotY, dotSize, dotSize);
            }
        }
        
        // Add some small pencil "scratch" lines near the text edges
        g.setColour(colour.withAlpha(0.15f));
        for (int i = 0; i < 10; ++i)
        {
            int scratchX = random.nextInt(static_cast<int>(width));
            int scratchY = random.nextInt(static_cast<int>(height));
            
            // Only add scratches if they're near text pixels
            if (tempImage.getPixelAt(scratchX, scratchY).getBrightness() > 0.1f)
            {
                float scratchLength = random.nextFloat() * 3.0f + 2.0f;
                float angle = random.nextFloat() * juce::MathConstants<float>::twoPi;
                
                float endX = x + scratchX + std::cos(angle) * scratchLength;
                float endY = y + scratchY + std::sin(angle) * scratchLength;
                
                g.drawLine(x + scratchX, y + scratchY, endX, endY, 0.3f);
            }
        }
    }
};
