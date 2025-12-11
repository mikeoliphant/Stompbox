#include <math.h>
#include <algorithm>

#include "Wah.h"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

Wah::Wah()
{
	Name = "Wah";
	Description = "Wah designed for use with a foot pedal";

	fVslider0 = FAUSTFLOAT(0.5);

	NumParameters = WAH_NUMPARAMETERS;
	CreateParameters(NumParameters);
	Parameters[WAH_LEVEL].Name = "Wah";
	Parameters[WAH_LEVEL].DefaultValue = fVslider0;
	Parameters[WAH_LEVEL].SourceVariable = &fVslider0;
	Parameters[WAH_LEVEL].Description = "Effect strength";
}

void Wah::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void Wah::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (2827.4333882308138 / fConst0);
	fConst2 = (1413.7166941154069 / fConst0);
}

void Wah::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fRec1[l0] = 0.0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		fRec2[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec3[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 3); l3 = (l3 + 1)) {
		fRec0[l3] = 0.0;

	}
}

void Wah::compute(int count, float* input0, float* output0) {
	double fSlow0 = double(fVslider0);
	double fSlow1 = (0.0001000000000000001 * pow(4.0, fSlow0));
	double fSlow2 = pow(2.0, (2.2999999999999998 * fSlow0));
	double fSlow3 = (1.0 - (fConst2 * (fSlow2 / pow(2.0, ((2.0 * (1.0 - fSlow0)) + 1.0)))));
	double fSlow4 = (0.0020000000000000018 * (cos((fConst1 * fSlow2)) * fSlow3));
	double fSlow5 = (0.0010000000000000009 * mydsp_faustpower2_f(fSlow3));
	for (int i = 0; (i < count); i = (i + 1)) {
		fRec1[0] = (fSlow1 + (0.999 * fRec1[1]));
		fRec2[0] = ((0.999 * fRec2[1]) - fSlow4);
		fRec3[0] = (fSlow5 + (0.999 * fRec3[1]));
		fRec0[0] = ((double(input0[i]) * fRec1[0]) - ((fRec2[0] * fRec0[1]) + (fRec3[0] * fRec0[2])));
		output0[i] = FAUSTFLOAT((fRec0[0] - fRec0[1]));
		fRec1[1] = fRec1[0];
		fRec2[1] = fRec2[0];
		fRec3[1] = fRec3[0];
		fRec0[2] = fRec0[1];
		fRec0[1] = fRec0[0];
	}
}
