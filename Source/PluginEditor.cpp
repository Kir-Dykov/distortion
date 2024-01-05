/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"    
#include "PluginEditor.h"
#include "Param.h"

const int windowSize = 512;
const int textBoxHeight = 20;
const int border = 0;

//=================================================================================



//==============================================================================
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor (DistortionAudioProcessor& processor)
    : AudioProcessorEditor (&processor), audioProcessor (processor), 
    depthSlider("depth"), mixSlider("mix"), gateSlider("gate"), biasSlider("bias"), inGainSlider("pre"), outGainSlider("post"),
    depthAttachment(audioProcessor.parameters, "Depth", depthSlider)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLNF);
    juce::Font::setDefaultMinimumHorizontalScaleFactor(.1f);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //setSize (300, 100);

    //choiceButtonComponent = ChoiceButtonComponent();

    // Set the size of the GUI editor
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(1.0);
    setResizeLimits(150, 150, 10000, 10000);
    setSize(windowSize, windowSize);

    // Add the gain slider to the GUI
    addAndMakeVisible(depthSlider);
    addAndMakeVisible(gateSlider);
    addAndMakeVisible(biasSlider);

    //addAndMakeVisible(expParamSlider);
    addAndMakeVisible(mixSlider);

    addAndMakeVisible(inGainSlider);
    addAndMakeVisible(outGainSlider);
    
    for (int i = 0; i < 2; i++) {
        addAndMakeVisible(meterIn[i]);
        addAndMakeVisible(meterOut[i]);
    }
    
    /*addAndMakeVisible(depthLabel);
    depthLabel.setJustificationType(juce::Justification::centred);
    depthLabel.attachToComponent(&depthSlider, false);
    depthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    depthLabel.setText("Value", juce::dontSendNotification);*/

    //const int labelPadding = 0;
    //const int labelHeight = 20;
    //depthLabel.setBounds(labelPadding, labelPadding, getWidth() - 2 * labelPadding, labelHeight);

    //addAndMakeVisible(typeChoices);
    //addAndMakeVisible(choiceButtonComponent);
    //depthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    //depthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow,true,40, textBoxHeight);
    ////depthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 40, textBoxHeight);

    //gateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    //gateSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, textBoxHeight);

    //biasSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    //biasSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, textBoxHeight);

    //expParamSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    //expParamSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, textBoxHeight);

    //mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    ////mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, textBoxHeight);
    //mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, textBoxHeight);

    depthSlider.onValueChange = [this]() -> void { this->repaint(); return; };
    gateSlider.onValueChange = [this]() -> void { this->repaint(); return; };
    biasSlider.onValueChange = [this]() -> void { this->repaint(); return; };
    mixSlider.onValueChange = [this]() -> void { this->repaint(); return; };
    //expParamSlider.onValueChange = [this]() -> void { this->repaint(); return; };
    //expParamSlider.onDragEnd = [this]() -> void { this->repaint(); return; };


    // Attach the gain parameter to the slider
    /*depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, "Depth", depthSlider);*/

    gateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, "Gate", gateSlider);

    biasAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, "Bias", biasSlider);

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, "Mix", mixSlider);

    inGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, "InGain", inGainSlider);

    outGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.parameters, "OutGain", outGainSlider);

    //expParamAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    //    processor.parameters, "ExpParam", expParamSlider);


    

    //getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::white);
    //getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::black);
    //getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);


    //choiceButtonComponent.setBounds(0, getHeight() * 0.25, getWidth() * 0.25, getHeight() * 0.5);
    //typeChoices.setBounds(0, getHeight() * 0.25, getWidth() * 0.25, getHeight() * 0.5);
    //mixSlider.setBounds(10, 30, getWidth() - 20, 40);



    addAndMakeVisible(dcButton);
    dcButton.setButtonText("dc");
    dcButton.setClickingTogglesState(true);
    dcButton.onClick = [this] {
        audioProcessor.dcIsOn ^= true;
    };
    dcButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::black); // Set default color
    dcButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue); // Set default color  


    const int effectTypeButtonX = 10;
    const int effectTypeButtonY = getHeight() / 4;
    const int effectTypeButtonPad = 10;
    const int effectTypeButtonWidth = getWidth() / 4 - 2 * effectTypeButtonX;
    const int effectTypeButtonHeight = (getHeight() / 2 - 5 * effectTypeButtonPad) / 4;


    juce::String effectTypeButtonsSaturationNames [2] = {"clip", "jam"};
    
    for (int i = 0; i < 2; i++) {
        addAndMakeVisible(effectTypeButtonsSaturation[i]);
        effectTypeButtonsSaturation[i].setButtonText(effectTypeButtonsSaturationNames[i]);
        effectTypeButtonsSaturation[i].onClick = [this, i]
                { 
                    audioProcessor.current_effect_function = audioProcessor.distortion_functions[i]; 
                    for (int j = 0; j < 2; j++) {
                        effectTypeButtonsSaturation[j].setToggleState(i==j, juce::dontSendNotification);
                    }
                    for (int j = 0; j < 3; j++) {
                        effectTypeButtonsFold[j].setToggleState(false, juce::dontSendNotification);
                    }
                    activeFoldButtonIndex = -1;
                    curvePathThickness = 7.f;
                   
                    this->repaint(); 
                };

        int pad = 3;
        int width = int((0.25f * getWidth() - 3 * pad) * 0.5f);
        int height = 30;
        effectTypeButtonsSaturation[i].setBounds(0.75f*getWidth()+pad + i*(width + pad), 0.25f * getHeight() + pad, width, height);

        effectTypeButtonsSaturation[i].setColour(juce::TextButton::buttonOnColourId, juce::Colours::black); // Set default color
        effectTypeButtonsSaturation[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue); // Set default color  
    }
    
    juce::String effectTypeButtonsFoldNames[3] = { juce::CharPointer_UTF8("\xE2\x96\xA0"), juce::CharPointer_UTF8("\xE2\x96\xB2"), juce::CharPointer_UTF8("\xE2\x97\x8F") };
    //juce::String effectTypeButtonsFoldNames[3] = { "Q", "T", "S" };
    for (int i = 0; i < 3; i++) {
        addAndMakeVisible(effectTypeButtonsFold[i]);
        effectTypeButtonsFold[i].setButtonText(effectTypeButtonsFoldNames[i]);
        effectTypeButtonsFold[i].onClick = [this, i]
        {
            activeFoldButtonIndex = i;
            audioProcessor.current_effect_function = audioProcessor.distortion_functions[2 + activeFoldScaleButtonIndex + 2 * activeFoldButtonIndex];
            for (int j = 0; j < 2; j++) {
                effectTypeButtonsSaturation[j].setToggleState(false, juce::dontSendNotification);
            }
            for (int j = 0; j < 3; j++) {
                effectTypeButtonsFold[j].setToggleState(i==j, juce::dontSendNotification);
            }
            curvePathThickness = 3.f;
            this->repaint();
        };

        
        effectTypeButtonsFold[i].setColour(juce::TextButton::buttonOnColourId, juce::Colours::black); // Set default color
        effectTypeButtonsFold[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue); // Set default color  
    }

    juce::String effectTypeButtonsFoldScaleNames[2] = { "log", "lin" };
    for (int i = 0; i < 2; i++) {
        addAndMakeVisible(effectTypeButtonsFoldScale[i]);
        effectTypeButtonsFoldScale[i].setButtonText(effectTypeButtonsFoldScaleNames[i]);
        effectTypeButtonsFoldScale[i].onClick = [this, i]
        {
            activeFoldScaleButtonIndex = i;
            for (int j = 0; j < 2; j++) {
                effectTypeButtonsFoldScale[j].setToggleState(i == j, juce::dontSendNotification);
            }
            if (activeFoldButtonIndex >= 0) {
                
                audioProcessor.current_effect_function = audioProcessor.distortion_functions[2 + activeFoldScaleButtonIndex + 2*activeFoldButtonIndex];
            };
            
            this->repaint();
        };

        effectTypeButtonsFoldScale[i].setColour(juce::TextButton::buttonOnColourId, juce::Colours::black); // Set default color
        effectTypeButtonsFoldScale[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue); // Set default color  
    }

    effectTypeButtonsSaturation[1].setToggleState(true, juce::dontSendNotification);
    effectTypeButtonsFoldScale[activeFoldScaleButtonIndex].setToggleState(true, juce::dontSendNotification);

    //for (int i = 0; i < effects_number; i++) {
    //    addAndMakeVisible(effectTypeButtons[i]);
    //    effectTypeButtons[i].setButtonText(effectTypeButtonsNames[i]);
    //    effectTypeButtons[i].onClick = [this, i, effects_number]
    //    { 
    //        audioProcessor.current_effect_function = audioProcessor.distortion_functions[i]; 
    //        for (int j = 0; j < effects_number; j++) {
    //            effectTypeButtons[j].setToggleState(i==j, juce::dontSendNotification);
    //        }
    //        this->repaint(); 
    //    };
    //    effectTypeButtons[i].setToggleState(i == 1, juce::dontSendNotification);
    //    effectTypeButtons[i].setBounds(
    //        effectTypeButtonX,
    //        effectTypeButtonY + (i+1) * effectTypeButtonPad + i * effectTypeButtonHeight,
    //        effectTypeButtonWidth,
    //        effectTypeButtonHeight);

    //    // effectTypeButtons[i].setToggleable(true); // Enable toggle behavior
    //    effectTypeButtons[i].setColour(juce::TextButton::buttonOnColourId, juce::Colours::black); // Set default color
    //    effectTypeButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey); // Set default color        
    //}


    /*addAndMakeVisible(effectTypeButtonSaturation1);
    effectTypeButtonSaturation1.setButtonText("Saturation-1");
    effectTypeButtonSaturation1.onClick = [this] 
    { audioProcessor.current_effect_function = audioProcessor.distortion_functions[0]; this->repaint(); };
    effectTypeButtonSaturation1.setBounds(
        effectTypeButtonX,
        effectTypeButtonY + effectTypeButtonPad,
        effectTypeButtonWidth,
        effectTypeButtonHeight);

    addAndMakeVisible(effectTypeButtonSaturation2);
    effectTypeButtonSaturation2.setButtonText("Saturation-2");
    effectTypeButtonSaturation2.onClick = [this]
    { audioProcessor.current_effect_function = audioProcessor.distortion_functions[1]; this->repaint(); };
    effectTypeButtonSaturation2.setBounds(
        effectTypeButtonX,
        effectTypeButtonY + 2*effectTypeButtonPad + effectTypeButtonHeight,
        effectTypeButtonWidth,
        effectTypeButtonHeight);

    addAndMakeVisible(effectTypeButtonLogQuantization);
    effectTypeButtonLogQuantization.setButtonText("Log Quantization");
    effectTypeButtonLogQuantization.onClick = [this]
    { audioProcessor.current_effect_function = audioProcessor.distortion_functions[2]; this->repaint(); };
    effectTypeButtonLogQuantization.setBounds(
        effectTypeButtonX,
        effectTypeButtonY + 3 * effectTypeButtonPad + 2*effectTypeButtonHeight,
        effectTypeButtonWidth,
        effectTypeButtonHeight);

    addAndMakeVisible(effectTypeFuzz);
    effectTypeFuzz.setButtonText("Fuzzzzzzzz");
    effectTypeFuzz.onClick = [this]
    { audioProcessor.current_effect_function = audioProcessor.distortion_functions[3]; this->repaint(); };
    effectTypeFuzz.setBounds(
        effectTypeButtonX,
        effectTypeButtonY + 4 * effectTypeButtonPad + 3 * effectTypeButtonHeight,
        effectTypeButtonWidth,
        effectTypeButtonHeight);*/


    

    //typeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
    //    processor.parameters, "Effect Type", typeChoices);

    /*choiceButtonComponentAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.parameters, "Effect Type", *(choiceButtonComponent.buttons[0])    );*/

    

}

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void DistortionAudioProcessorEditor::paint (juce::Graphics& g)
{

    float width = getWidth();
    float height = getHeight();

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //background
    g.fillAll(juce::Colours::grey.withAlpha(1.0f));

    juce::ColourGradient cg;
    
    cg = juce::ColourGradient::vertical(
        juce::Colour::fromRGBA(255, 0, 255, 255), 0.0,
        juce::Colour::fromRGBA(100, 0, 255, 255), height
    );
    g.setGradientFill(cg);
    g.fillAll();

    cg = juce::ColourGradient::horizontal(
        juce::Colour::fromRGBA(0, 0, 255, 100), 0.0,
        juce::Colour::fromRGBA(100, 255, 255, 100), width
    );
    g.setGradientFill(cg);
    g.fillAll();

    

    // unit rectangle for reference
    g.setColour (juce::Colours::white);
    //g.drawLine(getWidth()*0.25, getHeight()*0.75, getWidth() * 0.75, getHeight() * 0.25, 2.0f);
    //g.drawRect(border + (getWidth() - 2*border)*0.25, border + (getHeight() - 2 * border) * 0.25, 
    //    (getWidth() - 2 * border) * 0.5, (getHeight() - 2 * border) * 0.5, 1.0f);
    //juce::Path axisPath;
    axisPath.clear();
    float curve_round = 0.03 * width;
    axisPath.startNewSubPath(width*0.5, height*0.5);
    axisPath.lineTo(width * 0.5, height * 0.25 + curve_round);
    axisPath.quadraticTo(width * 0.5, height * 0.25, width * 0.5 + curve_round, height * 0.25);
    axisPath.lineTo(width * 0.75 - curve_round, height * 0.25);
    axisPath.quadraticTo(width * 0.75, height * 0.25, width * 0.75 - curve_round, height * 0.25 + curve_round);
    axisPath.lineTo(width * 0.25 + curve_round, height * 0.75 - curve_round);
    axisPath.quadraticTo(width * 0.25, height * 0.75, width * 0.25 + curve_round, height * 0.75);
    axisPath.lineTo(width * 0.5 - curve_round, height * 0.75);
    axisPath.quadraticTo(width * 0.5, height * 0.75, width * 0.5 , height * 0.75 - curve_round);
    axisPath.lineTo(width * 0.5, height * 0.5);




    //axisPath.lineTo(getWidth() * 0.5, getWidth() * 0.75);
    //axisPath.lineTo(getWidth() * 0.5, getWidth() * 0.25);
    //axisPath.lineTo(getWidth() * 0.75, getWidth() * 0.25);
    //axisPath.lineTo(getWidth() * 0.25, getWidth() * 0.75);
    g.strokePath(axisPath, juce::PathStrokeType(
        5.0f*width/512.f,
        juce::PathStrokeType::JointStyle::curved
    ));

    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World", getLocalBounds(), juce::Justification::right | juce::Justification::top, 1);

 

    

    /// DRAW A DISTORTION CURVE
    float x0 = getWidth() * 0.5f;
    float y0 = getHeight() * 0.5f;
    float x1 = (getWidth() * 0.5 - border) * 0.5;
    float y1 = (getWidth() * 0.5 - border) * 0.5;
    float step = 4.f / windowSize / 4.f;
    float x = -2.f * x1 + x0;
    float y = y0 - y1 * audioProcessor.current_effect_function(-2.f + audioProcessor.biasParamTarget->load(), audioProcessor.depthParamTarget->load());
    curvePath.clear();
    curvePath.startNewSubPath(x, y);
    //g.setColour(juce::Colour(0.f,0.f,0.f,1.f));
    //g.setColour(juce::Colours::black.withAlpha(1.0f));
    //g.setColour(juce::Colour::fromRGB(50,0,50));

    cg = juce::ColourGradient::horizontal(
        juce::Colour::fromRGB(0, 0, 50), 0.0,
        juce::Colour::fromRGB(50, 0, 0), width
    );
    g.setGradientFill(cg);

    for (float i = -2.f+step; i < 2.f; i += step) {
        x = i * x1 + x0;
        //y = y0 - y1 * audioProcessor.current_effect_function(audioProcessor.gate_coef(i) * i - audioProcessor.biasParam->load());
        float k = audioProcessor.current_effect_function(audioProcessor.biasParamTarget->load(), audioProcessor.depthParamTarget->load());
        float c = audioProcessor.gate_koef(i, audioProcessor.gateParamTarget->load());
            
        y = y0 - y1 * (c * audioProcessor.current_effect_function(i + audioProcessor.biasParamTarget->load(), audioProcessor.depthParamTarget->load()) + (1 - c)*k);
        curvePath.lineTo(x, y);
    }
    g.strokePath(curvePath, juce::PathStrokeType(
        curvePathThickness * width / 512.f,
        juce::PathStrokeType::JointStyle::curved    
    ));


    {
        g.setColour(juce::Colours::white);

        float k = audioProcessor.current_effect_function(audioProcessor.biasParamTarget->load(), audioProcessor.depthParamTarget->load());
        float c = audioProcessor.gate_koef(0., audioProcessor.gateParamTarget->load());

        y = y0 - y1 * (c * audioProcessor.current_effect_function(audioProcessor.biasParamTarget->load(), audioProcessor.depthParamTarget->load()) + (1 - c) * k);
        float x_delta = 0.08 * width * audioProcessor.gateParamTarget->load() * audioProcessor.gateParamTarget->load();
        g.drawLine(x0 - x_delta, y, x0 + x_delta, y, 2.f * width / 512.f);
    }
    

}

void DistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const int dialWidth1 = getWidth() / 4;
    const int dialWidth2 = getWidth() / 4;
    // Set the position and size of GUI components
    depthSlider.setBounds(getWidth() * 0.25, getHeight() * 0.25, getWidth() * 0.25, getWidth() * 0.25);
    gateSlider.setBounds(getWidth() * 0.51, getHeight() * (0.49 + 0.125), getWidth() * 0.125, getWidth() * 0.150);
    biasSlider.setBounds(getWidth() * (0.49 + 0.125), getHeight() * 0.51, getWidth() * 0.125, getWidth() * 0.150);

    dcButton.setBounds(getWidth()*(0.505 + 0.125*0.75 - 0.05), getHeight()*(0.505 + 0.125*0.75 - 0.05), getWidth() * 0.05, getWidth() * 0.05);
    /*{
        int width = getWidth() / 4 / 4;
        expParamSlider.setBounds(getWidth() - width, 0.25 * getHeight() + 40 + 3 + (0.25 * getWidth() - 3 * 3) * 0.25, width, width);

    }*/

    mixSlider.setBounds(getWidth() - dialWidth2, getHeight() - dialWidth2,
        dialWidth2, dialWidth2);
   
    inGainSlider.setBounds(getWidth() * 0, getHeight() * 0.75 - getWidth() * 0.125, getWidth() * 0.125, getWidth() * 0.125);
    outGainSlider.setBounds(getWidth() * 0.125, getHeight() * 0.75 - getWidth() * 0.125, getWidth() * 0.125, getWidth() * 0.125);


    for (int i = 0; i < 2; i++) {
        int pad = 3;
        int width = (0.25 * getWidth() - 3 * pad) * 0.5;
        int height = 30;
        effectTypeButtonsSaturation[i].setBounds(0.75 * getWidth() + pad + i * (width + pad), 0.25 * getHeight() + pad, width, height);

        //effectTypeButtonsSaturation[i].setColour(juce::TextButton::buttonOnColourId, juce::Colours::black); // Set default color
        //effectTypeButtonsSaturation[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey); // Set default color  
    }

    {
        int pad = 3;
        int pad2 = 40;
        int width = (0.25 * getWidth() - 3 * pad) * 0.25;
        int height = width;

        effectTypeButtonsFold[0].setBounds(0.75 * getWidth() + pad + (width + pad), 0.25 * getHeight() + pad2 + pad + height, width, height);
        effectTypeButtonsFold[1].setBounds(0.75 * getWidth() + pad, 0.25 * getHeight() + pad2, width, height);
        effectTypeButtonsFold[2].setBounds(0.75 * getWidth() + pad + (width + pad), 0.25 * getHeight() + pad2, width, height);

    }

    {
        int pad = 3;
        int pad2 = 40;
        int width = (0.25 * getWidth() - 3 * pad) * 0.25;
        int height = width;

        effectTypeButtonsFoldScale[0].setBounds(0.75 * getWidth() + pad + 2 * (width + pad), 0.25 * getHeight() + pad2, width, height);
        effectTypeButtonsFoldScale[1].setBounds(0.75 * getWidth() + pad + 3 * (width + pad), 0.25 * getHeight() + pad2, width, height);
        //effectTypeButtonsFoldScale[2].setBounds(0.75 * getWidth() + pad + 2 * (width + pad), 0.25 * getHeight() + pad2 + pad + height, width, height);

    }

    {
        float meter_width = getWidth()*0.03;
        float pad = (getWidth() * 0.25f - meter_width * 4) / 6.f;
        
        meterIn[0].setBounds(pad, getHeight() * 0.25, meter_width, getHeight() * (0.5 - 0.125));
        meterIn[1].setBounds(pad * 2 + meter_width, getHeight() * 0.25, meter_width, getHeight() * (0.5 - 0.125));
        meterOut[0].setBounds(pad * 4 + 2 * meter_width, getHeight() * 0.25, meter_width, getHeight() * (0.5 - 0.125));
        meterOut[1].setBounds(pad * 5 + 3 * meter_width, getHeight() * 0.25, meter_width, getHeight() * (0.5 - 0.125));
    }

}
