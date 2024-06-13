/*
  ==============================================================================

    ControlSection.h
    Created: 2 Mar 2024 5:44:16pm
    Author:  Iker

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUIAttributes.h"
#include "CustomLookAndFeel.h"
#include "sections/LoadSection.h"
#include "sections/OutputLoadSection.h"


using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

class ControlSection :  public juce::Component,
                        public juce::Button::Listener,
                        public juce::ChangeListener
{
public:
    ControlSection(TFGAuroVoxStudioAudioProcessor* p);
    ~ControlSection() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

private:

    int inputButtonXPos = 260;
    int inputButtonYPos = 170;
    int inputButtonWidth = 50;
    int inputButtonHeight = 30;

    int outputButtonXPos = 780;
    int outputButtonYPos = 170;
    int outputButtonWidth = 50;
    int outputButtonHeight = 30;

    juce::Rectangle<int> controlSectionBounds = getBounds();
    juce::Point<int> controlSectionCentre = controlSectionBounds.getCentre();

    CustomLookAndFeel customLookAndFeel;

    LoadSection inputLoadSection;
    OutputLoadSection outputLoadSection;
    juce::ToggleButton inputPlayButton;
    juce::ToggleButton outputPlayButton;

    TFGAuroVoxStudioAudioProcessor& processor;

    //EditorLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlSection)
};