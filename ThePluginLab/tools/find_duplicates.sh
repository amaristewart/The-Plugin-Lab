#!/bin/bash

# Simple script to find potential duplicate symbol definitions
# Execute from the project root directory

echo "Looking for potential duplicate function definitions..."
find . -name "*.cpp" -o -name "*.mm" -o -name "*.c" | xargs grep -l "^[a-zA-Z_][a-zA-Z0-9_]* [a-zA-Z_][a-zA-Z0-9_]*::[a-zA-Z_][a-zA-Z0-9_]*(" | sort

echo "Looking for potential duplicate global variables..."
find . -name "*.cpp" -o -name "*.mm" -o -name "*.c" | xargs grep -l "^[a-zA-Z_][a-zA-Z0-9_]* [a-zA-Z_][a-zA-Z0-9_]*\s*=" | sort

echo "Checking for class/struct definitions in multiple files..."
find . -name "*.h" -o -name "*.hpp" | xargs grep -l "^class [a-zA-Z_][a-zA-Z0-9_]*\s*{" | sort
find . -name "*.h" -o -name "*.hpp" | xargs grep -l "^struct [a-zA-Z_][a-zA-Z0-9_]*\s*{" | sort
