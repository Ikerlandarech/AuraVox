/*
  ==============================================================================

    LoadSection.cpp
    Created: 2 Mar 2024 11:44:02pm
    Author:  Iker

  ==============================================================================
*/

#include "OutputLoadSection.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"

OutputLoadSection::OutputLoadSection(TFGAuroVoxStudioAudioProcessor* p)
    : processor(*p)
{
}

OutputLoadSection::~OutputLoadSection()
{

}

void OutputLoadSection::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);

    auto rectangleBounds = getLocalBounds().reduced(3, 3);

    juce::Rectangle<float> loadRectangle(rectangleBounds.getX(),
                                         rectangleBounds.getY(),
                                         rectangleBounds.getWidth(),
                                         rectangleBounds.getHeight());
    g.drawRoundedRectangle(              loadRectangle,
                                         10, //Corner Size
                                         3); //Line Thickness)
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.fillRoundedRectangle(loadRectangle, 10.f); //Corner Size

    juce::Rectangle<int> loadRectangleInt = loadRectangle.toNearestInt();

    g.reduceClipRegion(loadRectangleInt);
    waveformDisplay(g);
}

void OutputLoadSection::resized()
{

    loadAudioButton.setBounds(loadSectionCentre.getX(),
                              loadSectionCentre.getY(),
                              loadAudioButtonWidth,
                              loadAudioButtonHeight);
}

void OutputLoadSection::buttonClicked (juce::Button* button) {}

void OutputLoadSection::waveformDisplay(juce::Graphics& g)
{
    auto waveform = processor.getOutputWaveformBuffer();

    if (waveform.getNumSamples() > 0)
    {
        loadAudioButton.setVisible (false);
        g.setColour (juce::Colours::white);
        juce::Path p;
        audioPoints.clear();

        auto ratio = waveform.getNumSamples() / loadAudioButtonWidth;
        auto buffer = waveform.getReadPointer (0);

        //Scaling AudioFile to window in x axis:
        for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio)
        {
            audioPoints.push_back (buffer[sample]);
        }

        p.startNewSubPath (0, getHeight() / 2);

        //Scaling AudioFile to window in y axis:
        for (int sample = 0; sample < audioPoints.size(); sample++)
        {
            auto point = juce::jmap<float> (audioPoints[sample], -1.0f, 1.0f, loadAudioButtonHeight, 0);
            p.lineTo (sample, point);
        }

        g.strokePath (p, juce::PathStrokeType (1));

        g.setColour (juce::Colours::white);
        g.setFont (15.0f);
        auto textBounds = getLocalBounds().reduced (10, 10);
        selectedModel = processor.getCurrentModel();
        selectedModelName = getSelectedModelName (selectedModel);
        if (processor.isFileLoaded())
        {
            fileName = processor.getFileName();
            outputFileName = fileName + "_" + selectedModelName;
            g.drawFittedText (outputFileName, textBounds, juce::Justification::topRight, 1);
        }

        if (processor.isOutputPlayed()) {
            playHeadPosition = juce::jmap<int> (
                processor.getSampleCount(), 0, processor.getOutputWaveformBuffer().getNumSamples(), 0, getWidth());

            g.setColour (juce::Colours::white);
            g.drawLine (playHeadPosition, 0, playHeadPosition, getHeight(), 2.0f);
            g.setColour (juce::Colours::black.withAlpha (0.2f));
            g.fillRect (0, 0, playHeadPosition, getHeight());
            g.setColour (juce::Colours::white);
            if (playHeadPosition >= getWidth() - 4)
            {
                sendChangeMessage();
            }
        }
    }
}

bool OutputLoadSection::isInterestedInFileDrag(const juce::StringArray& files)
{
    return false;
}

void OutputLoadSection::filesDropped(const juce::StringArray& files, int x, int y)
{
}

void OutputLoadSection::mouseDown(const juce::MouseEvent& event)
{ sendChangeMessage(); }

juce::String OutputLoadSection::getSelectedModelName (int selectedModel) 
{
    switch (selectedModel)
    {
        case 1:     return "Violin";
        case 2:     return "Clarinet";
        case 3:     return "Flute";
        case 4:     return "Tuba";
        case 5:     return "Trombone";
        case 6:     return "Saxophone";
        case 7:     return "Trumpet";
        default:    return "_";
    }
}

void OutputLoadSection::mouseDrag (const juce::MouseEvent& event)
{
    juce::StringArray files;
    outputFile = processor.getOutputFile();
    files.add (outputFile.getFullPathName());
    performExternalDragDropOfFiles (files, false);
}