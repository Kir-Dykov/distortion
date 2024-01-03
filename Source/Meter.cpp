/*
  ==============================================================================

    Meter.cpp
    Created: 2 Jan 2024 11:28:53pm
    Author:  miq

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Meter.h"

//==============================================================================
Meter::Meter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
{
    startTimerHz(24);
    
}

Meter::~Meter()
{
}

void Meter::paint (juce::Graphics& g)
{
    const float level = valueSupplier();
    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds, 2.f);
    
    g.setGradientFill(gradient);

    auto scaledY = juce::jmap(level, 0.f, 1.f, 0.f, 0.8f*float(getHeight()));
    g.fillRoundedRectangle(bounds.removeFromBottom(scaledY), 2.f);

}

void Meter::resized()
{
    auto bounds = getLocalBounds().toFloat();
    gradient = juce::ColourGradient(
        juce::Colour::fromRGB(25, 0, 50), bounds.getBottomLeft(),
        juce::Colour::fromRGB(255, 0, 0), bounds.getTopLeft(), false
    );
    gradient.addColour(0.25, juce::Colour::fromRGB(100, 0, 50));
    gradient.addColour(0.50, juce::Colour::fromRGB(255, 100, 100));
    gradient.addColour(0.6, juce::Colour::fromRGB(255, 255, 0));
    gradient.addColour(0.8, juce::Colour::fromRGB(255, 255, 255));
    //gradient.addColour(0.81, juce::Colour::fromRGB(255, 0, 0));
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void Meter::timerCallback()
{
    repaint();
}