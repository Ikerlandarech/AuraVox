/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 3 Mar 2024 5:53:12pm
    Author:  Iker

  ==============================================================================
*/
#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
}
CustomLookAndFeel::~CustomLookAndFeel()
{
}
void CustomLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setColour(juce::Colours::white);

    juce::Rectangle<float> inputButtonRectangle(inputButtonBounds.getX(),
                                                inputButtonBounds.getY(),
                                                inputButtonRectWidth,
                                                inputButtonRectHeight);
    juce::Rectangle<float> inputButtonRectangleRed = inputButtonRectangle.reduced(3, 3);

    g.drawRoundedRectangle(inputButtonRectangleRed,
                                                10, //Corner Size
                                                3); //Line Thickness)
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(inputButtonRectangleRed, 10.f); //Corner Size
    juce::Path path;
    const auto bounds = button.getLocalBounds().toFloat();

    //Draw play symbol if the button is toggled off
    if (!button.getToggleState())
    {
        // Define triangle points
        juce::Point<float> p1(bounds.getWidth() * 0.35f, bounds.getHeight() * 0.3f);
        juce::Point<float> p2(bounds.getWidth() * 0.35f, bounds.getHeight() * 0.7f);
        juce::Point<float> p3(bounds.getWidth() * 0.65f, bounds.getHeight() * 0.5f);

        // Add triangle to path
        path.startNewSubPath(p1);
        path.lineTo(p2);
        path.lineTo(p3);
        path.closeSubPath();

        // Set the color and fill the path
        g.setColour(juce::Colours::white);
        g.fillPath(path);
    }
    // Draw pause symbol if the button is toggled on
    else
    {
        // Define rectangle dimensions
        float pauseRectWidth = bounds.getWidth() * 0.1f;
        float pauseRectHeight = bounds.getHeight() * 0.4f;
        float pauseRectGap = bounds.getWidth() * 0.1f; // Reduce the gap between rectangles

        // Calculate the total width of the symbol (two rectangles and the gap between them)
        float totalWidth = (2 * pauseRectWidth) + pauseRectGap;

        // Calculate the starting x-coordinate to center the symbol within the button
        float startX = (bounds.getWidth() - totalWidth) / 2.0f;

        // Add first rectangle to path
        path.addRectangle(startX, bounds.getHeight() * 0.3f,
            pauseRectWidth, pauseRectHeight);

        // Add second rectangle to path, adjusted to be separated from the first one
        path.addRectangle(startX + pauseRectWidth + pauseRectGap, bounds.getHeight() * 0.3f,
            pauseRectWidth, pauseRectHeight);

        // Set the color and stroke for the symbols
        g.setColour(juce::Colours::white);
        g.fillPath(path);
    }

    //Set the color and stroke for the symbols
    g.setColour(juce::Colours::white);
    g.strokePath(path, juce::PathStrokeType(2.0f));
}