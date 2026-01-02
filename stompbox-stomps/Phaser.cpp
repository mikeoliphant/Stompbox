#include <math.h>
#include <algorithm>

#include "Phaser.h"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

Phaser::Phaser()
{
	Name = "Phaser";
	Description = "Phaser effect";

	freq = 0.5f;
	depth = 0.5f;
	width = 1000;
	fb = 0.5f;
	fratio = 1.5f;
	frqwidth = 0.3f;
	
	auto& speedParam = AddParameter();
	speedParam.Name = "Speed";
	speedParam.MaxValue = 10;
	speedParam.SourceVariable = &freq;
	speedParam.DefaultValue = freq;
	speedParam.Description = "Rate of phaser sweep";

	auto& depthParam = AddParameter();
	depthParam.Name = "Depth";
	depthParam.MaxValue = 0.5;
	depthParam.SourceVariable = &depth;
	depthParam.DefaultValue = depth;
	depthParam.Description = "Strength of phaser effect";

	auto& widthParam = AddParameter();
	widthParam.Name = "FrqWidth";
	widthParam.SourceVariable = &frqwidth;
	widthParam.DefaultValue = frqwidth;
	widthParam.IsAdvanced = true;
	widthParam.Description = "Frequency width of phaser sweep";

	auto& ratioParam = AddParameter();
	ratioParam.Name = "Ratio";
	ratioParam.MinValue = 1.1f;
	ratioParam.MaxValue = 4;
	ratioParam.SourceVariable = &fratio;
	ratioParam.DefaultValue = fratio;
	ratioParam.IsAdvanced = true;
	ratioParam.Description = "Phaser frequency ratio";

	auto& feedbackParam = AddParameter();
	feedbackParam.Name = "FBack";
	feedbackParam.MinValue = 0;
	feedbackParam.MaxValue = 1;
	feedbackParam.SourceVariable = &fb;
	feedbackParam.DefaultValue = fb;
	feedbackParam.IsAdvanced = true;
	feedbackParam.Description = "Amount of phased signal feed back";
}

void Phaser::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void Phaser::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (1.0 / fConst0);
	fConst2 = (6.2831853071795862 / fConst0);
}

void Phaser::instanceClear() {
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

void Phaser::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double centerFreq = 550;
	double ratio = 1 + (frqwidth * 5);

	frqmin = centerFreq / ratio;
	frqmax = centerFreq * ratio;

	double fSlow0 = exp((fConst1 * (0.0 - (3.1415926535897931 * width))));
	double fSlow1 = mydsp_faustpower2_f(fSlow0);
	double fSlow2 = (0.0 - (2.0 * fSlow0));
	double fSlow3 = (fConst1 * fratio);
	double fSlow4 = (6.2831853071795862 * frqmin);
	double fSlow5 = (3.1415926535897931 * (frqmax - frqmin));
	double fSlow6 = (fConst2 * freq);
	double fSlow7 = sin(fSlow6);
	double fSlow8 = cos(fSlow6);
	double fSlow9 = (fConst1 * mydsp_faustpower2_f(fratio));
	double fSlow10 = (1.0 - depth);
	for (int i = 0; (i < count); i = (i + 1)) {
		double fTemp0 = double(input0[i]);
		iVec0[0] = 1;
		fRec3[0] = ((fSlow7 * fRec4[1]) + (fSlow8 * fRec3[1]));
		fRec4[0] = ((double((1 - iVec0[1])) + (fSlow8 * fRec4[1])) - (fSlow7 * fRec3[1]));
		double fTemp1 = (fSlow4 + (fSlow5 * (1.0 - fRec3[0])));
		double fTemp2 = (fRec2[1] * cos((fSlow3 * fTemp1)));
		fRec2[0] = ((fTemp0 + (fb * fRec0[1])) - ((fSlow1 * fRec2[2]) + (fSlow2 * fTemp2)));
		double fTemp3 = (fRec1[1] * cos((fSlow9 * fTemp1)));
		fRec1[0] = ((fRec2[2] + (fSlow1 * (fRec2[0] - fRec1[2]))) + (fSlow2 * (fTemp2 - fTemp3)));
		fRec0[0] = ((fRec1[2] + (fSlow2 * fTemp3)) + (fSlow1 * fRec1[0]));
		output0[i] = FAUSTFLOAT(((depth * fRec0[0]) + (fSlow10 * fTemp0)));
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

