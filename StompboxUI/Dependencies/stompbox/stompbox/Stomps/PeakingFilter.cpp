#include <math.h>
#include <algorithm>

#include "PeakingFilter.h"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

PeakingFilter::PeakingFilter(double freq, double q)
{
	Name = "Peaking";

	fVslider0 = 0;
		
	this->freq = freq;
	this->q = q;

	NumParameters = PEAKINGFILTER_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[PEAKINGFILTER_LEVEL].Name = "Level";
	Parameters[PEAKINGFILTER_LEVEL].MaxValue = -15;
	Parameters[PEAKINGFILTER_LEVEL].MaxValue = 15;
	Parameters[PEAKINGFILTER_LEVEL].SourceVariable = &fVslider0;
	Parameters[PEAKINGFILTER_LEVEL].DefaultValue = fVslider0;
}

void PeakingFilter::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void PeakingFilter::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (freq / fConst0);
	fConst2 = tan((3.1415926535897931 * fConst1));
	fConst3 = (2.0 * (1.0 - (1.0 / mydsp_faustpower2_f(fConst2))));
	fConst4 = (1.0 / fConst2);
	fConst5 = (3.1415926535897931 * (freq / ((q * fConst0) * sin((6.2831853071795862 * fConst1)))));
}

void PeakingFilter::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fRec1[l0] = 0.0;

	}
	for (int l1 = 0; (l1 < 3); l1 = (l1 + 1)) {
		fRec0[l1] = 0.0;

	}
}

void PeakingFilter::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = (0.0010000000000000009 * double(fVslider0));
	for (int i = 0; (i < count); i = (i + 1)) {
		double fTemp0 = (fConst3 * fRec0[1]);
		fRec1[0] = (fSlow0 + (0.999 * fRec1[1]));
		int iTemp1 = (fRec1[0] > 0.0);
		double fTemp2 = (fConst5 * pow(10.0, (0.050000000000000003 * fabs(fRec1[0]))));
		double fTemp3 = (iTemp1 ? fConst5 : fTemp2);
		double fTemp4 = ((fConst4 * (fConst4 + fTemp3)) + 1.0);
		fRec0[0] = (double(input0[i]) - ((fTemp0 + (fRec0[2] * ((fConst4 * (fConst4 - fTemp3)) + 1.0))) / fTemp4));
		double fTemp5 = (iTemp1 ? fTemp2 : fConst5);
		output0[i] = FAUSTFLOAT((((fRec0[2] * ((fConst4 * (fConst4 - fTemp5)) + 1.0)) + (fTemp0 + (fRec0[0] * ((fConst4 * (fConst4 + fTemp5)) + 1.0)))) / fTemp4));
		fRec1[1] = fRec1[0];
		fRec0[2] = fRec0[1];
		fRec0[1] = fRec0[0];
	}
}

