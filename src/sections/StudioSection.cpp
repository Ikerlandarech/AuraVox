/*
  ==============================================================================

    StudioSection.cpp
    Created: 2 Mar 2024 5:44:47pm
    Author:  Iker

  ==============================================================================
*/

#include <JuceHeader.h>
#include "StudioSection.h"

StudioSection::StudioSection(TFGAuroVoxStudioAudioProcessor* p) : processor(*p)
{

}

StudioSection::~StudioSection()
{

}

void StudioSection::paint(juce::Graphics& g)
{
    //g.setColour (juce::Colours::red);
    //g.fillRect(violinModelSELSpace);

    //g.setColour (juce::Colours::orange);
    //g.fillRect(clarinetModelSELSpace);

    //g.setColour (juce::Colours::yellow);
    //g.fillRect(fluteModelSELSpace);

    //g.setColour (juce::Colours::green);
    //g.fillRect(tubaModelSELSpace);

    //g.setColour (juce::Colours::blue);
    //g.fillRect(tromboneModelSELSpace);

    //g.setColour (juce::Colours::purple);
    //g.fillRect(saxophoneModelSELSpace);

    //g.setColour (juce::Colours::pink);
    //g.fillRect(trumpetModelSELSpace);

    //g.setColour(juce::Colours::white);
}

void StudioSection::mouseDown (const juce::MouseEvent& event)
{
    auto clickPosition = event.getPosition().toFloat();

    if (violinModelSELSpace.contains (clickPosition))           {loadModel(1);}
    else if (clarinetModelSELSpace.contains (clickPosition))    {loadModel(2);}
    else if (fluteModelSELSpace.contains (clickPosition))       {loadModel(3);}
    else if (tubaModelSELSpace.contains (clickPosition))        {loadModel(4);}
    else if (tromboneModelSELSpace.contains (clickPosition))    {loadModel(5);}
    else if (saxophoneModelSELSpace.contains (clickPosition))   {loadModel(6);}
    else if (trumpetModelSELSpace.contains (clickPosition))     {loadModel(7);}
}

void StudioSection::resized()
{
    auto bounds = getBounds();
    auto width = getWidth();
    auto height = getHeight();
}

void StudioSection::buttonClicked (juce::Button* button) {}

void StudioSection::loadModel (int modelIndex)
{
    DBG ("Loading model: " + modelIndex);
    processor.loadModel (modelIndex);
}
