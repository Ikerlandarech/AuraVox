#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUIAttributes.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

class OutputLoadSection : public juce::Component,
                    public juce::Button::Listener,
                    public juce::FileDragAndDropTarget,
                    public juce::ChangeBroadcaster,
                    public juce::DragAndDropContainer
{
public:
    OutputLoadSection(TFGAuroVoxStudioAudioProcessor* p);
    ~OutputLoadSection() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* button) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void mouseDown(const juce::MouseEvent& event) override;
    juce::String getSelectedModelName(int selectedModel);
    void mouseDrag (const juce::MouseEvent& event) override;

private:
    int playHeadPosition;
    int endWidth;

    juce::Rectangle<int> loadSectionBounds = getBounds();
    
    juce::Point<int> loadSectionCentre = loadSectionBounds.getCentre();

    int loadAudioButtonWidth = 400;
    int loadAudioButtonHeight = 150;

    juce::TextButton loadAudioButton;
    std::vector<float> audioPoints;
    bool isWaveform{ false };

    juce::String fileName{ "" };
    juce::String outputFileName { "" };
    int selectedModel = 0;
    juce::String selectedModelName { "" };
    juce::File outputFile;

    void waveformDisplay(juce::Graphics&);

    TFGAuroVoxStudioAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputLoadSection)
};