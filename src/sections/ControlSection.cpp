#include <JuceHeader.h>
#include "ControlSection.h"
#include "PluginProcessor.h"

ControlSection::ControlSection(TFGAuroVoxStudioAudioProcessor* p)
    : processor(*p), inputLoadSection (p), outputLoadSection (p)
{
    inputPlayButton.setLookAndFeel(&customLookAndFeel);
    inputPlayButton.setClickingTogglesState(true);
    outputPlayButton.setLookAndFeel(&customLookAndFeel);
    outputPlayButton.setClickingTogglesState(true);
    inputPlayButton.addListener(this);
    outputPlayButton.addListener(this);
    addAndMakeVisible (inputLoadSection);
    addAndMakeVisible (outputLoadSection);
    addAndMakeVisible(inputPlayButton);
    addAndMakeVisible(outputPlayButton);

    /*inputPlayButton.getToggleState();*/

}

ControlSection::~ControlSection()
{
    setLookAndFeel(nullptr); // Reset the LookAndFeel to default
}

void ControlSection::paint(juce::Graphics& g)
{

    g.setColour(juce::Colours::white);
    int controlSectionWidth = getWidth();
    int controlSectionHeight = getHeight();
    juce::Rectangle<float> controlSectionRectangle(controlSectionCentre.getX() + 3,
                                                   controlSectionCentre.getY() + 3,
                                                   controlSectionWidth - 6,
                                                   controlSectionHeight - 6);
    g.drawRoundedRectangle(controlSectionRectangle,
                                                   10, //Corner Size
                                                   3); //Line Thickness)
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(controlSectionRectangle, 10.f); //Corner Size

    juce::Rectangle<int> controlSectionRectangleInt = controlSectionRectangle.toNearestInt();

    g.reduceClipRegion(controlSectionRectangleInt);

}

void ControlSection::resized()
{
    auto controlSectionBounds = getBounds();
    auto controlSectionCentre = controlSectionBounds.getCentre();
    auto width = 1.031 * getWidth();
    auto height = getHeight();

    //INPUT PLAY BUTTON
    inputPlayButton.setBounds(inputButtonXPos,
                              inputButtonYPos,
                              inputButtonWidth,
                              inputButtonHeight);
    
    //OUTPUT PLAY BUTTON
    outputPlayButton.setBounds(outputButtonXPos,
                               outputButtonYPos,
                               outputButtonWidth,
                               outputButtonHeight);

    //INPUT LOAD SECTION:
    int inputloadSectionWidth = 0.37f * width - 7;
    int inputloadSectionHeight = 0.77f * height - 11;
    int inputloadSectionXOffset = width * 0.23f;
    int inputloadSectionYOffset = height * -0.08f;
    inputLoadSection.setSize (inputloadSectionWidth, inputloadSectionHeight);
    inputLoadSection.setCentrePosition (controlSectionCentre.getX() - inputloadSectionXOffset,
                                        controlSectionCentre.getY() + inputloadSectionYOffset);

    //OUTPUT LOAD SECTION:
    int outputloadSectionWidth = 0.37f * width - 7;
    int outputloadSectionHeight = 0.77f * height - 11;
    int outputloadSectionXOffset = (width * 0.235f);
    int outputloadSectionYOffset = height * -0.08f;
    outputLoadSection.setSize (outputloadSectionWidth, outputloadSectionHeight);
    outputLoadSection.setCentrePosition (controlSectionCentre.getX() + outputloadSectionXOffset,
                                         controlSectionCentre.getY() + outputloadSectionYOffset);
}

void ControlSection::buttonClicked(juce::Button* button)
{
    if (button == &inputPlayButton)
    {
        //Updating the processor on the button state:
        processor.setInputToggleButtonState(inputPlayButton.getToggleState()); //IsInputPlaying == true
        processor.playInputButtonClicked();
    }
    if (button == &outputPlayButton)
    {
        //Updating the processor on the button state:
        processor.setOutputToggleButtonState(outputPlayButton.getToggleState()); //IsOutputPlaying == true
        processor.playOutputButtonClicked();
    }
}

void ControlSection::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    //inputPlayButton.triggerClick();
    //outputPlayButton.triggerClick();
}


