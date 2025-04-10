#!/bin/bash

# A more advanced script to detect duplicate symbol definitions
# Run after a failed build to identify the specific duplicate symbols

if [ -z "$1" ]; then
    echo "Usage: $0 <path_to_build_log>"
    exit 1
fi

BUILD_LOG=$1

echo "Analyzing build log for duplicate symbols..."

# Extract duplicate symbol names from build log
grep -i "duplicate symbol" "$BUILD_LOG" | sed 's/.*duplicate symbol //g' | sed 's/ in.*//g' | sort | uniq > duplicate_symbols.txt

if [ ! -s duplicate_symbols.txt ]; then
    echo "No duplicate symbols found in the build log."
    rm duplicate_symbols.txt
    exit 0
fi

echo "Found these duplicate symbols:"
cat duplicate_symbols.txt

echo -e "\nSearching for these symbols in source files..."

while read symbol; do
    echo -e "\nLocations for symbol: $symbol"
    find . -name "*.cpp" -o -name "*.mm" -o -name "*.m" -o -name "*.c" | xargs grep -l "$symbol" | grep -v "moc_" | grep -v ".build"
done < duplicate_symbols.txt

rm duplicate_symbols.txt
echo -e "\nAnalysis complete!"
