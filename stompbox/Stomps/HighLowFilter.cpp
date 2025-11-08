#include <math.h>
#include <algorithm>

#include "HighLowFilter.h"

static double mydsp_faustpower2_f(double value) {
	return value * value;
}


HighLowFilter::HighLowFilter()
{
	Name = "HighPass";
	Description = "Dual high/low pass filter";

	NumParameters = HIGHLOWFILTER_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].Name = "Low";
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].MinValue = 1;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].MaxValue = 500;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].SourceVariable = &lowFreq;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].DefaultValue = lowFreq;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].RangePower = 3;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].DisplayFormat = "{0:0}hz";
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].Description = "Low cut (high pass) frequency";


	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].Name = "High";
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].MinValue = 1000;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].MaxValue = 20000;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].SourceVariable = &highFreq;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].DefaultValue = highFreq;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].RangePower = 2;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].DisplayFormat = "{0:0}hz";
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].Description = "High cut (low pass) frequency";
}

void HighLowFilter::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void HighLowFilter::instanceConstants(int samplingFreq)
{
	fSampleRate = samplingFreq;
	fConst0 = std::min<double>(1.92e+05, std::max<double>(1.0, double(fSampleRate)));
	fConst1 = 44.1 / fConst0;
	fConst2 = 1.0 - fConst1;
	fConst3 = 3.141592653589793 / fConst0;
}

void HighLowFilter::instanceClear() {
	for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
		fRec0[l0] = 0.0;
	}
	for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
		fRec2[l1] = 0.0;
	}
	for (int l2 = 0; l2 < 3; l2 = l2 + 1) {
		fRec3[l2] = 0.0;
	}
	for (int l3 = 0; l3 < 3; l3 = l3 + 1) {
		fRec1[l3] = 0.0;
	}
}

void HighLowFilter::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = fConst1 * highFreq;
	double fSlow1 = fConst1 * lowFreq;
	for (int i0 = 0; i0 < count; i0 = i0 + 1) {
		fRec0[0] = fSlow0 + fConst2 * fRec0[1];
		double fTemp0 = std::tan(fConst3 * fRec0[0]);
		double fTemp1 = 1.0 / fTemp0;
		double fTemp2 = (fTemp1 + 1.414213562373095) / fTemp0 + 1.0;
		fRec2[0] = fSlow1 + fConst2 * fRec2[1];
		double fTemp3 = std::tan(fConst3 * fRec2[0]);
		double fTemp4 = 1.0 / fTemp3;
		double fTemp5 = (fTemp4 + 1.414213562373095) / fTemp3 + 1.0;
		double fTemp6 = mydsp_faustpower2_f(fTemp3);
		fRec3[0] = double(input0[i0]) - (fRec3[2] * ((fTemp4 + -1.414213562373095) / fTemp3 + 1.0) + 2.0 * fRec3[1] * (1.0 - 1.0 / fTemp6)) / fTemp5;
		fRec1[0] = (fRec3[2] + (fRec3[0] - 2.0 * fRec3[1])) / (fTemp6 * fTemp5) - (fRec1[2] * ((fTemp1 + -1.414213562373095) / fTemp0 + 1.0) + 2.0 * fRec1[1] * (1.0 - 1.0 / mydsp_faustpower2_f(fTemp0))) / fTemp2;
		output0[i0] = FAUSTFLOAT((fRec1[2] + fRec1[0] + 2.0 * fRec1[1]) / fTemp2);
		fRec0[1] = fRec0[0];
		fRec2[1] = fRec2[0];
		fRec3[2] = fRec3[1];
		fRec3[1] = fRec3[0];
		fRec1[2] = fRec1[1];
		fRec1[1] = fRec1[0];
	}
}

