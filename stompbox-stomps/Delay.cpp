#include <math.h>
#include <algorithm>

#include "Delay.h"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

Delay::Delay()
{
	Name = "Delay";
	Description = "Signal delay effect";

	delay = 250;
	level = 1;
	feedback = 0.5;
	wet = 0.5;
	warmth = 0.5;
	hilo = 0.5;

	NumParameters = DELAY_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[DELAY_DELAY].Name = "Delay";
	Parameters[DELAY_DELAY].MinValue = 1;
	Parameters[DELAY_DELAY].MaxValue = 1000;
	Parameters[DELAY_DELAY].SourceVariable = &delay;
	Parameters[DELAY_DELAY].DefaultValue = delay;
	Parameters[DELAY_DELAY].CanSyncToHostBPM = true;
	Parameters[DELAY_DELAY].DisplayFormat = "{0:0}ms";
	Parameters[DELAY_DELAY].Description = "Delay time";

	Parameters[DELAY_LEVEL].Name = "Mix";
	Parameters[DELAY_LEVEL].MinValue = 0;
	Parameters[DELAY_LEVEL].MaxValue = 1.2f;
	Parameters[DELAY_LEVEL].SourceVariable = &wet;
	Parameters[DELAY_LEVEL].DefaultValue = wet;
	Parameters[DELAY_LEVEL].Description = "Delay volume";

	Parameters[DELAY_FEEDBACK].Name = "FBack";
	Parameters[DELAY_FEEDBACK].MinValue = 0;
	Parameters[DELAY_FEEDBACK].MaxValue = 1;
	Parameters[DELAY_FEEDBACK].SourceVariable = &feedback;
	Parameters[DELAY_FEEDBACK].DefaultValue = feedback;
	Parameters[DELAY_FEEDBACK].Description = "Amount of delay decay";

	Parameters[DELAY_WARMTH].Name = "Warm";
	Parameters[DELAY_WARMTH].MinValue = 0;
	Parameters[DELAY_WARMTH].MaxValue = 1;
	Parameters[DELAY_WARMTH].SourceVariable = &warmth;
	Parameters[DELAY_WARMTH].DefaultValue = warmth;
	Parameters[DELAY_WARMTH].IsAdvanced = true;
	Parameters[DELAY_WARMTH].Description = "Amount of frequency filtering in the decay";

	Parameters[DELAY_LOWCUT].Name = "HiLo";
	Parameters[DELAY_LOWCUT].SourceVariable = &hilo;
	Parameters[DELAY_LOWCUT].DefaultValue = hilo;
	Parameters[DELAY_LOWCUT].IsAdvanced = true;
	Parameters[DELAY_LOWCUT].Description = "Bias of filtering toward low/high frequencies";
}

void Delay::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void Delay::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (3.1415926535897931 / fConst0);
	fConst2 = (0.001 * fConst0);
	fConst3 = (10.0 / fConst0);
	fConst4 = (0.0 - fConst3);
}

void Delay::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fRec3[l0] = 0.0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		fRec4[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec5[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		fRec6[l3] = 0.0;

	}
	IOTA = 0;
	for (int l4 = 0; (l4 < 262144); l4 = (l4 + 1)) {
		fVec0[l4] = 0.0;

	}
	for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) {
		fRec2[l5] = 0.0;

	}
	for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) {
		fRec1[l6] = 0.0;

	}
	for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
		fRec0[l7] = 0.0;

	}
}

void Delay::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	hipass = 20 + (500 * pow(warmth, 2) * hilo);
	lowpass = 10500 - (10000 * pow(warmth, 2) * (1 - hilo));

	double fSlow0 = tan((fConst1 * lowpass));
	double fSlow1 = (1.0 / fSlow0);
	double fSlow2 = (1.0 / (((fSlow1 + 1.4142135623730949) / fSlow0) + 1.0));
	double fSlow3 = tan((fConst1 * hipass));
	double fSlow4 = (1.0 / fSlow3);
	double fSlow5 = (1.0 / (((fSlow4 + 1.4142135623730949) / fSlow3) + 1.0));
	double fSlow6 = mydsp_faustpower2_f(fSlow3);
	double fSlow7 = (1.0 / fSlow6);
	double fSlow8 = (fConst2 * delay);
	double fSlow9 = (((fSlow4 + -1.4142135623730949) / fSlow3) + 1.0);
	double fSlow10 = (2.0 * (1.0 - fSlow7));
	double fSlow11 = (0.0 - (2.0 / fSlow6));
	double fSlow12 = (2.0 * (1.0 - (1.0 / mydsp_faustpower2_f(fSlow0))));
	double fSlow13 = (((fSlow1 + -1.4142135623730949) / fSlow0) + 1.0);
	for (int i = 0; (i < count); i = (i + 1)) {
		double fTemp0 = double(input0[i]);
		double fTemp1 = ((fRec3[1] != 0.0) ? (((fRec4[1] > 0.0) & (fRec4[1] < 1.0)) ? fRec3[1] : 0.0) : (((fRec4[1] == 0.0) & (fSlow8 != fRec5[1])) ? fConst3 : (((fRec4[1] == 1.0) & (fSlow8 != fRec6[1])) ? fConst4 : 0.0)));
		fRec3[0] = fTemp1;
		fRec4[0] = std::max<double>(0.0, std::min<double>(1.0, (fRec4[1] + fTemp1)));
		fRec5[0] = (((fRec4[1] >= 1.0) & (fRec6[1] != fSlow8)) ? fSlow8 : fRec5[1]);
		fRec6[0] = (((fRec4[1] <= 0.0) & (fRec5[1] != fSlow8)) ? fSlow8 : fRec6[1]);
		double fTemp2 = (fTemp0 + (feedback * fRec0[1]));
		fVec0[(IOTA & 262143)] = fTemp2;
		fRec2[0] = ((level * ((fRec4[0] * fVec0[((IOTA - int(std::min<double>(192000.0, std::max<double>(0.0, fRec6[0])))) & 262143)]) + ((1.0 - fRec4[0]) * fVec0[((IOTA - int(std::min<double>(192000.0, std::max<double>(0.0, fRec5[0])))) & 262143)]))) - (fSlow5 * ((fSlow9 * fRec2[2]) + (fSlow10 * fRec2[1]))));
		fRec1[0] = ((fSlow5 * (((fSlow7 * fRec2[0]) + (fSlow11 * fRec2[1])) + (fSlow7 * fRec2[2]))) - (fSlow2 * ((fSlow12 * fRec1[1]) + (fSlow13 * fRec1[2]))));
		fRec0[0] = (fSlow2 * (fRec1[2] + (fRec1[0] + (2.0 * fRec1[1]))));
		output0[i] = FAUSTFLOAT((fTemp0 + (wet * fRec0[0])));
		fRec3[1] = fRec3[0];
		fRec4[1] = fRec4[0];
		fRec5[1] = fRec5[0];
		fRec6[1] = fRec6[0];
		IOTA = (IOTA + 1);
		fRec2[2] = fRec2[1];
		fRec2[1] = fRec2[0];
		fRec1[2] = fRec1[1];
		fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];

	}
}
