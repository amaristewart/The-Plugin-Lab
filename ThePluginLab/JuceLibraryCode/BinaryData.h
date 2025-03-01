/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   hover_wav;
    const int            hover_wavSize = 45052;

    extern const char*   click_wav;
    const int            click_wavSize = 30714;

    extern const char*   PluginLabLogo_png;
    const int            PluginLabLogo_pngSize = 162270;

    extern const char*   BasicCompressor_xml;
    const int            BasicCompressor_xmlSize = 833;

    extern const char*   BasicEQ_xml;
    const int            BasicEQ_xmlSize = 707;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 5;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
