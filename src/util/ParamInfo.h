#pragma once

#include "JuceHeader.h"

struct ParamInfo
{
    juce::String paramID;
    juce::String paramName;
    // Slider-related variables.
    float rangeMin;
    float rangeMax;
    float defaultValue;
    float rangeInterval;
    bool isIntParam;
    int numDecimalPlaces;
    juce::String unit;
};

std::map<juce::String, std::vector<ParamInfo>> getSliderParamsInfo();

// Creates and adds AudioProcessor params to the value tree state.
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
