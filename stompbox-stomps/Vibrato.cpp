#include <math.h>
#include <algorithm>

#include "Vibrato.h"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

Vibrato::Vibrato()
{
	Name = "Vibrato";
	Description = "Pitch modulation effect";

	freq = 2;
	depth = 0.5;
	width = 1000.0;
	fb = 0.5;   // feedback
	fratio = 1.5;

	auto& speedParam = AddParameter();
	speedParam.Name = "Speed";
	speedParam.MinValue = 0.1f;
	speedParam.MaxValue = 10;
	speedParam.SourceVariable = &freq;
	speedParam.DefaultValue = freq;
	speedParam.Description = "Rate of the pitch modulation";

	auto& depthParam = AddParameter();
	depthParam.Name = "Depth";
	depthParam.SourceVariable = &depth;
	depthParam.DefaultValue = depth;
	depthParam.Description = "Strength of the pitch modulation";

	auto& feedbackParam = AddParameter();
	feedbackParam.Name = "FBack";
	feedbackParam.MinValue = 0;
	feedbackParam.MaxValue = 1;
	feedbackParam.SourceVariable = &fb;
	feedbackParam.DefaultValue = fb;
	feedbackParam.IsAdvanced = true;
	feedbackParam.Description = "Amount of feedback applied";

	auto& ratioParam = AddParameter();
	ratioParam.Name = "Ratio";
	ratioParam.MinValue = 1.1f;
	ratioParam.MaxValue = 4;
	ratioParam.SourceVariable = &fratio;
	ratioParam.DefaultValue = fratio;
	ratioParam.IsAdvanced = true;
	ratioParam.Description = "Modulation frequency ratio";
}

void Vibrato::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void Vibrato::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (1.0 / fConst0);
	fConst2 = (6.2831853071795862 / fConst0);
}

void Vibrato::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		iVec0[l0] = 0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		fRec3[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec4[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 3); l3 = (l3 + 1)) {
		fRec2[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 3); l4 = (l4 + 1)) {
		fRec1[l4] = 0.0;

	}
	for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
		fRec0[l5] = 0.0;

	}
}

void Vibrato::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	float centerFreq = 200;
	float ratio = depth * 10;

	frqmin = centerFreq / ratio;
	frqmax = centerFreq * ratio;

	double fSlow0 = exp((fConst1 * (0.0 - (3.1415926535897931 * width))));
	double fSlow1 = mydsp_faustpower2_f(fSlow0);
	double fSlow2 = (0.0 - (2.0 * fSlow0));
	double fSlow3 = (fConst2 * frqmin);
	double fSlow4 = (0.5 * (0.0 - (fConst2 * (frqmin - frqmax))));
	double fSlow5 = (fConst2 * freq);
	double fSlow6 = sin(fSlow5);
	double fSlow7 = cos(fSlow5);
	double fSlow8 = mydsp_faustpower2_f(fratio);
	for (int i = 0; (i < count); i = (i + 1)) {
		iVec0[0] = 1;
		fRec3[0] = ((fSlow6 * fRec4[1]) + (fSlow7 * fRec3[1]));
		fRec4[0] = ((double((1 - iVec0[1])) + (fSlow7 * fRec4[1])) - (fSlow6 * fRec3[1]));
		double fTemp0 = (fSlow3 + (fSlow4 * (1.0 - fRec3[0])));
		double fTemp1 = (fRec2[1] * cos((fratio * fTemp0)));
		fRec2[0] = ((double(input0[i]) + (fb * fRec0[1])) - ((fSlow2 * fTemp1) + (fSlow1 * fRec2[2])));
		double fTemp2 = (cos((fSlow8 * fTemp0)) * fRec1[1]);
		fRec1[0] = (((fSlow1 * (fRec2[0] - fRec1[2])) + fRec2[2]) - (fSlow2 * (fTemp2 - fTemp1)));
		fRec0[0] = (fRec1[2] + ((fSlow1 * fRec1[0]) + (fSlow2 * fTemp2)));
		output0[i] = FAUSTFLOAT(fRec0[0]);
		iVec0[1] = iVec0[0];
		fRec3[1] = fRec3[0];
		fRec4[1] = fRec4[0];
		fRec2[2] = fRec2[1];
		fRec2[1] = fRec2[0];
		fRec1[2] = fRec1[1];
		fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];

	}
}

