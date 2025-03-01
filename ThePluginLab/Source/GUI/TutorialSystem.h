#pragma once
#include <JuceHeader.h>

class TutorialSystem : public juce::Component
{
public:
    struct TutorialStep
    {
        juce::String title;
        juce::String description;
        juce::Component* highlightComponent = nullptr;
        std::function<bool()> completionCheck;
    };
    
    TutorialSystem()
    {
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(false, true);
        
        nextButton.setButtonText("Next >");
        nextButton.onClick = [this]() { nextStep(); };
        addAndMakeVisible(nextButton);
        
        messageBox.setMultiLine(true);
        messageBox.setReadOnly(true);
        messageBox.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black.withAlpha(0.8f));
        messageBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
        addAndMakeVisible(messageBox);
    }
    
    void startTutorial(const juce::String& tutorialName)
    {
        currentTutorial = tutorialName;
        currentStep = 0;
        
        if (tutorialName == "EQ Tutorial")
            loadEQTutorial();
        else if (tutorialName == "Compression Tutorial")
            loadCompressionTutorial();
        else if (tutorialName == "Basics")
            loadBasicsTutorial();
            
        showCurrentStep();
        setVisible(true);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dim background
        g.fillAll(juce::Colours::black.withAlpha(0.5f));
        
        // Highlight current component
        if (auto* comp = getCurrentStep().highlightComponent)
        {
            auto bounds = comp->getBounds();
            auto screenBounds = comp->getScreenBounds();
            auto localBounds = getLocalArea(nullptr, screenBounds);
            
            // Cut out the highlighted component area
            g.setColour(juce::Colours::black.withAlpha(0.5f));
            g.excludeClipRegion(localBounds);
            g.fillAll();
            
            // Draw highlight border
            g.setColour(juce::Colours::yellow);
            g.drawRect(localBounds.expanded(2), 2.0f);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto bottomArea = bounds.removeFromBottom(50);
        
        nextButton.setBounds(bottomArea.removeFromRight(100).reduced(10));
        messageBox.setBounds(bounds.reduced(50, 200));
    }
    
private:
    void nextStep()
    {
        if (currentStep < steps.size() - 1)
        {
            currentStep++;
            showCurrentStep();
        }
        else
            setVisible(false);
    }
    
    void showCurrentStep()
    {
        auto& step = getCurrentStep();
        messageBox.setText(step.title + "\n\n" + step.description);
        repaint();
    }
    
    const TutorialStep& getCurrentStep() const
    {
        return steps[currentStep];
    }
    
    void loadEQTutorial()
    {
        steps = {
            { "Welcome to EQ", "Learn how to create and use an equalizer plugin.", nullptr },
            { "Adding Bands", "Drag EQ bands from the panel to create your equalizer.", nullptr },
            { "Connecting Bands", "Connect the bands together to create your signal chain.", nullptr },
            // Add more steps...
        };
    }
    
    void loadCompressionTutorial()
    {
        steps = {
            { "Compression Basics", "Learn how to use compression in your plugin.", nullptr },
            { "Threshold", "The threshold determines when compression starts.", nullptr },
            { "Ratio", "The ratio controls how much compression is applied.", nullptr },
            // Add more steps...
        };
    }
    
    void loadBasicsTutorial()
    {
        steps = {
            { "Welcome", "Learn the basics of creating plugins.", nullptr },
            { "Components", "Drag components from the left panel.", nullptr },
            { "Connections", "Connect components to create your signal flow.", nullptr },
            // Add more steps...
        };
    }
    
    juce::String currentTutorial;
    size_t currentStep = 0;
    std::vector<TutorialStep> steps;
    
    juce::TextButton nextButton;
    juce::TextEditor messageBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TutorialSystem)
}; 
