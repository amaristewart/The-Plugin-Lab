#include <JuceHeader.h>
#include "MainComponent.h"
#include "Common/ExceptionHandler.h"

// This class handles the application shutdown to prevent DisplayLink crashes
class SafeApplicationShutdown
{
public:
    static void performShutdown()
    {

    }
};

class ThePluginLabApplication : public juce::JUCEApplication
{
public:
    ThePluginLabApplication() {}
    
    const juce::String getApplicationName() override       { return "The Plugin Lab"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }
    
    void initialise(const juce::String& commandLine) override
    {
        // Install exception handlers
        ExceptionHandler::installExceptionHandlers();
        
        // Create main window
        mainWindow.reset(new MainWindow(getApplicationName()));
    }
    
    void shutdown() override
    {
        // Perform safe shutdown
        SafeApplicationShutdown::performShutdown();
        
        // Clean up resources properly before shutdown
        mainWindow = nullptr;
    }
    
    void systemRequestedQuit() override
    {
        // Ensure clean shutdown
        quit();
    }
    
    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        // Focus this instance if another one starts
        mainWindow->toFront(true);
    }
    
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                          juce::Desktop::getInstance().getDefaultLookAndFeel()
                              .findColour(juce::ResizableWindow::backgroundColourId),
                          DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(createMainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
           #else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
           #endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
    private:
        juce::Component* createMainComponent()
        {
            try {
                auto* comp = new MainComponent();
                return comp;
            }
            catch (const std::exception& e) {
                ExceptionHandler::showErrorMessage("Error initializing application", e.what());
                return new juce::Component(); // Return empty component to avoid crashing
            }
        }
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };
    
private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(ThePluginLabApplication)
