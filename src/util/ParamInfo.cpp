#include "ui/ParamInfo.h"

std::map<juce::String, std::vector<ParamInfo>> getSliderParamsInfo()
{
    static const std::map<juce::String, std::vector<ParamInfo>> ret = {
        // Model control params.
        { "Controls",
          { { "PitchShift", "Pitch Shift", -24, 24, 0, 1, true, 1, "st" },
            { "HarmonicGain", "Harmonics", 0.0f, 1.0f, 1.0f, 0.01f, false, 2, "" },
            { "NoiseGain", "Noise", 0.0f, 1.0f, 1.0f, 0.01f, false, 2, "" },
            { "OutputGain", "Output Gain", -60.0f, 0.0f, 0.0f, 0.01f, false, 2, "dB" } } },
#if JucePlugin_IsSynth
        // Synth mode params.
        { "Envelope",
          { { "Attack", "Attack", 0.1f, 3.0f, 0.1f, 0.01f, false, 2, "s" },
            { "Decay", "Decay", 0.1f, 3.0f, 0.0f, 0.01f, false, 2, "s" },
            { "Sustain", "Sustain", 0.0f, 1.0f, 1.0f, 0.01f, false, 2, "" },
            { "Release", "Release", 0.1f, 5.0f, 1.2f, 0.01f, false, 2, "s" } } },
#endif
        { "Effects",
          { { "ReverbSize", "Reverb Size", 0.0f, 1.0f, 0.4f, 0.01f, false, 2, "" },
            { "ReverbDamping", "Reverb Damp", 0.0f, 1.0f, 0.1f, 0.01f, false, 2, "" },
            { "ReverbWet", "Reverb Wet", 0.0f, 1.0f, 0.0f, 0.01f, false, 2, "" } } }
    };
    return ret;
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> ("InputGain", "Input Gain", -0.5f, 0.5f, 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("InputPitch", "Input Pitch", -0.5f, 0.5f, 0.0f));

    // Scene-related params.
    const auto paramInfos = getSliderParamsInfo();
    for (const auto& [groupName, infos] : paramInfos)
    {
        for (const auto& info : infos)
        {
            if (info.isIntParam)
            {
                layout.add (std::make_unique<juce::AudioParameterInt> (info.paramID,
                                                                       info.paramName,
                                                                       static_cast<int> (info.rangeMin),
                                                                       static_cast<int> (info.rangeMax),
                                                                       static_cast<int> (info.defaultValue)));
            }
            else
            {
                layout.add (std::make_unique<juce::AudioParameterFloat> (
                    info.paramID, info.paramName, info.rangeMin, info.rangeMax, info.defaultValue));
            }
        }
    }

    return layout;
}
