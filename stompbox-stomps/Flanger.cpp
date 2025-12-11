#include <math.h>
#include <algorithm>

#include "Flanger.h"

Flanger::Flanger()
{
	Name = "Flanger";
	Description = "Flanging effect";

	fHslider0 = FAUSTFLOAT(1.0);
	fHslider1 = FAUSTFLOAT(0.5);
	fHslider2 = FAUSTFLOAT(0.20000000000000001);

	NumParameters = FLANGER_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[FLANGER_RATE].Name = "Rate";
	Parameters[FLANGER_RATE].MaxValue = 5;
	Parameters[FLANGER_RATE].SourceVariable = &fHslider2;
	Parameters[FLANGER_RATE].DefaultValue = fHslider2;
	Parameters[FLANGER_RATE].Description = "Rate of frequency modulation";

	Parameters[FLANGER_DEPTH].Name = "Depth";
	Parameters[FLANGER_DEPTH].MinValue = 0;
	Parameters[FLANGER_DEPTH].MaxValue = 1;
	Parameters[FLANGER_DEPTH].SourceVariable = &fHslider0;
	Parameters[FLANGER_DEPTH].DefaultValue = fHslider0;
	Parameters[FLANGER_DEPTH].Description = "Strength of frequency modulation";

	Parameters[FLANGER_FEEDBACK].Name = "FBack";
	Parameters[FLANGER_FEEDBACK].MinValue = 0;
	Parameters[FLANGER_FEEDBACK].MaxValue = 1;
	Parameters[FLANGER_FEEDBACK].SourceVariable = &fHslider1;
	Parameters[FLANGER_FEEDBACK].DefaultValue = fHslider1;
	Parameters[FLANGER_FEEDBACK].Description = "System feedback level";
}

void Flanger::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void Flanger::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (6.2831853071795862 / fConst0);
}

void Flanger::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		iVec0[l0] = 0;

	}
	IOTA = 0;
	for (int l1 = 0; (l1 < 4096); l1 = (l1 + 1)) {
		fVec1[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec1[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		fRec2[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		fRec0[l4] = 0.0;

	}
}

void Flanger::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = double(fHslider0);
	double fSlow1 = double(fHslider1);
	double fSlow2 = (fConst1 * double(fHslider2));
	double fSlow3 = cos(fSlow2);
	double fSlow4 = sin(fSlow2);
	for (int i = 0; (i < count); i = (i + 1)) {
		double fTemp0 = double(input0[i]);
		iVec0[0] = 1;
		double fTemp1 = ((fSlow1 * fRec0[1]) - fTemp0);
		fVec1[(IOTA & 4095)] = fTemp1;
		fRec1[0] = ((fSlow3 * fRec1[1]) + (fSlow4 * fRec2[1]));
		fRec2[0] = ((double((1 - iVec0[1])) + (fSlow3 * fRec2[1])) - (fSlow4 * fRec1[1]));
		double fTemp2 = (fConst0 * ((0.0050000000000000001 * (fRec1[0] + 1.0)) + 0.001));
		int iTemp3 = int(fTemp2);
		double fTemp4 = floor(fTemp2);
		fRec0[0] = ((fVec1[((IOTA - std::min<int>(2049, std::max<int>(0, iTemp3))) & 4095)] * (fTemp4 + (1.0 - fTemp2))) + (fVec1[((IOTA - std::min<int>(2049, std::max<int>(0, (iTemp3 + 1)))) & 4095)] * (fTemp2 - fTemp4)));
		output0[i] = FAUSTFLOAT((0.5 * (fTemp0 - (fSlow0 * fRec0[0]))));
		iVec0[1] = iVec0[0];
		IOTA = (IOTA + 1);
		fRec1[1] = fRec1[0];
		fRec2[1] = fRec2[0];
		fRec0[1] = fRec0[0];

	}
}
