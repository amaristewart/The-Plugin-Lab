# How to Fix Duplicate Symbols

## Common Causes and Solutions

### 1. Header Guards Missing

Make sure all header files have proper include guards:

```cpp
#ifndef HEADER_NAME_H
#define HEADER_NAME_H

// content

#endif // HEADER_NAME_H
```

### 2. Implementation in Header Files

Functions defined in header files should be marked `inline` or moved to .cpp files:

```cpp
// In header: declare only
void myFunction();

// In cpp file: define
void myFunction() {
    // implementation
}
```

### 3. Multiple Includes of Implementation Files

Check if you're including .cpp files instead of just headers:

```cpp
// BAD - don't include .cpp files
#include "implementation.cpp"

// GOOD - include only headers
#include "header.h"
```

### 4. Global Variables in Headers

Move global variable definitions to a single .cpp file:

```cpp
// In header
extern int globalVar;

// In exactly ONE cpp file
int globalVar = 0;
```

### 5. Template Implementations

Templates should be fully defined in headers but with proper include guards.

### 6. Class Static Members

Static class members should be defined in ONE cpp file:

```cpp
// In header
class MyClass {
    static int staticVar;
};

// In exactly ONE cpp file
int MyClass::staticVar = 0;
```
