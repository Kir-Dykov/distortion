/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Param.h"
#include "StyleSheet.h"
#include "Meter.h"

#include <random>


inline float gainToMeterHight(float x) {
    float xx = sqrtf(x);
    return 2 * xx / (1 + xx);
}

//==============================================================================
/**
*/
class DistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DistortionAudioProcessorEditor (DistortionAudioProcessor&);
    ~DistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DistortionAudioProcessor& audioProcessor;

    // GUI Components
    CustomDial depthSlider;
    CustomDial gateSlider;
    CustomDial biasSlider;
    CustomDial mixSlider;

    CustomDial inGainSlider;
    CustomDial outGainSlider;
   // juce::ToggleButton typeChoices;

    //CustomDial expParamSlider;


    juce::TextButton effectTypeButtonsSaturation[2];
    juce::TextButton effectTypeButtonsFold[3];
    juce::TextButton effectTypeButtonsFoldScale[2];


    int activeFoldButtonIndex = -1;
    int activeFoldScaleButtonIndex = 0;

    juce::TextButton dcButton;

    // Parameter Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> biasAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> expParamAttachment;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> typeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainAttachment;


    juce::Path curvePath;
    float curvePathThickness = 7.f;
    juce::Path axisPath;

    std::random_device rd;
    std::mt19937 gen{ rd()};
    //std::uniform_real_distribution<> uniform (-1.0, 1.0);
    std::uniform_real_distribution<> uniform{ -1.0, 1.0 };

    CustomLNF myCustomLNF;

    Meter meterIn[2] = {
        Meter([&]() {return gainToMeterHight(sqrtf(2.f)*audioProcessor.rmsIn[0].getCurrentValue()); }),
        Meter([&]() {return gainToMeterHight(sqrtf(2.f) * audioProcessor.rmsIn[1].getCurrentValue()); })
    };
    Meter meterOut[2] = {
        Meter([&]() {return gainToMeterHight(sqrtf(2.f) * audioProcessor.rmsOut[0].getCurrentValue()); }),
        Meter([&]() {return gainToMeterHight(sqrtf(2.f) * audioProcessor.rmsOut[1].getCurrentValue()); })
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessorEditor)


};
