/*
  ==============================================================================

    StyleSheet.cpp
    Created: 30 Dec 2023 10:45:57pm
    Author:  miq

  ==============================================================================
*/

#include "StyleSheet.h"
float lineWbase = 15;
float reduceByBase = 0.03;


void CustomRotaryLNF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, 
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    //g.setColour(juce::Colours::black);
    //g.setGradientFill()
    
    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour(juce::Slider::rotarySliderFillColourId);
    auto thumb   = slider.findColour(juce::Slider::thumbColourId);

    int reduceBy = reduceByBase*height;

    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(reduceBy);
    //g.fillCheckerBoard(bounds, 1.f, 1.f, juce::Colours::white, juce::Colours::black);

    //g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
    float heightLabel = 3.f*sqrtf((float)width);
    g.setFont(heightLabel);
    auto label = slider.getName();
    int widthLabel = g.getCurrentFont().getStringWidth(label)+5;

        auto xLabel = bounds.getCentreX() - widthLabel/2.f;
    auto yLabel = y+height-heightLabel;
    //auto widthLabel = (width - 2 * reduceBy) * sin_;
    //auto heightLabel = (height - (yLabel - y)) / 2;
    auto boundsLabel = juce::Rectangle<int>(xLabel, yLabel, widthLabel, heightLabel);
    
    auto labelAttributed = juce::AttributedString(label);
    labelAttributed.setLineSpacing(0.3f);
    g.setColour(slider.findColour(juce::Slider::textBoxTextColourId));
    g.drawFittedText(label, boundsLabel, juce::Justification::centred, 2, 0.f);
    
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto lineW = lineWbase*height/400;
    auto arcRadius = radius - lineW * 0.5f;
    
    slider.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 
        int((width - 2.f * (lineW + reduceBy)) ),
       int( (height - 2.f * (lineW + reduceBy)) * 0.4f));


    float a1 = acosf(juce::jmin(0.99f,(float(height + reduceBy)/2.f - heightLabel) / radius));
    float a2 = asinf(juce::jmin(0.99f, (widthLabel / 2.f + lineW) / radius));
    float a3 = juce::jmin(a1, a2) + juce::MathConstants<float>::pi;
    DBG(a1);
    DBG(a2);
    DBG(a3);
    DBG("\n");
    //a3 = 1.2 * juce::MathConstants<float>::pi;
    rotaryStartAngle = a3;
    rotaryEndAngle = juce::MathConstants<float>::pi * 4 - a3;

     
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(outline);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);

        g.setColour(fill);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    //auto thumbWidth = lineW * 2.0f;
    juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

    auto xc = bounds.getCentreX();
    auto yc = bounds.getCentreY();
    auto xdir = sin(toAngle);
    auto ydir = -cos(toAngle);


    g.setColour(thumb);
    g.drawLine(xc + xdir * (arcRadius - lineW), yc + ydir * (arcRadius - lineW), xc + xdir * (arcRadius + lineW/2), yc + ydir * (arcRadius + lineW/2), lineW);

   
}

juce::Slider::SliderLayout CustomRotaryLNF::getSliderLayout(juce::Slider& slider)
{
    // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

    int minXSpace = 0;
    int minYSpace = 0;

    auto textBoxPos = slider.getTextBoxPosition();

    if (textBoxPos == juce::Slider::TextBoxLeft || textBoxPos == juce::Slider::TextBoxRight)
        minXSpace = 30;
    else
        minYSpace = 15;

    auto localBounds = slider.getLocalBounds();

    auto textBoxWidth = juce::jmax(0, juce::jmin(slider.getTextBoxWidth(), localBounds.getWidth()));
    //auto textBoxWidth = juce::jmax(0, juce::jmin(slider.getTextBoxWidth(), localBounds.getWidth() - minXSpace));
    auto textBoxHeight = juce::jmax(0, juce::jmin(slider.getTextBoxHeight(), localBounds.getHeight()));

    juce::Slider::SliderLayout layout;

    // 2. set the textBox bounds

    if (textBoxPos != juce::Slider::NoTextBox && localBounds.getHeight() > 30)
    {
        if (slider.isBar())
        {
            layout.textBoxBounds = localBounds;
        }
        else
        {
            //layout.textBoxBounds = localBounds;

            layout.textBoxBounds.setWidth(textBoxWidth);
            layout.textBoxBounds.setHeight(textBoxHeight);
            layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);
            layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);

            //if (textBoxPos == juce::Slider::TextBoxLeft)           layout.textBoxBounds.setX(0);
            //else if (textBoxPos == juce::Slider::TextBoxRight)     layout.textBoxBounds.setX(localBounds.getWidth() - textBoxWidth);
            //else /* above or below -> centre horizontally */ layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

            //if (textBoxPos == juce::Slider::TextBoxAbove)          layout.textBoxBounds.setY(0);
            //else if (textBoxPos == juce::Slider::TextBoxBelow)     layout.textBoxBounds.setY(localBounds.getHeight() - textBoxHeight);
            //else /* left or right -> centre vertically */    layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);
        }
    }

    // 3. set the slider bounds

    layout.sliderBounds = localBounds;

    //if (slider.isBar())
    //{
    //    layout.sliderBounds.reduce(1, 1);   // bar border
    //}
    //else
    //{
    //    if (textBoxPos == juce::Slider::TextBoxLeft)       layout.sliderBounds.removeFromLeft(textBoxWidth);
    //    else if (textBoxPos == juce::Slider::TextBoxRight) layout.sliderBounds.removeFromRight(textBoxWidth);
    //    else if (textBoxPos == juce::Slider::TextBoxAbove) layout.sliderBounds.removeFromTop(textBoxHeight);
    //    else if (textBoxPos == juce::Slider::TextBoxBelow) layout.sliderBounds.removeFromBottom(textBoxHeight);

    //    const int thumbIndent = getSliderThumbRadius(slider);

    //    if (slider.isHorizontal())    layout.sliderBounds.reduce(thumbIndent, 0);
    //    else if (slider.isVertical()) layout.sliderBounds.reduce(0, thumbIndent);
    //}

    return layout;
}


juce::Label* CustomRotaryLNF::createSliderTextBox(juce::Slider& slider)
{
    juce::Label*  l = LookAndFeel_V2::createSliderTextBox(slider);

    l->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId) );
    l->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    l->setBorderSize(juce::BorderSize<int>());
    l->setFont((slider.getHeight() - 2.f * ( lineWbase*slider.getHeight() / 400.f + reduceByBase* slider.getHeight())) * 0.45f);
    l->setInterceptsMouseClicks(false, false);

    //slider.mouseDoubleClick = l->mouseDoubleClick;

    return l;
}

CustomLNF::CustomLNF()
{
    static juce::Typeface::Ptr customTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::segoeprint_ttf, BinaryData::segoeprint_ttfSize);
    setDefaultSansSerifTypeface(customTypeface);
}
