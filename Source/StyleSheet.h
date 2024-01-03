/*
  ==============================================================================

    StyleSheet.h
    Created: 30 Dec 2023 10:45:57pm
    Author:  miq

  ==============================================================================
*/

#pragma once

#include  <JuceHeader.h>


class CustomLNF : public juce::LookAndFeel_V4 {
public:
    CustomLNF();
};

class CustomRotaryLNF : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&);

    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider);

    juce::Label* createSliderTextBox(juce::Slider& slider);
};

class CustomDial : public juce::Slider
{
    CustomRotaryLNF customRotaryLNF;
public:
    CustomDial(juce::String name) {
        setName(name);
        setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkblue);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::black);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        
        setLookAndFeel(&customRotaryLNF);
        //toFront(false);
        //setAlwaysOnTop(true);
    }


    /*void mouseDoubleClick(const juce::MouseEvent& event) {

    }*/
};

