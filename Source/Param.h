#pragma once
#include <functional>
#include <JuceHeader.h>

using String = juce::String;

typedef const std::function< String(float value, int maximumStringLength)> StrFromValFunc;

namespace strFromVal
{
	StrFromValFunc percent = [](float value, int) -> String
	{ 
		return String(int(value * 100.f)) + "%";
	};
	StrFromValFunc decimal0 = [](float value, int) -> String { return String(value, 0); };
	StrFromValFunc decimal1 = [](float value, int) -> String { return String(value, 1); };
	StrFromValFunc decimal2 = [](float value, int) -> String { return String(value, 2); };
	StrFromValFunc decimal3 = [](float value, int) -> String { return String(value, 3); };
	StrFromValFunc decimal4 = [](float value, int) -> String { return String(value, 4); };
	StrFromValFunc dbGain1 = [](float value, int) -> String
	{
		float db = 20*log10f(value*value*exp(2.75835880627f*(value-1)));
		if (db <= -100.f) {
			//return String(juce::CharPointer_UTF8("-\xE2\x88\x9E"))+"db";
			return "muted";
		}
		else {
			return String(db, 1) + "db";
		}
	};
	/*StrFromValFunc octave;
	StrFromValFunc semitone;
	StrFromValFunc centitone;
	StrFromValFunc hertz;
	StrFromValFunc degree;
	StrFromValFunc sec;
	StrFromValFunc ms;
	StrFromValFunc db;
	StrFromValFunc voices;*/
}

int sgn(int val);
float sgnf(float val);


class RotaryLookAndFeel : public juce::LookAndFeel_V3
{
public:
	juce::Label* createSliderTextBox(juce::Slider& slider) override;
	juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider);

};


