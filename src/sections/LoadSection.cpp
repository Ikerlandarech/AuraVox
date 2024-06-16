#include "LoadSection.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"

LoadSection::LoadSection(TFGAuroVoxStudioAudioProcessor* p)
    : processor(*p)
{
    loadAudioButton.onClick = [&]() {processor.loadInputFile(); };
    if (isWaveform == false)
    {
        loadAudioButton.setButtonText("Drag & Drop \n or \n Choose your Audio File");
    }
    addAndMakeVisible(loadAudioButton);
}

LoadSection::~LoadSection()
{

}

void LoadSection::paint(juce::Graphics& g)
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

void LoadSection::resized()
{

    loadAudioButton.setBounds(loadSectionCentre.getX(),
                              loadSectionCentre.getY(),
                              loadAudioButtonWidth,
                              loadAudioButtonHeight);
}

void LoadSection::buttonClicked (juce::Button* button) {}

void LoadSection::waveformDisplay(juce::Graphics& g)
{
    auto waveform = processor.getInputWaveformBuffer();


    if (waveform.getNumSamples() > 0)
    {
        loadAudioButton.setVisible(false);
        g.setColour(juce::Colours::white);
        juce::Path p;
        audioPoints.clear();
        
        auto ratio = waveform.getNumSamples() / loadAudioButtonWidth;
        auto buffer = waveform.getReadPointer(0);

        //Scaling AudioFile to window in x axis:
        for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio)
        {
            audioPoints.push_back(buffer[sample]);
        }

        p.startNewSubPath(0, getHeight() / 2);

        //Scaling AudioFile to window in y axis:
        for (int sample = 0; sample < audioPoints.size(); sample++)
        {
            auto point = juce::jmap<float>(audioPoints[sample], -1.0f, 1.0f, loadAudioButtonHeight, 0);
            p.lineTo(sample, point);
        }

        g.strokePath(p, juce::PathStrokeType(1));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
        auto textBounds = getLocalBounds().reduced(10, 10);
        g.drawFittedText (fileName, textBounds, juce::Justification::topRight, 1);

        if (processor.isInputPlayed())
        {
            playHeadPosition = juce::jmap<int> (
                processor.getSampleCount(), 0, processor.getInputWaveformBuffer().getNumSamples(), 0, getWidth());

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

bool LoadSection::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
        {
            return true;
        }
    }
    return false;
}

void LoadSection::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            processor.loadInputFile(file);
        }
        processor.setFileName (file);
        fileName = processor.getFileName();
    }
    repaint();
}

void LoadSection::mouseDown(const juce::MouseEvent& event)
{
    sendChangeMessage();
}