#pragma once
#include <JuceHeader.h>

/**
 * SIMPLIFIED EQ Processor for GUI prototype only
 * This class only stores parameter data for visualization
 * No actual audio processing is implemented
 */
class EQProcessor 
{
public:
    enum FilterType
    {
        LowPass = 0,
        HighPass,
        LowShelf,
        HighShelf,
        BandPass,
        Notch,
        Peak,
        NumFilterTypes
    };
    
    struct FilterBand
    {
        FilterType type = FilterType::Peak;
        float frequency = 1000.0f;
        float gainDb = 0.0f;
        float q = 0.707f;
        bool active = true;
    };
    
    EQProcessor()
    {
        // Initialize with default values
        bands[0] = { FilterType::LowShelf, 80.0f, 0.0f, 0.707f, true };
        bands[1] = { FilterType::Peak, 250.0f, 0.0f, 0.707f, true };
        bands[2] = { FilterType::Peak, 1000.0f, 0.0f, 0.707f, true };
        bands[3] = { FilterType::Peak, 4000.0f, 0.0f, 0.707f, true };
        bands[4] = { FilterType::HighShelf, 12000.0f, 0.0f, 0.707f, true };
        
        // Disable remaining bands
        for (int i = 5; i < 8; ++i) {
            bands[i].active = false;
        }
    }
    
    // Parameter access methods
    void setFilterType(int band, FilterType type) {
        if (band >= 0 && band < 8) bands[band].type = type;
    }
    
    FilterType getFilterType(int band) const {
        return (band >= 0 && band < 8) ? bands[band].type : FilterType::Peak;
    }
    
    void setFrequency(int band, float frequency) {
        if (band >= 0 && band < 8) bands[band].frequency = frequency;
    }
    
    float getFrequency(int band) const {
        return (band >= 0 && band < 8) ? bands[band].frequency : 1000.0f;
    }
    
    void setGain(int band, float gainDb) {
        if (band >= 0 && band < 8) bands[band].gainDb = gainDb;
    }
    
    float getGain(int band) const {
        return (band >= 0 && band < 8) ? bands[band].gainDb : 0.0f;
    }
    
    void setQ(int band, float q) {
        if (band >= 0 && band < 8) bands[band].q = q;
    }
    
    float getQ(int band) const {
        return (band >= 0 && band < 8) ? bands[band].q : 0.707f;
    }
    
    void setBandActive(int band, bool active) {
        if (band >= 0 && band < 8) bands[band].active = active;
    }
    
    bool isBandActive(int band) const {
        return (band >= 0 && band < 8) ? bands[band].active : false;
    }
    
    int getNumBands() const { return 8; }
    
    // Educational descriptions
    juce::String getFilterTypeDescription(FilterType type) const {
        switch (type) {
            case FilterType::LowPass:
                return "Low Pass: Allows frequencies below cutoff to pass through while attenuating frequencies above the cutoff.";
            case FilterType::HighPass:
                return "High Pass: Allows frequencies above cutoff to pass through while attenuating frequencies below the cutoff.";
            case FilterType::LowShelf:
                return "Low Shelf: Boosts or cuts frequencies below the cutoff frequency, leaving higher frequencies unaffected.";
            case FilterType::HighShelf:
                return "High Shelf: Boosts or cuts frequencies above the cutoff frequency, leaving lower frequencies unaffected.";
            case FilterType::BandPass:
                return "Band Pass: Allows a band of frequencies to pass through while attenuating frequencies outside the band.";
            case FilterType::Notch:
                return "Notch: Attenuates a narrow band of frequencies while allowing frequencies outside the notch to pass through.";
            case FilterType::Peak:
                return "Peak: Boosts or cuts a band of frequencies around the center frequency.";
            default:
                return "Unknown filter type.";
        }
    }
    
    juce::String getFrequencyDescription() const {
        return "Frequency (Hz): The center or cutoff frequency that the filter acts upon.";
    }
    
    juce::String getQDescription() const {
        return "Q: Controls the width of the frequency band. Higher Q values create narrower bands.";
    }
    
    juce::String getGainDescription() const {
        return "Gain (dB): Amount of boost or cut applied to the affected frequencies.";
    }
    
private:
    FilterBand bands[8];
};
