#include "Param.h"


int sgn(int val) {
	return (0 < val) - (val < 0);
}

float sgnf(float val) {
	return float((0.f < val) - (val < 0.f));
}


juce::Label* RotaryLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
	juce::Label* l = LookAndFeel_V3::createSliderTextBox(slider);
	l->setBorderSize(juce::BorderSize<int>(2, 2, 40, 2));
	//l->setColour(juce::Label::outlineColourId, juce::Colours::transparentWhite);
	//l->setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentWhite);
	return l;
}

juce::Slider::SliderLayout RotaryLookAndFeel::getSliderLayout(juce::Slider& slider)
{
	juce::Slider::SliderLayout layout = juce::LookAndFeel_V2::getSliderLayout(slider);

	auto localBounds = slider.getLocalBounds();

	auto textBoxWidth = layout.textBoxBounds.getWidth();
	auto textBoxHeight = layout.textBoxBounds.getHeight();
	layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);
	layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);

	return layout;
}



