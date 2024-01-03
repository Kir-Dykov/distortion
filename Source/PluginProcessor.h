/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define _USE_MATH_DEFINES
#include <JuceHeader.h>
#include "Param.h"
#include <cmath>

//==============================================================================
/**
*/
class DistortionAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DistortionAudioProcessor();
    ~DistortionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    /*std::atomic<float>* depthParam = new std::atomic<float>;
    std::atomic<float>* gateParam = new std::atomic<float>;
    std::atomic<float>* biasParam = new std::atomic<float>;
    std::atomic<float>* mixParam = new std::atomic<float>;
    std::atomic<float>* exp_mode_powerParam = new std::atomic<float>;*/


    float depthParam;
    float gateParam;
    float biasParam;
    float mixParam;

    juce::LinearSmoothedValue<float> inputGainParam;
    juce::LinearSmoothedValue<float> outputGainParam;
    //float expPowerParam;

    std::atomic<float>* depthParamTarget;
    std::atomic<float>* gateParamTarget;
    std::atomic<float>* biasParamTarget;
    std::atomic<float>* mixParamTarget;

    std::atomic<float>* inputGainParamTarget;
    std::atomic<float>* outputGainParamTarget;

    //std::atomic<float>* expPowerParamTarget;



   // std::atomic<float>* effectType; //it's int actually
    juce::AudioProcessorValueTreeState parameters; 


    juce::LinearSmoothedValue<float> rmsIn[2];
    juce::LinearSmoothedValue<float> rmsOut[2];
    float rmsInLocal[2];
    float rmsOutLocal[2];



    /*std::function< float(float)> saturation_1 = [this](float x) {
        float L = 2.f*(1.00001f - depthParam->load(std::memory_order_relaxed));
        float isClip = (abs(x) > L);
        return x * (1.f - mixParam->load(std::memory_order_relaxed))
            + (mixParam->load(std::memory_order_relaxed)) * (
                (1.f - isClip) * (2.f * x * L - x * abs(x))/(L*L) +
                isClip * sgnf(x)
                );
    };*/

    std::function< float(float, float)> clip = [this](float x, float c2) {
        c2 = c2 * c2 * 0.25f;
        return 0.5 * (sqrtf((x+1)*(x+1)+c2) - sqrtf((x-1)*(x-1)+c2));
    };

    std::function< float(float, float)> jam = [this](float x, float c) {
        c = 1 - c;
        c = c * c * c;
        c = 1 - c;
        return x / (1 + c * (abs(x) - 0.9999999));
    };


    std::function< float(float, float)> quantize_log = [this](float x, float c) {
        float b = log(0.9999999f - 0.9999991f * c);
        return sgnf(x) * exp(b * floorf(log(abs(x) + 0.000000001f) / b));
    };

    std::function< float(float, float)> quantize_lin = [this](float x, float c) {
        float b = -0.00000001f - c;
        return sgnf(x) * (b * (floorf((abs(x)) / b) + 0.5f));
    };

    /*std::function< float(float, float)> quantize_exp = [this](float x, float c) {
        float b = exp(expPowerParam * c) - 0.99999f;
        return sgnf(x) * log(1.f + b * floorf((exp(expPowerParam *abs(x)) - 1.f )/ b + 1.f))/ expPowerParam;
    };*/


    std::function< float(float, float)> triangle_log = [this](float x, float c) {
        c = 1 - c;
        c = c * c * c;
        c = 1 - c;
        float xx = abs(log(abs(x)+0.000000001f)) * c / (1.000001f - c * c);
        return x * (-1 + 4 * abs(xx - int(xx) - 0.5f));
    };

    std::function< float(float, float)> triangle_lin = [this](float x, float c) {
        c = 0.9999 - c;
        float cInv = c * c * c;
        c = 1 - cInv;
        float xx = abs(abs(x) * c / (1.000001f - c * c) - 0.25);
        float d = cInv / ((4 - 3 * (1 - cInv * cInv))*(c+0.0000001));
        return sgnf(x) * sqrtf(x*x + d*d) * (-1 + 4 * abs(xx - int(xx) - 0.5f));
    };

    //std::function< float(float, float)> triangle_exp = [this](float x, float c) {
    //    c = 1 - c;
    //    float cInv = c * c * c;
    //    c = 1 - c;
    //    //float exp_mode_powerParam_ = 4;
    //    float xx = abs((exp(expPowerParam *abs(x))-1.f) * c / (1.000001f - c * c) - 0.25);
    //    //float d = cInv / ((14 - 13 * (1 - cInv * cInv)) * (c + 0.0000001));
    //    return x * (-1 + 4 * abs(xx - int(xx) - 0.5f));
    //    //return x * (-1 + 4 * abs(xx - int(xx) - 0.5f));
    //};

    std::function< float(float, float)> sine_log = [this](float x, float c) {
        c = 1 - c;
        c = c * c * c;
        c = 1 - c;
        float xx = abs(log(abs(x) + 0.000000001f)) * c / (1.000001f - c * c);
        return x * cos(2.f * M_PI * xx);
    };

    std::function< float(float, float)> sine_lin = [this](float x, float c) {
        c = 1 - c;
        float d = c * c * c;
        c = 1 - d;
        d *= 0.75;
        float xx = (sqrtf(x*x + d*d) - d) * c / (1.000001f - c * c);
        return x * cos(2.f * M_PI * xx);
    };

    /*std::function< float(float, float)> sine_exp = [this](const float& x, float c) {
        c = 1 - c;
        c = c * c * c;
        c = 1 - c;
        float xx = (exp(expPowerParam * abs(x))-1.f) * c / (1.000001f - c * c);
        return x * cos(2.f * M_PI * xx);
    };*/


    inline float gate_koef(float, float);
    /*std::function< float(float)> gate = [this](float x) {
        float xx = x / (0.0000001 + depthParam->load(std::memory_order_relaxed));
        return x * (1 - exp(-xx * xx));
    };*/

    /*std::function< float(float)> saturation_2 = [this](float x) {
        float c = depthParam->load(std::memory_order_relaxed);
        c = 1 - c;
        c = c * c * c;
        c = 1 - c;
        return x * (1 - mixParam->load(std::memory_order_relaxed))
            + (mixParam->load(std::memory_order_relaxed)) * x / (1 + c * (abs(x) - 0.9999999));
    };*/
       
    /*std::function< float(float)> log_quiantization = [this](float x) {
        float b = log(0.9999999 - 0.9999991*depthParam->load(std::memory_order_relaxed));
        return x * (1 - mixParam->load(std::memory_order_relaxed))
            + (mixParam->load(std::memory_order_relaxed)) * sgnf(x)*exp(b*floorf(log(abs(x))/b));
    };

    std::function< float(float)> fuzz = [this](float x) {
        float c = depthParam->load(std::memory_order_relaxed);
        c = 1 - c;
        c = c * c * c;
        c = 1 - c;
        float xx = abs(x) * c / (1.000001 - c * c);
        return x * (1 - mixParam->load(std::memory_order_relaxed))
            + (mixParam->load(std::memory_order_relaxed))* x * (-1 + 4*abs(xx - int(xx) - 0.5));
    };*/

    static const int effects_number = 2 + 3*3;

    const std::function< float(float, float)> distortion_functions [effects_number] = {
        clip,
        jam,
        quantize_log,
        quantize_lin,
        //quantize_exp,
        triangle_log,
        triangle_lin,
        //triangle_exp,
        sine_log,
        sine_lin,
        //sine_exp
    };

    float smoothing_koef;

    bool dcIsOn = false;
    double dc_k;
    double mean[2] = { 0, 0 };

    std::function< float(float, float)> current_effect_function = distortion_functions[1];

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessor)
};
