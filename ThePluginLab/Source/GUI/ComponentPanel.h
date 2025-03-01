#pragma once
#include <JuceHeader.h>
#include "../Common/Types.h"
#include "../Components/DraggableComponent.h"

class CategorySection : public juce::Component
{
public:
    CategorySection(const juce::String& title, const juce::Colour& headerColor)
        : categoryTitle(title), headerColour(headerColor)
    {
        titleLabel.setText(title, juce::dontSendNotification);
        titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
        titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        titleLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(titleLabel);
        
        description.setFont(juce::Font(13.0f));
        description.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        description.setJustificationType(juce::Justification::topLeft);
        description.setMinimumHorizontalScale(1.0f);
        addAndMakeVisible(description);
        
        setInterceptsMouseClicks(false, true);
        
        componentsContainer.setInterceptsMouseClicks(false, true);
        addAndMakeVisible(componentsContainer);
    }
    
    void setDescription(const juce::String& text)
    {
        description.setText(text, juce::dontSendNotification);
    }
    
    void addComponent(DraggableComponent* component)
    {
        componentsContainer.addAndMakeVisible(component);
        components.add(component);
        resized();
    }
    
    void paint(juce::Graphics& g) override
    {
        // Draw header background
        g.setColour(headerColour);
        g.fillRoundedRectangle(0.0f, 0.0f, getWidth() * 1.0f, 40.0f, 5.0f);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Position title and description - increase description height for better visibility
        titleLabel.setBounds(bounds.removeFromTop(40).reduced(10, 5));
        
        // Increase description height to ensure all text is visible
        int descriptionHeight = 70;  // Increased from 60 to 70
        description.setBounds(bounds.removeFromTop(descriptionHeight).reduced(10, 5));
        
        // Position components in a grid
        componentsContainer.setBounds(bounds.reduced(5, 0));
        
        // Adjust sizes to ensure components fit well
        const int itemHeight = 85;
        const int itemWidth = 80;  // Slightly smaller to ensure better fit
        const int hgap = 15;       // Increased gap
        const int vgap = 12;
        
        // Calculate how many items fit horizontally with extra padding
        int containerWidth = componentsContainer.getWidth();
        
        // Allow more space for each component by limiting columns if needed
        int columnsPerRow = juce::jmax(2, juce::jmin(4, (containerWidth - (2 * hgap)) / (itemWidth + hgap)));
        
        // Calculate actual component width based on available space
        int freeSpace = containerWidth - (columnsPerRow + 1) * hgap;
        int actualItemWidth = juce::jmax(75, freeSpace / columnsPerRow); // Ensure minimum width
        
        // Position components with better spacing
        int x = 0;
        int y = 0;
        
        for (auto* comp : components)
        {
            // Calculate position with consistent spacing and padding
            int xPos = hgap + x * (actualItemWidth + hgap);
            int yPos = y * (itemHeight + vgap);
            
            // Set the component bounds
            comp->setBounds(xPos, yPos, actualItemWidth, itemHeight);
            
            // Move to next position
            x++;
            if (x >= columnsPerRow)
            {
                x = 0;
                y++;
            }
        }
        
        // Calculate required height for the container with extra padding
        int requiredHeight = (components.size() == 0) ? 0 : 
                             ((components.size() + columnsPerRow - 1) / columnsPerRow) * (itemHeight + vgap) + vgap;
        componentsContainer.setSize(componentsContainer.getWidth(), requiredHeight);
    }
    
    int getRequiredHeight() const
    {
        return 40 + 70 + componentsContainer.getHeight() + 20; // Header + Description (increased) + Components + Margin
    }
    
private:
    juce::Label titleLabel;
    juce::Label description;
    juce::Component componentsContainer;
    juce::OwnedArray<DraggableComponent> components;
    juce::String categoryTitle;
    juce::Colour headerColour;
};

//==============================================================================

class ComponentPanel : public juce::Component
{
public:
    ComponentPanel();
    ~ComponentPanel() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::Viewport viewport;
    juce::Component contentContainer;
    
    // Sections for different component types
    std::unique_ptr<CategorySection> guiSection;
    std::unique_ptr<CategorySection> eqSection;
    std::unique_ptr<CategorySection> dynamicsSection;
    
    // Helper method to create and add components 
    void createComponents();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComponentPanel)
};
