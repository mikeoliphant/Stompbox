#include <math.h>
#include <algorithm>

#include "NoiseGate.h"

NoiseGate::NoiseGate()
{
	Name = "NoiseGate";
	Description = "Simple noise gate";

	threshold = -60;
	attackMS = 5;
	holdMS = 10;
	releaseMS = 50;

	NumParameters = NOISEGATE_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[NOISEGATE_THRESHOLD].Name = "Thresh";
	Parameters[NOISEGATE_THRESHOLD].MinValue = -100;
	Parameters[NOISEGATE_THRESHOLD].MaxValue = 0;
	Parameters[NOISEGATE_THRESHOLD].SourceVariable = &threshold;
	Parameters[NOISEGATE_THRESHOLD].DefaultValue = threshold;
	Parameters[NOISEGATE_THRESHOLD].CanSyncToHostBPM = true;
	Parameters[NOISEGATE_THRESHOLD].DisplayFormat = "{0:0}db";
	Parameters[NOISEGATE_THRESHOLD].Description = "Signal level that gate closes below";

	Parameters[NOISEGATE_ATTACK].Name = "Attack";
	Parameters[NOISEGATE_ATTACK].MinValue = 1;
	Parameters[NOISEGATE_ATTACK].MaxValue = 100;
	Parameters[NOISEGATE_ATTACK].SourceVariable = &attackMS;
	Parameters[NOISEGATE_ATTACK].DefaultValue = attackMS;
	Parameters[NOISEGATE_ATTACK].DisplayFormat = "{0:0}ms";
	Parameters[NOISEGATE_ATTACK].IsAdvanced = true;
	Parameters[NOISEGATE_ATTACK].Description = "Time (ms) for gate to fully open";

	Parameters[NOISEGATE_HOLD].Name = "Hold";
	Parameters[NOISEGATE_HOLD].MinValue = 1;
	Parameters[NOISEGATE_HOLD].MaxValue = 100;
	Parameters[NOISEGATE_HOLD].SourceVariable = &holdMS;
	Parameters[NOISEGATE_HOLD].DefaultValue = holdMS;
	Parameters[NOISEGATE_HOLD].DisplayFormat = "{0:0}ms";
	Parameters[NOISEGATE_HOLD].IsAdvanced = true;
	Parameters[NOISEGATE_HOLD].Description = "Minimum time (ms) gate stays open";

	Parameters[NOISEGATE_RELEASE].Name = "Release";
	Parameters[NOISEGATE_RELEASE].MinValue = 1;
	Parameters[NOISEGATE_RELEASE].MaxValue = 100;
	Parameters[NOISEGATE_RELEASE].SourceVariable = &releaseMS;
	Parameters[NOISEGATE_RELEASE].DefaultValue = releaseMS;
	Parameters[NOISEGATE_RELEASE].DisplayFormat = "{0:0}ms";
	Parameters[NOISEGATE_RELEASE].IsAdvanced = true;
	Parameters[NOISEGATE_RELEASE].Description = "Time (ms) for gate to fully close";
}

void NoiseGate::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void NoiseGate::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(1.92e+05, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = 1.0 / fConst0;
	fConst2 = 0.001 * fConst0;
}

void NoiseGate::instanceClear() {
	for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
		fRec1[l0] = 0.0;
	}
	for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
		iVec0[l1] = 0;
	}
	for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
		iRec2[l2] = 0;
	}
	for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
		fRec0[l3] = 0.0;
	}
}

void NoiseGate::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = std::pow(1e+01, 0.05 * threshold);
	double fSlow1 = 0.001 * attackMS;
	double fSlow2 = 0.001 * releaseMS;
	double fSlow3 = std::min<double>(fSlow1, fSlow2);
	int iSlow4 = std::fabs(fSlow3) < 2.220446049250313e-16;
	double fSlow5 = ((iSlow4) ? 0.0 : std::exp(-(fConst1 / ((iSlow4) ? 1.0 : fSlow3))));
	double fSlow6 = 1.0 - fSlow5;
	int iSlow7 = int(fConst2 * holdMS);
	int iSlow8 = std::fabs(fSlow2) < 2.220446049250313e-16;
	double fSlow9 = ((iSlow8) ? 0.0 : std::exp(-(fConst1 / ((iSlow8) ? 1.0 : fSlow2))));
	int iSlow10 = std::fabs(fSlow1) < 2.220446049250313e-16;
	double fSlow11 = ((iSlow10) ? 0.0 : std::exp(-(fConst1 / ((iSlow10) ? 1.0 : fSlow1))));
	for (int i0 = 0; i0 < count; i0 = i0 + 1) {
		double fTemp0 = double(input0[i0]);
		fRec1[0] = std::fabs(fTemp0) * fSlow6 + fRec1[1] * fSlow5;
		int iTemp1 = fRec1[0] > fSlow0;
		iVec0[0] = iTemp1;
		iRec2[0] = std::max<int>(iSlow7 * (iTemp1 < iVec0[1]), iRec2[1] + -1);
		double fTemp2 = std::fabs(std::max<double>(double(iTemp1), double(iRec2[0] > 0)));
		double fTemp3 = ((fTemp2 > fRec0[1]) ? fSlow11 : fSlow9);
		fRec0[0] = fTemp2 * (1.0 - fTemp3) + fRec0[1] * fTemp3;
		output0[i0] = FAUSTFLOAT(fTemp0 * fRec0[0]);
		fRec1[1] = fRec1[0];
		iVec0[1] = iVec0[0];
		iRec2[1] = iRec2[0];
		fRec0[1] = fRec0[0];
	}
}
