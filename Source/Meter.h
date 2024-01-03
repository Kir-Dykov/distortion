/*
  ==============================================================================

    Meter.h
    Created: 2 Jan 2024 11:28:53pm
    Author:  miq

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Meter  : public juce::Component, public juce::Timer
{
public:
    Meter(std::function<float()>&&);
    ~Meter() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    std::function<float()> valueSupplier;
    juce::ColourGradient gradient;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Meter)
};
