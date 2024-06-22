#pragma once

#include "sections/ControlSection.h"
#include "PluginProcessor.h"
#include "sections/StudioSection.h"
#include <JuceHeader.h>

//==============================================================================
/**
*/
class TFGAuroVoxStudioAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    TFGAuroVoxStudioAudioProcessorEditor (TFGAuroVoxStudioAudioProcessor&);
    ~TFGAuroVoxStudioAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    StudioSection studioSection;
    ControlSection controlSection;

    TFGAuroVoxStudioAudioProcessor& audioProcessor;

    juce::Image background = juce::ImageCache::getFromMemory (BinaryData::TFG_AUROVOX_BG_jpg,
                                                              BinaryData::TFG_AUROVOX_BG_jpgSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TFGAuroVoxStudioAudioProcessorEditor)
};