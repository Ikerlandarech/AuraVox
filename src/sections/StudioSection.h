#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUIAttributes.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

class StudioSection :   public juce::Component, 
                        public juce::Button::Listener
{
public:
    StudioSection(TFGAuroVoxStudioAudioProcessor* p);
    ~StudioSection() override;


    void paint(juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent& event) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;

private:
    void loadModel (int modelIndex);

    TFGAuroVoxStudioAudioProcessor& processor;

    juce::Rectangle<float> violinModelSELSpace { 25, 108, 199, 271 };
    juce::Rectangle<float> clarinetModelSELSpace { 693, 7, 179, 262 };
    juce::Rectangle<float> fluteModelSELSpace { 232, 13, 215, 204 };
    juce::Rectangle<float> tubaModelSELSpace { 336, 228, 165, 192 };
    juce::Rectangle<float> tromboneModelSELSpace { 467, 49, 195, 175 };
    juce::Rectangle<float> saxophoneModelSELSpace { 878, 153, 144, 277 };
    juce::Rectangle<float> trumpetModelSELSpace { 587, 286, 248, 127 };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StudioSection)
};