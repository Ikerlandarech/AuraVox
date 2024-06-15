#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
TFGAuroVoxStudioAudioProcessorEditor::TFGAuroVoxStudioAudioProcessorEditor (TFGAuroVoxStudioAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      studioSection (&p),
      controlSection (&p)
{
    //addAndMakeVisible (contentComponent);

    addAndMakeVisible(studioSection);
    //inputLoadSection.addChangeListener (&controlSection);
    //outputLoadSection.addChangeListener (&controlSection);
    addAndMakeVisible (controlSection);


    startTimerHz (50);

    setSize (1100, 700);
    setResizable (false, false);
}

TFGAuroVoxStudioAudioProcessorEditor::~TFGAuroVoxStudioAudioProcessorEditor()
{
    audioProcessor.removeAllChangeListeners();
    stopTimer();
}

//==============================================================================
void TFGAuroVoxStudioAudioProcessorEditor::paint (juce::Graphics& g)
{
    {
        g.drawImage (background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
        g.setColour (juce::Colours::white);
        juce::Font font;
        font.setHeight (GUIAttributes::ControlFontSizes::h1);
        font.setTypefaceName ("Futura");
        g.setFont (font);
        juce::Rectangle<int> textBounds (getLocalBounds().getX() + 40,
                                         getLocalBounds().getY() + 5,
                                         getLocalBounds().getWidth() * 0.5f,
                                         getLocalBounds().getHeight() * 0.15f);
        //g.drawText ("AuraVox Studio", textBounds, juce::Justification::centred, true);
    }
}

void TFGAuroVoxStudioAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    //ALL:
    auto editorBounds = getBounds();
    auto editorCentre = editorBounds.getCentre();
    int editorWidth = getWidth();
    int editorHeight = getHeight();

    //STUDIO SECTION:
    int studioSectionWidth = 0.97f * editorWidth;
    int studioSectionHeight = 0.65f * editorHeight;
    int studioSectionYOffset = editorHeight * 0.15f;
    studioSection.setSize (studioSectionWidth, studioSectionHeight);
    studioSection.setCentrePosition (editorCentre.getX(), editorCentre.getY() - studioSectionYOffset);

    //CONTROL SECTION:
    int controlSectionWidth = 0.97f * editorWidth;
    int controlSectionHeight = 0.3f * editorHeight;
    int controlSectionYOffset = editorHeight * 0.34f;
    controlSection.setSize (controlSectionWidth, controlSectionHeight);
    controlSection.setCentrePosition (editorCentre.getX(), editorCentre.getY() + controlSectionYOffset);
}

void TFGAuroVoxStudioAudioProcessorEditor::timerCallback() { repaint(); }