#pragma once
#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4,
                          public juce::Component
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    int inputButtonRectWidth = 50;
    int inputButtonRectHeight = 30;

    juce::Rectangle<int> inputButtonBounds = getBounds();
    juce::Point<int> inputButtonCentre = inputButtonBounds.getCentre();



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};

