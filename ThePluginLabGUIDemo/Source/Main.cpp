#include <JuceHeader.h>
#include "../MainComponent.h" // Change this to the correct path

//==============================================================================
class ThePluginLabGUIDemoApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    ThePluginLabGUIDemoApplication() {}

    const juce::String getApplicationName() override       { return "The Plugin Lab"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // Initialize logger to help diagnose crashes
        juce::Logger::writeToLog("Application starting...");
        juce::Logger::writeToLog("Current directory: " + juce::File::getCurrentWorkingDirectory().getFullPathName());
        
        try {
            // Create main window with proper error handling
            mainWindow.reset(new MainWindow(getApplicationName()));
            juce::Logger::writeToLog("Main window created successfully");
        }
        catch (const std::exception& e) {
            juce::Logger::writeToLog("Exception during initialization: " + juce::String(e.what()));
        }
    }

    void shutdown() override
    {
        // Delete the main window
        mainWindow = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance is launched, this method is invoked
    }

    //==============================================================================
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                  .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() function
START_JUCE_APPLICATION (ThePluginLabGUIDemoApplication)
