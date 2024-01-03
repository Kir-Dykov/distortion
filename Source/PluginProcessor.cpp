/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Param.h"





//==============================================================================
DistortionAudioProcessor::DistortionAudioProcessor()
     : 
        AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        parameters(*this, 
            nullptr, // Undo manager
            juce::Identifier("PARAMETERS"), // Add an identifier for your parameters
            {
                std::make_unique<juce::AudioParameterFloat>("Depth", "Depth", 
                    juce::NormalisableRange<float>(0.f, 1.f,0.000001f,1.f,false), 0.0f, "Depth",
                    juce::AudioProcessorParameter::genericParameter, strFromVal::percent),
                std::make_unique<juce::AudioParameterFloat>("Gate", "Gate",
                    juce::NormalisableRange<float>(0.f, 1.f,0.000001f,1.f,false), 0.1f, "Gate",
                    juce::AudioProcessorParameter::genericParameter, strFromVal::percent),
                std::make_unique<juce::AudioParameterFloat>("Bias", "Bias",
                    juce::NormalisableRange<float>(-1.f, 1.f,0.000001f,1.f,false), 0.0f, "Bias",
                    juce::AudioProcessorParameter::genericParameter, strFromVal::decimal2),
                std::make_unique<juce::AudioParameterFloat>("Mix", "Mix", 
                    juce::NormalisableRange<float>(0.f, 1.f,0.0001f,1.f), 1.0f, "Mix",  
                    juce::AudioProcessorParameter::genericParameter, strFromVal::percent),

                std::make_unique<juce::AudioParameterFloat>("InGain", "InGain",
                    juce::NormalisableRange<float>(0.f, 2.f,0.0001f,1.f), 1.0f, "InGain",
                    juce::AudioProcessorParameter::genericParameter, strFromVal::dbGain1),

                std::make_unique<juce::AudioParameterFloat>("OutGain", "OutGain",
                    juce::NormalisableRange<float>(0.f, 2.f,0.0001f,1.f), 1.0f, "OutGain",
                    juce::AudioProcessorParameter::genericParameter, strFromVal::dbGain1),
                /*std::make_unique<juce::AudioParameterFloat>("ExpParam", "ExpParam",
                    juce::NormalisableRange<float>(0.1f, 10.f,0.1f,1.f), 4.0f, "ExpParam",
                    juce::AudioProcessorParameter::genericParameter),*/
                  /*std::make_unique< juce::AudioParameterChoice>("Effect Type", "Effect Type",
                             juce::StringArray{"Saturation-1", "Saturation-2", "Log Quantization", "Fuzz"}, 1),*/
                             //std::make_unique<juce::AudioParameterFloat>("Gain (post)", "Gain (post)", 0.0f, 1.0f, 0.0f),
                // Adjust the range and default value as needed
            })
{
}
float gain_map(float value) {
    return value * value * exp(2.75835880627f * (value - 1.f));
}


//DistortionAudioProcessor::DistortionAudioProcessor() 
//{
//
//}

DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

//==============================================================================
const juce::String DistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // Add the gain parameter
    depthParamTarget = parameters.getRawParameterValue("Depth");
    gateParamTarget = parameters.getRawParameterValue("Gate");
    biasParamTarget = parameters.getRawParameterValue("Bias");
    //gainParam = parameters.getRawParameterValue("Gain(post)");
    mixParamTarget = parameters.getRawParameterValue("Mix");
    inputGainParamTarget = parameters.getRawParameterValue("InGain");
    outputGainParamTarget = parameters.getRawParameterValue("OutGain");
    //expPowerParamTarget = parameters.getRawParameterValue("ExpParam");
    //effectType = parameters.getRawParameterValue("Effect Type");

    depthParam = depthParamTarget->load();
    gateParam = gateParamTarget->load();
    biasParam = biasParamTarget->load();
    mixParam = mixParamTarget->load();
    //inputGainParam = inputGainParamTarget->load();
    //outputGainParam = outputGainParamTarget->load();
    inputGainParam.reset(sampleRate, 50.f / 1000.f);
    outputGainParam.reset(sampleRate, 50.f / 1000.f);
    inputGainParam.setCurrentAndTargetValue(gain_map(inputGainParamTarget->load()));
    outputGainParam.setCurrentAndTargetValue(gain_map(outputGainParamTarget->load()));

    float rms_smoothing_time = 0.2f;
    for (int i = 0; i < 2; i++) {
        rmsIn[i].reset(sampleRate, rms_smoothing_time);
        rmsIn[i].setCurrentAndTargetValue(0.f);
        rmsOut[i].reset(sampleRate, rms_smoothing_time);
        rmsOut[i].setCurrentAndTargetValue(0.f);
    }
    
    //expPowerParam = expPowerParamTarget->load();

    /*depthParam->store(depthParam_->load());
    gateParam->store(gateParam_->load());
    biasParam->store(biasParam_->load());
    mixParam->store(mixParam_->load());
    exp_mode_powerParam->store(exp_mode_powerParam_->load());*/

    float smoothing_half_time_convergence_seconds = 0.020f;
    float smoothing_half_time_convergence_samples = sampleRate * smoothing_half_time_convergence_seconds;
    smoothing_koef = exp(-log(2)/ smoothing_half_time_convergence_samples);

    dc_k = exp(-log(2) / (sampleRate/100));
    mean[0] = mean[1] = 0.f;
}

void DistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

/*
void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
    {
        // Get the number of input and output channels
        auto totalNumInputChannels  = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        // Process each channel
        for (auto channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);

            // Apply distortion to each sample in the buffer
            for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                // Simple hard clipping distortion
                channelData[sample] = juce::jlimit(-1.0f, 1.0f, channelData[sample] * gain);
            }
        }
    }
*/


inline float DistortionAudioProcessor::gate_koef (float x, float c) {
    if (c < 0.0001f) 
        return 1.f;
    float xx = x / (c*c);
    return (1 - exp(-xx * xx * xx * xx));
};




void DistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //juce::ScopedNoDenormals noDenormals;

    // Get the number of input and output channels
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto totalNumSamples = buffer.getNumSamples();

    std::cout << "HELLO CONSOLE" << std::endl;
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    float* channelData[32];

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        channelData[channel] = buffer.getWritePointer(channel);
    }


    /*float depthIncrement = (depthParamTarget->load() - depthParam) / totalNumSamples;
    float gateIncrement = (gateParamTarget->load() - gateParam) / totalNumSamples;
    float biasIncrement = (biasParamTarget->load() - biasParam) / totalNumSamples;
    float mixIncrement = (mixParamTarget->load() - mixParam) / totalNumSamples;
    float expIncrement = (expPowerParamTarget->load() - expPowerParam) / totalNumSamples;*/



        //gateParam =  gateParamTarget->load();
        //biasParam = biasParamTarget->load();
        //mixParam =  mixParamTarget->load(); 

    for (int i = 0; i < 2; i++) {
        rmsInLocal[i] = buffer.getRMSLevel(i, 0, totalNumSamples) * gain_map(inputGainParam.getCurrentValue());
        if (rmsInLocal[i] > rmsIn[i].getCurrentValue()) { rmsIn[i].setCurrentAndTargetValue(rmsInLocal[i]); }
        else { rmsIn[i].setTargetValue(rmsInLocal[i]); }
    }
      
    rmsOutLocal[0] = rmsOutLocal[1] = 0.;

    for (auto sample = 0; sample < totalNumSamples; ++sample)
    {  
        

        //depthParam += depthIncrement;
        //gateParam += gateIncrement;
        //biasParam += biasIncrement;
        //mixParam += mixIncrement;

        depthParam = smoothing_koef * depthParam + (1 - smoothing_koef) * depthParamTarget->load();
        gateParam = smoothing_koef * gateParam + (1 - smoothing_koef) * gateParamTarget->load();
        biasParam = smoothing_koef * biasParam + (1 - smoothing_koef) * biasParamTarget->load();
        mixParam = smoothing_koef * mixParam + (1 - smoothing_koef) * mixParamTarget->load();
        //expPowerParam = smoothing_koef * expPowerParam + (1 - smoothing_koef) * expPowerParamTarget->load();

        inputGainParam.setTargetValue(inputGainParamTarget->load());
        outputGainParam.setTargetValue(outputGainParamTarget->load());

        float inGain = gain_map(inputGainParam.getNextValue());
        float outGain = gain_map(outputGainParam.getNextValue());
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            float gate_k = gate_koef(channelData[channel][sample] * inGain, gateParam);
            float zero_level = current_effect_function(biasParam, depthParam);

            float wet = (gate_k * current_effect_function(channelData[channel][sample] * inGain + biasParam, depthParam) + (1 - gate_k) * zero_level);

            if (dcIsOn) {
                mean[channel] = dc_k * mean[channel] + (1 - dc_k) * wet;
                //mean[channel] = double(dc_k) * (double(mean[channel]) - double(wet)) + double(wet);
                wet -= mean[channel];
            }

            wet *= outGain;


            rmsOutLocal[channel] += wet * wet;
            channelData[channel][sample] = (1.f - mixParam) * channelData[channel][sample] + mixParam * wet;
        }
    }

    for (int i = 0; i < 2; i++) {
        rmsOutLocal[i] = sqrt(rmsOutLocal[i] / totalNumSamples);
        if (rmsOutLocal[i] > rmsOut[i].getCurrentValue()) { rmsOut[i].setCurrentAndTargetValue(rmsOutLocal[i]); }
        else { rmsOut[i].setTargetValue(rmsOutLocal[i]); }
    }
   
    //DBG(mean[1]);
    //DBG(channelData[1][0]);

    rmsIn[0].skip(totalNumSamples);
    rmsIn[1].skip(totalNumSamples);
    rmsOut[0].skip(totalNumSamples);
    rmsOut[1].skip(totalNumSamples);
}

//==============================================================================
bool DistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortionAudioProcessor::createEditor()
{
    //// Create the GUI editor and pass the AudioProcessorValueTreeState
    //return new juce::GenericAudioProcessorEditor(*this);
     return new DistortionAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get()) {
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionAudioProcessor();
}
