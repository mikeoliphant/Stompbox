#include <math.h>
#include <algorithm>

#include "Crossover.h"

Crossover::Crossover()
{
}

void Crossover::init(int samplingFreq)
{
	instanceConstants(samplingFreq);

	instanceClear();
}

void Crossover::instanceConstants(int samplingFreq)
{
    fSampleRate = samplingFreq;
	fConst0 = 3.141592653589793 / std::min<double>(1.92e+05, std::max<double>(1.0, double(fSampleRate)));
}

void Crossover::instanceClear() {
	for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
		fRec3[l0] = 0.0;
	}
	for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
		fRec4[l1] = 0.0;
	}
	for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
		fRec0[l2] = 0.0;
	}
	for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
		fRec1[l3] = 0.0;
	}
	for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
		fRec7[l4] = 0.0;
	}
	for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
		fRec8[l5] = 0.0;
	}
}

void Crossover::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0, FAUSTFLOAT* output1)
{
	double fSlow0 = std::tan(fConst0 * crossoverFreq);
	double fSlow1 = fSlow0 * (fSlow0 + 1.4142135623730951) + 1.0;
	double fSlow2 = 2.0 / fSlow1;
	double fSlow3 = fSlow0 / fSlow1;
	double fSlow4 = 1.0 / fSlow1;

	for (int i0 = 0; i0 < count; i0 = i0 + 1)
	{
		double fTemp0 = double(input0[i0]);
		double fTemp1 = fRec3[1] + fSlow0 * (fTemp0 - fRec4[1]);
		fRec3[0] = fSlow2 * fTemp1 - fRec3[1];
		double fTemp2 = fRec4[1] + fSlow3 * fTemp1;
		fRec4[0] = 2.0 * fTemp2 - fRec4[1];
		double fRec5 = fSlow4 * fTemp1;
		double fRec6 = fTemp2;
		double fTemp3 = fRec0[1] + fSlow0 * (fRec6 - fRec1[1]);
		fRec0[0] = fSlow2 * fTemp3 - fRec0[1];
		double fTemp4 = fRec1[1] + fSlow3 * fTemp3;
		fRec1[0] = 2.0 * fTemp4 - fRec1[1];
		double fRec2 = fTemp4;
		output0[i0] = FAUSTFLOAT(fRec2);
		double fTemp5 = fRec6 + 1.4142135623730951 * fRec5;
		double fTemp6 = fRec7[1] + fSlow0 * (fTemp0 - (fTemp5 + fRec8[1]));
		fRec7[0] = fSlow2 * fTemp6 - fRec7[1];
		double fTemp7 = fRec8[1] + fSlow3 * fTemp6;
		fRec8[0] = 2.0 * fTemp7 - fRec8[1];
		double fRec9 = fSlow4 * fTemp6;
		double fRec10 = fTemp7;
		output1[i0] = FAUSTFLOAT(fTemp0 - (fRec10 + fRec6 + 1.4142135623730951 * (fRec5 + fRec9)));
		fRec3[1] = fRec3[0];
		fRec4[1] = fRec4[0];
		fRec0[1] = fRec0[0];
		fRec1[1] = fRec1[0];
		fRec7[1] = fRec7[0];
		fRec8[1] = fRec8[0];
	}
}