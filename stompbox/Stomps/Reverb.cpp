#include <math.h>
#include <algorithm>

#include "Reverb.h"

FreeVerb::FreeVerb()
{
	Name = "Reverb";
	Description = "Simple room reverb";

	fVslider0 = FAUSTFLOAT(50.0);
	fVslider1 = FAUSTFLOAT(0.5);
	fVslider2 = FAUSTFLOAT(5);

	NumParameters = REVERB_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[REVERB_ROOMSIZE].Name = "Size";
	Parameters[REVERB_ROOMSIZE].SourceVariable = &fVslider1;
	Parameters[REVERB_ROOMSIZE].DisplayFormat = "{0:0.00}";
	Parameters[REVERB_ROOMSIZE].Description = "Room size";

	Parameters[REVERB_DECAY].Name = "Decay";
	Parameters[REVERB_DECAY].SourceVariable = &fVslider2;
	Parameters[REVERB_DECAY].MinValue = 0;
	Parameters[REVERB_DECAY].MaxValue = 10;
	Parameters[REVERB_DECAY].DefaultValue = fVslider2;
	Parameters[REVERB_DECAY].DisplayFormat = "{0:0.0}";
	Parameters[REVERB_DECAY].Description = "Length of reverb decay";

	Parameters[REVERB_WETDRY].Name = "Blend";
	Parameters[REVERB_WETDRY].SourceVariable = &fVslider0;
	Parameters[REVERB_WETDRY].DefaultValue = fVslider0;
	Parameters[REVERB_WETDRY].MaxValue = 100;
	Parameters[REVERB_WETDRY].Step = 1;
	Parameters[REVERB_WETDRY].DisplayFormat = "{0:0}";
	Parameters[REVERB_WETDRY].Description = "Dry/wet effect blend";
}

void FreeVerb::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);
	instanceClear();
}

void FreeVerb::instanceConstants(int sample_rate)
{
	fSampleRate = sample_rate;
	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSampleRate)));
	iConst1 = int(((0.036666666666666667 * fConst0) + 23.0));
	iConst2 = int(((0.035306122448979592 * fConst0) + 23.0));
	iConst3 = int(((0.03380952380952381 * fConst0) + 23.0));
	iConst4 = int(((0.030748299319727893 * fConst0) + 23.0));
	iConst5 = int(((0.025306122448979593 * fConst0) + 23.0));
	iConst6 = int(((0.026938775510204082 * fConst0) + 23.0));
	iConst7 = int(((0.028956916099773244 * fConst0) + 23.0));
	iConst8 = int(((0.032244897959183672 * fConst0) + 23.0));
	iConst9 = int(((0.012607709750566893 * fConst0) + 23.0));
	iConst10 = int(((0.01 * fConst0) + 23.0));
	iConst11 = int(((0.0077324263038548759 * fConst0) + 23.0));
	iConst12 = int(((0.0051020408163265311 * fConst0) + 23.0));
}

void FreeVerb::instanceClear()
{
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fRec9[l0] = 0.0;

	}
	IOTA = 0;
	for (int l1 = 0; (l1 < 8192); l1 = (l1 + 1)) {
		fVec0[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec8[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		fRec11[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 8192); l4 = (l4 + 1)) {
		fVec1[l4] = 0.0;

	}
	for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
		fRec10[l5] = 0.0;

	}
	for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
		fRec13[l6] = 0.0;

	}
	for (int l7 = 0; (l7 < 8192); l7 = (l7 + 1)) {
		fVec2[l7] = 0.0;

	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		fRec12[l8] = 0.0;

	}
	for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
		fRec15[l9] = 0.0;

	}
	for (int l10 = 0; (l10 < 8192); l10 = (l10 + 1)) {
		fVec3[l10] = 0.0;

	}
	for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
		fRec14[l11] = 0.0;

	}
	for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
		fRec17[l12] = 0.0;

	}
	for (int l13 = 0; (l13 < 8192); l13 = (l13 + 1)) {
		fVec4[l13] = 0.0;

	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		fRec16[l14] = 0.0;

	}
	for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
		fRec19[l15] = 0.0;

	}
	for (int l16 = 0; (l16 < 8192); l16 = (l16 + 1)) {
		fVec5[l16] = 0.0;

	}
	for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
		fRec18[l17] = 0.0;

	}
	for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
		fRec21[l18] = 0.0;

	}
	for (int l19 = 0; (l19 < 8192); l19 = (l19 + 1)) {
		fVec6[l19] = 0.0;

	}
	for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
		fRec20[l20] = 0.0;

	}
	for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
		fRec23[l21] = 0.0;

	}
	for (int l22 = 0; (l22 < 8192); l22 = (l22 + 1)) {
		fVec7[l22] = 0.0;

	}
	for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
		fRec22[l23] = 0.0;

	}
	for (int l24 = 0; (l24 < 4096); l24 = (l24 + 1)) {
		fVec8[l24] = 0.0;

	}
	for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
		fRec6[l25] = 0.0;

	}
	for (int l26 = 0; (l26 < 2048); l26 = (l26 + 1)) {
		fVec9[l26] = 0.0;

	}
	for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
		fRec4[l27] = 0.0;

	}
	for (int l28 = 0; (l28 < 2048); l28 = (l28 + 1)) {
		fVec10[l28] = 0.0;

	}
	for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
		fRec2[l29] = 0.0;

	}
	for (int l30 = 0; (l30 < 1024); l30 = (l30 + 1)) {
		fVec11[l30] = 0.0;

	}
	for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
		fRec0[l31] = 0.0;

	}
}

void FreeVerb::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = double(fVslider0);
	double fSlow1 = (0.00014999999999999999 * fSlow0);
	double fSlow2 = ((0.28000000000000003 * double(fVslider1)) + 0.69999999999999996);
	double fSlow3 = (0.10000000000000001 * double(fVslider2));
	double fSlow4 = (1.0 - fSlow3);
	double fSlow5 = (1.0 - (0.01 * fSlow0));
	double fSlow6 = (fSlow5 + (fSlow0 * ((0.01 * fSlow5) + 0.00014999999999999999)));
	for (int i = 0; (i < count); i = (i + 1)) {
		double fTemp0 = double(input0[i]);
		double fTemp1 = (fSlow1 * fTemp0);
		fRec9[0] = ((fSlow4 * fRec9[1]) + (fSlow3 * fRec8[1]));
		fVec0[(IOTA & 8191)] = (fTemp1 + (fSlow2 * fRec9[0]));
		fRec8[0] = fVec0[((IOTA - iConst1) & 8191)];
		fRec11[0] = ((fSlow3 * fRec10[1]) + (fSlow4 * fRec11[1]));
		fVec1[(IOTA & 8191)] = (fTemp1 + (fSlow2 * fRec11[0]));
		fRec10[0] = fVec1[((IOTA - iConst2) & 8191)];
		fRec13[0] = ((fSlow3 * fRec12[1]) + (fSlow4 * fRec13[1]));
		fVec2[(IOTA & 8191)] = (fTemp1 + (fSlow2 * fRec13[0]));
		fRec12[0] = fVec2[((IOTA - iConst3) & 8191)];
		fRec15[0] = ((fSlow4 * fRec15[1]) + (fSlow3 * fRec14[1]));
		fVec3[(IOTA & 8191)] = (fTemp1 + (fSlow2 * fRec15[0]));
		fRec14[0] = fVec3[((IOTA - iConst4) & 8191)];
		fRec17[0] = ((fSlow4 * fRec17[1]) + (fSlow3 * fRec16[1]));
		fVec4[(IOTA & 8191)] = (fTemp1 + (fSlow2 * fRec17[0]));
		fRec16[0] = fVec4[((IOTA - iConst5) & 8191)];
		fRec19[0] = ((fSlow4 * fRec19[1]) + (fSlow3 * fRec18[1]));
		fVec5[(IOTA & 8191)] = (fTemp1 + (fSlow2 * fRec19[0]));
		fRec18[0] = fVec5[((IOTA - iConst6) & 8191)];
		fRec21[0] = ((fSlow3 * fRec20[1]) + (fSlow4 * fRec21[1]));
		fVec6[(IOTA & 8191)] = ((fSlow2 * fRec21[0]) + fTemp1);
		fRec20[0] = fVec6[((IOTA - iConst7) & 8191)];
		fRec23[0] = ((fSlow4 * fRec23[1]) + (fSlow3 * fRec22[1]));
		fVec7[(IOTA & 8191)] = ((fSlow2 * fRec23[0]) + fTemp1);
		fRec22[0] = fVec7[((IOTA - iConst8) & 8191)];
		double fTemp2 = (fRec8[0] + (fRec10[0] + (fRec12[0] + (fRec14[0] + (fRec16[0] + (fRec18[0] + (fRec20[0] + fRec22[0])))))));
		fVec8[(IOTA & 4095)] = ((0.5 * fRec6[1]) + fTemp2);
		fRec6[0] = fVec8[((IOTA - iConst9) & 4095)];
		double fRec7 = (fRec6[1] - fTemp2);
		fVec9[(IOTA & 2047)] = (fRec7 + (0.5 * fRec4[1]));
		fRec4[0] = fVec9[((IOTA - iConst10) & 2047)];
		double fRec5 = (fRec4[1] - fRec7);
		fVec10[(IOTA & 2047)] = (fRec5 + (0.5 * fRec2[1]));
		fRec2[0] = fVec10[((IOTA - iConst11) & 2047)];
		double fRec3 = (fRec2[1] - fRec5);
		fVec11[(IOTA & 1023)] = (fRec3 + (0.5 * fRec0[1]));
		fRec0[0] = fVec11[((IOTA - iConst12) & 1023)];
		double fRec1 = (fRec0[1] - fRec3);
		output0[i] = FAUSTFLOAT((fRec1 + (fSlow6 * fTemp0)));
		fRec9[1] = fRec9[0];
		IOTA = (IOTA + 1);
		fRec8[1] = fRec8[0];
		fRec11[1] = fRec11[0];
		fRec10[1] = fRec10[0];
		fRec13[1] = fRec13[0];
		fRec12[1] = fRec12[0];
		fRec15[1] = fRec15[0];
		fRec14[1] = fRec14[0];
		fRec17[1] = fRec17[0];
		fRec16[1] = fRec16[0];
		fRec19[1] = fRec19[0];
		fRec18[1] = fRec18[0];
		fRec21[1] = fRec21[0];
		fRec20[1] = fRec20[0];
		fRec23[1] = fRec23[0];
		fRec22[1] = fRec22[0];
		fRec6[1] = fRec6[0];
		fRec4[1] = fRec4[0];
		fRec2[1] = fRec2[0];
		fRec0[1] = fRec0[0];

	}
}