# Project Header Map

To avoid duplicate symbols, ensure you're including the correct headers:

## Main Components
- For main app UI: `#include "Source/Main/MainComponent.h"`
- For GUI elements: `#include "Source/GUI/MainComponent.h"`

## Connection Components
- For connection logic: `#include "Source/Connections/ConnectionComponent.h"`
- For UI components: `#include "Source/Components/ConnectionComponent.h"`

## Audio Connection Points
- For audio connection logic: `#include "Source/Connections/AudioConnectionPoint.h"`
- For connection UI elements: `#include "Source/Components/AudioConnectionPoint.h"`

Make sure all related header files have proper include guards and that class declarations
use the corresponding namespaces.
