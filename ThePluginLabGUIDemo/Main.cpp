#include <JuceHeader.h>
#include "MainComponent.h"

// Global function to ensure logo is created at application startup
void ensureLogoExists()
{
    // Create logo at the exact specified path
    juce::File logoFile("/Users/amaristewart/Documents/GitHub/The-Plugin-Lab/GUI/ThePluginLabGUIDemo/Resources/logo.png");
    
    // Make sure the parent directory exists
    juce::File resourcesDir = logoFile.getParentDirectory();
    if (!resourcesDir.exists())
    {
        bool success = resourcesDir.createDirectory();
        juce::Logger::writeToLog("Creating directory at: " + resourcesDir.getFullPathName() + 
                                " - " + (success ? "SUCCESS" : "FAILED"));
    }
    
    if (!logoFile.existsAsFile())
    {
        juce::Logger::writeToLog("Creating logo at exact path: " + logoFile.getFullPathName());
        
        // Create a nicer looking placeholder logo - with larger dimensions
        juce::Image placeholderLogo(juce::Image::RGB, 240, 46, true);
        juce::Graphics g(placeholderLogo);
        
        // Draw a gradient background
        juce::ColourGradient gradient(juce::Colour(100, 150, 255), 0, 0, 
                                      juce::Colour(50, 100, 200), 240, 46, false);
        g.setGradientFill(gradient);
        g.fillAll();
        
        // Draw text aligned to the left for corner placement
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(26.0f, juce::Font::bold));
        g.drawText("The Plugin Lab", placeholderLogo.getBounds().reduced(10, 0), 
                   juce::Justification::centredLeft, true);
        
        // Save the logo
        try {
            juce::FileOutputStream stream(logoFile);
            if (stream.openedOk())
            {
                juce::PNGImageFormat format;
                bool success = format.writeImageToStream(placeholderLogo, stream);
                juce::Logger::writeToLog("Logo created at exact path: " + 
                                       (success ? "SUCCESS" : "FAILED"));
            }
            else
            {
                juce::Logger::writeToLog("Failed to open output stream for logo");
            }
        }
        catch (const std::exception& e) {
            juce::Logger::writeToLog("Error creating logo: " + juce::String(e.what()));
        }
    }
    else {
        juce::Logger::writeToLog("Logo already exists at: " + logoFile.getFullPathName());
    }
    
    // Also create backup logos in other locations for redundancy
    juce::Array<juce::File> possibleDirs;
    possibleDirs.add(juce::File::getCurrentWorkingDirectory().getChildFile("Resources"));
    possibleDirs.add(juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("Resources"));
    possibleDirs.add(juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory().getChildFile("Resources"));
    
    juce::Logger::writeToLog("Creating logo in the following directories:");
    for (auto& dir : possibleDirs)
    {
        if (!dir.exists())
            dir.createDirectory();
            
        juce::Logger::writeToLog("  " + dir.getFullPathName() + (dir.isDirectory() ? " (exists)" : " (failed)"));
        
        if (dir.isDirectory())
        {
            juce::File logoFile = dir.getChildFile("logo.png");
            
            if (!logoFile.existsAsFile())
            {
                // Create a placeholder logo - with larger dimensions
                juce::Image placeholderLogo(juce::Image::RGB, 240, 46, true);
                juce::Graphics g(placeholderLogo);
                
                juce::ColourGradient gradient(juce::Colour(100, 150, 255), 0, 0, 
                                          juce::Colour(50, 100, 200), 240, 46, false);
                g.setGradientFill(gradient);
                g.fillAll();
                
                g.setColour(juce::Colours::white);
                g.setFont(26.0f);
                g.drawText("The Plugin Lab", placeholderLogo.getBounds().reduced(10, 0), 
                           juce::Justification::centredLeft, true);
                
                // Save the logo
                try {
                    juce::FileOutputStream stream(logoFile);
                    if (stream.openedOk())
                    {
                        juce::PNGImageFormat format;
                        format.writeImageToStream(placeholderLogo, stream);
                        juce::Logger::writeToLog("Created logo at: " + logoFile.getFullPathName());
                    }
                }
                catch (const std::exception& e) {
                    juce::Logger::writeToLog("Error creating logo at " + logoFile.getFullPathName() + 
                                            ": " + juce::String(e.what()));
                }
            }
            else {
                juce::Logger::writeToLog("Logo already exists at: " + logoFile.getFullPathName());
            }
        }
    }
}

//==============================================================================
class ThePluginLabApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    ThePluginLabApplication() {}

    const juce::String getApplicationName() override       { return "The Plugin Lab"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // Initialize logger to help diagnose crashes
        juce::Logger::writeToLog("Application starting...");
        juce::Logger::writeToLog("Current directory: " + juce::File::getCurrentWorkingDirectory().getFullPathName());
        
        // Create logo at application startup - BEFORE creating UI components
        ensureLogoExists();
        
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
        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
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

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

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
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (ThePluginLabApplication)