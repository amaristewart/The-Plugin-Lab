#pragma once
#include <JuceHeader.h>
#include <exception>
#include <stdexcept>
#include <string>

/**
 * Class for handling exceptions and providing error reporting
 */
class ExceptionHandler
{
public:
    // Install global exception handlers
    static void installExceptionHandlers()
    {
        // Set up C++ exception handler
        std::set_terminate([]() {
            try {
                // Rethrow current exception
                std::rethrow_exception(std::current_exception());
            }
            catch (const std::exception& e) {
                reportCrash("Unhandled Exception", e.what());
            }
            catch (...) {
                reportCrash("Unknown Error", "An unknown error occurred");
            }
            
            // Exit with error code
            std::abort();
        });
        
        // JUCE-specific exception handling - fix signature for CrashHandlerFunction
        juce::SystemStats::setApplicationCrashHandler(exceptionCallback);
    }
    
    // Crash handler callback with correct signature
    static void exceptionCallback(void* exception)
    {
        const char* exInfo = "Unknown crash";
        
        #if JUCE_WINDOWS
        if (exception != nullptr)
            exInfo = static_cast<const char*>(exception);
        #elif JUCE_MAC || JUCE_LINUX
        // This will be implementation-specific
        exInfo = "Application crashed";
        #endif
        
        reportCrash("Application Crash", exInfo);
    }
    
    // Show error message to user
    static void showErrorMessage(const juce::String& title, const juce::String& message)
    {
        // Log the error
        juce::Logger::writeToLog("ERROR: " + title + ": " + message);
        
        // Show alert box on the message thread
        juce::MessageManager::callAsync([title, message]() {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                title,
                message,
                "OK");
        });
    }
    
private:
    // Report a crash with logging and possible crash reporting service
    static void reportCrash(const std::string& type, const std::string& details)
    {
        // Log the crash
        juce::String crashLog = "CRASH: " + juce::String(type) + "\n" + 
                              juce::String(details) + "\n" +
                              juce::SystemStats::getStackBacktrace();
                              
        juce::Logger::writeToLog(crashLog);
        
        // Try to show an error dialog if possible
        if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            // Fix: Use showMessageBoxAsync instead of showMessageBox
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Application Error",
                "The application has encountered an error and needs to close.\n\n" +
                juce::String(type) + ": " + juce::String(details));
        }
        else
        {
            // If not on the message thread, we can't safely show a dialog
            // Just log the error and continue
            juce::Logger::writeToLog("Cannot show error dialog: not on message thread");
        }
        
        // You could add integration with crash reporting services here
    }
};
