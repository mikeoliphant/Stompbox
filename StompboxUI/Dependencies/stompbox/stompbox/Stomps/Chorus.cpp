#include <math.h>
#include <algorithm>

#include "Chorus.h"

namespace chorus
{
	Chorus::Chorus()
	{
		Name = "Chorus";
		Description = "Chorus effect";

		fHslider0 = FAUSTFLOAT(1.0);
		fHslider1 = FAUSTFLOAT(0.1);
		fHslider2 = FAUSTFLOAT(2.5);
		fVslider0 = FAUSTFLOAT(100.0);

		NumParameters = CHORUS_NUMPARAMETERS;
		CreateParameters(NumParameters);

		//Parameters[CHORUS_LEVEL].Name = "Level";
		//Parameters[CHORUS_LEVEL].SourceVariable = &fHslider0;
		//Parameters[CHORUS_LEVEL].DefaultValue = fHslider0;

		Parameters[CHORUS_RATE].Name = "Rate";
		Parameters[CHORUS_RATE].MaxValue = 5;
		Parameters[CHORUS_RATE].SourceVariable = &fHslider2;
		Parameters[CHORUS_RATE].DefaultValue = fHslider2;
		Parameters[CHORUS_RATE].Description = "Speed of the chorus effect";

		Parameters[CHORUS_DEPTH].Name = "Depth";
		Parameters[CHORUS_DEPTH].MaxValue = 0.2;
		Parameters[CHORUS_DEPTH].SourceVariable = &fHslider1;
		Parameters[CHORUS_DEPTH].DefaultValue = fHslider1;
		Parameters[CHORUS_DEPTH].Description = "How pronounced the effect is";
	}

	void Chorus::init(int newSamplingFreq)
	{
		StompBox::init(newSamplingFreq);

		mydspSIG0* sig0 = newmydspSIG0();
		sig0->instanceInitmydspSIG0(newSamplingFreq);
		sig0->fillmydspSIG0(65536, ftbl0mydspSIG0);
		deletemydspSIG0(sig0);

		fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
		fConst1 = (0.5 * fConst0);
		fConst2 = (1.0 / fConst0);
		fConst3 = (1000.0 / fConst0);
		fConst4 = (0.0 - fConst3);

		instanceConstants(newSamplingFreq);

		instanceClear();
	}

	void Chorus::instanceConstants(int newSamplingFreq)
	{
		fSamplingFreq = newSamplingFreq;

		fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
		fConst1 = (0.01 * fConst0);
		fConst2 = (1.0 / fConst0);
		fConst3 = (1000.0 / fConst0);
		fConst4 = (0.0 - fConst3);
	}

	void Chorus::instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0;

		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec5[l1] = 0.0;

		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec1[l3] = 0.0;

		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec2[l4] = 0.0;

		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec3[l5] = 0.0;

		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec4[l6] = 0.0;

		}
		IOTA = 0;
		for (int l7 = 0; (l7 < 131072); l7 = (l7 + 1)) {
			fVec0[l7] = 0.0;

		}
	}

	void Chorus::compute(int count, float* input0, float* output0) {
		double fSlow0 = (0.0070000000000000062 * double(fHslider0));
		double fSlow1 = double(fHslider1);
		double fSlow2 = (fConst2 * double(fHslider2));
		double fSlow3 = (0.01 * double(fVslider0));
		double fSlow4 = (fSlow3 + (1.0 - fSlow3));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec0[0] = (fSlow0 + (0.99299999999999999 * fRec0[1]));
			fRec5[0] = (fSlow2 + (fRec5[1] - floor((fSlow2 + fRec5[1]))));
			double fTemp0 = (65536.0 * (fRec5[0] - floor(fRec5[0])));
			double fTemp1 = floor(fTemp0);
			int iTemp2 = int(fTemp1);
			double fTemp3 = (fConst1 * ((fSlow1 * (((fTemp0 - fTemp1) * ftbl0mydspSIG0[((iTemp2 + 1) & 65535)]) + ((fTemp1 + (1.0 - fTemp0)) * ftbl0mydspSIG0[(iTemp2 & 65535)]))) + 1.0));
			double fTemp4 = ((fRec1[1] != 0.0) ? (((fRec2[1] > 0.0) & (fRec2[1] < 1.0)) ? fRec1[1] : 0.0) : (((fRec2[1] == 0.0) & (fTemp3 != fRec3[1])) ? fConst3 : (((fRec2[1] == 1.0) & (fTemp3 != fRec4[1])) ? fConst4 : 0.0)));
			fRec1[0] = fTemp4;
			fRec2[0] = std::max<double>(0.0, std::min<double>(1.0, (fRec2[1] + fTemp4)));
			fRec3[0] = (((fRec2[1] >= 1.0) & (fRec4[1] != fTemp3)) ? fTemp3 : fRec3[1]);
			fRec4[0] = (((fRec2[1] <= 0.0) & (fRec3[1] != fTemp3)) ? fTemp3 : fRec4[1]);
			double fTemp5 = double(input0[i]);
			double fTemp6 = (fSlow3 * fTemp5);
			fVec0[(IOTA & 131071)] = fTemp6;
			output0[i] = FAUSTFLOAT(((fRec0[0] * ((fRec2[0] * fVec0[((IOTA - int(std::min<double>(65536.0, std::max<double>(0.0, fRec4[0])))) & 131071)]) + ((1.0 - fRec2[0]) * fVec0[((IOTA - int(std::min<double>(65536.0, std::max<double>(0.0, fRec3[0])))) & 131071)]))) + (fSlow4 * fTemp5)));
			fRec0[1] = fRec0[0];
			fRec5[1] = fRec5[0];
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			IOTA = (IOTA + 1);

		}
	}
}