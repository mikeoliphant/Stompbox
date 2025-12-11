#include <math.h>
#include <algorithm>

#include "AutoWah.h"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

AutoWah::AutoWah()
{
	Name = "AutoWah";
	Description = "Wah effect with auto open/close";

	fVslider0 = FAUSTFLOAT(0.5);

	NumParameters = AUTOWAH_NUMPARAMETERS;
	CreateParameters(NumParameters);
	Parameters[AUTOWAH_LEVEL].Name = "Level";
	Parameters[AUTOWAH_LEVEL].DefaultValue = fVslider0;
	Parameters[AUTOWAH_LEVEL].SourceVariable = &fVslider0;
	Parameters[AUTOWAH_LEVEL].Description = "Wah effect strength";
}

void AutoWah::init(int newSamplingFreq)
{
	StompBox::init(newSamplingFreq);

	instanceConstants(newSamplingFreq);

	instanceClear();
}

void AutoWah::instanceConstants(int newSamplingFreq)
{
	fSamplingFreq = newSamplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = exp((0.0 - (10.0 / fConst0)));
	fConst2 = (1.0 - fConst1);
	fConst3 = (1413.7166941154069 / fConst0);
	fConst4 = (2827.4333882308138 / fConst0);
}

void AutoWah::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fRec2[l0] = 0.0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		fRec1[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec3[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		fRec4[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 3); l4 = (l4 + 1)) {
		fRec0[l4] = 0.0;

	}
}

void AutoWah::compute(int count, float* input0, float* output0) {
	double fSlow0 = double(fVslider0);
	double fSlow1 = (1.0 - fSlow0);
	for (int i = 0; (i < count); i = (i + 1)) {
		double fTemp0 = double(input0[i]);
		double fTemp1 = fabs(fTemp0);
		fRec2[0] = std::max<double>(fTemp1, ((fConst1 * fRec2[1]) + (fConst2 * fTemp1)));
		fRec1[0] = ((0.0001000000000000001 * pow(4.0, fRec2[0])) + (0.999 * fRec1[1]));
		double fTemp2 = pow(2.0, (2.2999999999999998 * fRec2[0]));
		double fTemp3 = (1.0 - (fConst3 * (fTemp2 / pow(2.0, ((2.0 * (1.0 - fRec2[0])) + 1.0)))));
		fRec3[0] = ((0.999 * fRec3[1]) + (0.0010000000000000009 * mydsp_faustpower2_f(fTemp3)));
		fRec4[0] = ((0.999 * fRec4[1]) - (0.0020000000000000018 * (fTemp3 * cos((fConst4 * fTemp2)))));
		fRec0[0] = ((fTemp0 * fRec1[0]) - ((fRec0[2] * fRec3[0]) + (fRec4[0] * fRec0[1])));
		output0[i] = FAUSTFLOAT(((fSlow0 * (fRec0[0] - fRec0[1])) + (fSlow1 * fTemp0)));
		fRec2[1] = fRec2[0];
		fRec1[1] = fRec1[0];
		fRec3[1] = fRec3[0];
		fRec4[1] = fRec4[0];
		fRec0[2] = fRec0[1];
		fRec0[1] = fRec0[0];
	}
}
