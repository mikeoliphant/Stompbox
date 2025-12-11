#include <math.h>
#include <algorithm>

#include "MultiGate.h"

MultiGate::MultiGate()
{
	Name = "MultiGate";
	Description = "Multi-band noise gate";

	threshold = -70;
	attackMS = 10;
	releaseMS = 10;
	softness = 12;
	strength = 24;

	NumParameters = MULTIGATE_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[MULTIGATE_THRESHOLD].Name = "Thresh";
	Parameters[MULTIGATE_THRESHOLD].MinValue = -120;
	Parameters[MULTIGATE_THRESHOLD].MaxValue = 0;
	Parameters[MULTIGATE_THRESHOLD].SourceVariable = &threshold;
	Parameters[MULTIGATE_THRESHOLD].DefaultValue = threshold;
	Parameters[MULTIGATE_THRESHOLD].RangePower = 2;
	Parameters[MULTIGATE_THRESHOLD].DisplayFormat = "{0:0}db";
	Parameters[MULTIGATE_THRESHOLD].Description = "Gate threshold";

	Parameters[MULTIGATE_STRENGTH].Name = "Strength";
	Parameters[MULTIGATE_STRENGTH].MinValue = 1;
	Parameters[MULTIGATE_STRENGTH].MaxValue = 96;
	Parameters[MULTIGATE_STRENGTH].SourceVariable = &strength;
	Parameters[MULTIGATE_STRENGTH].DefaultValue = strength;
	Parameters[MULTIGATE_STRENGTH].DisplayFormat = "{0:0}dB";
	Parameters[MULTIGATE_STRENGTH].Description = "Amount of gate attenuation";
	Parameters[MULTIGATE_STRENGTH].IsAdvanced = false;

	Parameters[MULTIGATE_ATTACK].Name = "Attack";
	Parameters[MULTIGATE_ATTACK].MinValue = 1;
	Parameters[MULTIGATE_ATTACK].MaxValue = 50;
	Parameters[MULTIGATE_ATTACK].SourceVariable = &attackMS;
	Parameters[MULTIGATE_ATTACK].DefaultValue = attackMS;
	Parameters[MULTIGATE_ATTACK].DisplayFormat = "{0:0}ms";
	Parameters[MULTIGATE_ATTACK].Description = "Reaction time for gate opening";
	Parameters[MULTIGATE_ATTACK].IsAdvanced = true;

	Parameters[MULTIGATE_RELEASE].Name = "Release";
	Parameters[MULTIGATE_RELEASE].MinValue = 1;
	Parameters[MULTIGATE_RELEASE].MaxValue = 50;
	Parameters[MULTIGATE_RELEASE].SourceVariable = &releaseMS;
	Parameters[MULTIGATE_RELEASE].DefaultValue = releaseMS;
	Parameters[MULTIGATE_RELEASE].DisplayFormat = "{0:0}ms";
	Parameters[MULTIGATE_RELEASE].Description = "Reaction time for gate closing";
	Parameters[MULTIGATE_RELEASE].IsAdvanced = true;

	Parameters[MULTIGATE_SOFTNESS].Name = "Soft";
	Parameters[MULTIGATE_SOFTNESS].MinValue = 1;
	Parameters[MULTIGATE_SOFTNESS].MaxValue = 18;
	Parameters[MULTIGATE_SOFTNESS].SourceVariable = &softness;
	Parameters[MULTIGATE_SOFTNESS].DefaultValue = softness;
	Parameters[MULTIGATE_SOFTNESS].DisplayFormat = "{0:0}dB";
	Parameters[MULTIGATE_SOFTNESS].Description = "dB range above threshold for smooth open/close";
	Parameters[MULTIGATE_SOFTNESS].IsAdvanced = true;
}

void MultiGate::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void MultiGate::instanceConstants(int samplingFreq)
{
	fSampleRate = samplingFreq;

	fConst0 = std::min<double>(1.92e+05, std::max<double>(1.0, double(fSampleRate)));
	fConst1 = std::tan(3769.9111843077517 / fConst0);
	fConst2 = fConst1 * (fConst1 + 1.4142135623730951) + 1.0;
	fConst3 = 2.0 / fConst2;
	fConst4 = fConst1 / fConst2;
	fConst5 = 1.0 / fConst2;
	fConst6 = std::tan(402.1238596594935 / fConst0);
	fConst7 = fConst6 * (fConst6 + 1.4142135623730951) + 1.0;
	fConst8 = 2.0 / fConst7;
	fConst9 = fConst6 / fConst7;
	fConst10 = 1.0 / fConst7;
	fConst11 = std::tan(25132.741228718343 / fConst0);
	fConst12 = fConst11 * (fConst11 + 1.4142135623730951) + 1.0;
	fConst13 = 2.0 / fConst12;
	fConst14 = fConst11 / fConst12;
	fConst15 = 1.0 / fConst12;
	fConst16 = 1.0 / fConst0;
}

void MultiGate::instanceClear() {
	for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
		fRec8[l0] = 0.0;
	}
	for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
		fRec9[l1] = 0.0;
	}
	for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
		fRec4[l2] = 0.0;
	}
	for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
		fRec5[l3] = 0.0;
	}
	for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
		fRec1[l4] = 0.0;
	}
	for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
		fRec2[l5] = 0.0;
	}
	for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
		fRec12[l6] = 0.0;
	}
	for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
		fRec13[l7] = 0.0;
	}
	for (int l8 = 0; l8 < 2; l8 = l8 + 1) {
		fRec16[l8] = 0.0;
	}
	for (int l9 = 0; l9 < 2; l9 = l9 + 1) {
		fRec17[l9] = 0.0;
	}
	for (int l10 = 0; l10 < 2; l10 = l10 + 1) {
		fRec0[l10] = 0.0;
	}
	for (int l11 = 0; l11 < 2; l11 = l11 + 1) {
		fRec21[l11] = 0.0;
	}
	for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
		fRec22[l12] = 0.0;
	}
	for (int l13 = 0; l13 < 2; l13 = l13 + 1) {
		fRec20[l13] = 0.0;
	}
	for (int l14 = 0; l14 < 2; l14 = l14 + 1) {
		fRec28[l14] = 0.0;
	}
	for (int l15 = 0; l15 < 2; l15 = l15 + 1) {
		fRec29[l15] = 0.0;
	}
	for (int l16 = 0; l16 < 2; l16 = l16 + 1) {
		fRec25[l16] = 0.0;
	}
	for (int l17 = 0; l17 < 2; l17 = l17 + 1) {
		fRec26[l17] = 0.0;
	}
	for (int l18 = 0; l18 < 2; l18 = l18 + 1) {
		fRec31[l18] = 0.0;
	}
	for (int l19 = 0; l19 < 2; l19 = l19 + 1) {
		fRec32[l19] = 0.0;
	}
	for (int l20 = 0; l20 < 2; l20 = l20 + 1) {
		fRec35[l20] = 0.0;
	}
	for (int l21 = 0; l21 < 2; l21 = l21 + 1) {
		fRec36[l21] = 0.0;
	}
	for (int l22 = 0; l22 < 2; l22 = l22 + 1) {
		fRec24[l22] = 0.0;
	}
	for (int l23 = 0; l23 < 2; l23 = l23 + 1) {
		fRec40[l23] = 0.0;
	}
	for (int l24 = 0; l24 < 2; l24 = l24 + 1) {
		fRec41[l24] = 0.0;
	}
	for (int l25 = 0; l25 < 2; l25 = l25 + 1) {
		fRec39[l25] = 0.0;
	}
}

void MultiGate::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = threshold;
	double fSlow1 = 0.001 * releaseMS;
	int iSlow2 = std::fabs(fSlow1) < 2.220446049250313e-16;
	double fSlow3 = ((iSlow2) ? 0.0 : std::exp(-(fConst16 / ((iSlow2) ? 1.0 : fSlow1))));
	double fSlow4 = 0.001 * attackMS;
	int iSlow5 = std::fabs(fSlow4) < 2.220446049250313e-16;
	double fSlow6 = ((iSlow5) ? 0.0 : std::exp(-(fConst16 / ((iSlow5) ? 1.0 : fSlow4))));
	double fSlow7 = softness;
	double fSlow8 = 1.0 / fSlow7;
	double fSlow9 = 0.05 * strength;
	for (int i0 = 0; i0 < count; i0 = i0 + 1) {
		double fTemp0 = double(input0[i0]);
		double fTemp1 = fRec8[1] + fConst1 * (fTemp0 - fRec9[1]);
		fRec8[0] = fConst3 * fTemp1 - fRec8[1];
		double fTemp2 = fRec9[1] + fConst4 * fTemp1;
		fRec9[0] = 2.0 * fTemp2 - fRec9[1];
		double fRec10 = fConst5 * fTemp1;
		double fRec11 = fTemp2;
		double fTemp3 = fRec11 + 1.4142135623730951 * fRec10;
		double fTemp4 = fRec4[1] + fConst1 * (fTemp0 - (fTemp3 + fRec5[1]));
		fRec4[0] = fConst3 * fTemp4 - fRec4[1];
		double fTemp5 = fRec5[1] + fConst4 * fTemp4;
		fRec5[0] = 2.0 * fTemp5 - fRec5[1];
		double fRec6 = fConst5 * fTemp4;
		double fRec7 = fTemp5;
		double fTemp6 = fRec11 + fRec7;
		double fTemp7 = fRec10 + fRec6;
		double fTemp8 = 1.4142135623730951 * fTemp7;
		double fTemp9 = fRec1[1] + fConst6 * (fTemp0 - (fTemp8 + fTemp6 + fRec2[1]));
		fRec1[0] = fConst8 * fTemp9 - fRec1[1];
		double fTemp10 = fRec2[1] + fConst9 * fTemp9;
		fRec2[0] = 2.0 * fTemp10 - fRec2[1];
		double fRec3 = fConst10 * fTemp9;
		double fTemp11 = fRec7 + fRec11 + 2.8284271247461903 * fRec3;
		double fTemp12 = fTemp8 + fTemp11;
		double fTemp13 = fRec12[1] + fConst11 * (fTemp0 - (fTemp12 + fRec13[1]));
		fRec12[0] = fConst13 * fTemp13 - fRec12[1];
		double fTemp14 = fRec13[1] + fConst14 * fTemp13;
		fRec13[0] = 2.0 * fTemp14 - fRec13[1];
		double fRec14 = fConst15 * fTemp13;
		double fRec15 = fTemp14;
		double fTemp15 = fRec15 + fTemp11;
		double fTemp16 = fRec14 + fTemp7;
		double fTemp17 = 1.4142135623730951 * fTemp16;
		double fTemp18 = fRec16[1] + fConst11 * (fTemp0 - (fTemp17 + fTemp15 + fRec17[1]));
		fRec16[0] = fConst13 * fTemp18 - fRec16[1];
		double fTemp19 = fRec17[1] + fConst14 * fTemp18;
		fRec17[0] = 2.0 * fTemp19 - fRec17[1];
		double fRec18 = fConst15 * fTemp18;
		double fRec19 = fTemp19;
		double fTemp20 = fTemp0 - (1.4142135623730951 * (fRec18 + fTemp16) + fRec19 + fTemp15);
		double fTemp21 = std::fabs(fTemp20);
		double fTemp22 = ((fTemp21 > fRec0[1]) ? fSlow6 : fSlow3);
		fRec0[0] = fTemp21 * (1.0 - fTemp22) + fRec0[1] * fTemp22;
		double fTemp23 = fRec21[1] + fConst11 * (fRec15 - fRec22[1]);
		fRec21[0] = fConst13 * fTemp23 - fRec21[1];
		double fTemp24 = fRec22[1] + fConst14 * fTemp23;
		fRec22[0] = 2.0 * fTemp24 - fRec22[1];
		double fRec23 = fTemp24;
		double fTemp25 = std::fabs(fRec23);
		double fTemp26 = ((fTemp25 > fRec20[1]) ? fSlow6 : fSlow3);
		fRec20[0] = fTemp25 * (1.0 - fTemp26) + fRec20[1] * fTemp26;
		double fTemp27 = fRec28[1] + fConst1 * (fRec11 - fRec29[1]);
		fRec28[0] = fConst3 * fTemp27 - fRec28[1];
		double fTemp28 = fRec29[1] + fConst4 * fTemp27;
		fRec29[0] = 2.0 * fTemp28 - fRec29[1];
		double fRec30 = fTemp28;
		double fTemp29 = fRec25[1] + fConst11 * (fRec30 - fRec26[1]);
		fRec25[0] = fConst13 * fTemp29 - fRec25[1];
		double fTemp30 = fRec26[1] + fConst14 * fTemp29;
		fRec26[0] = 2.0 * fTemp30 - fRec26[1];
		double fRec27 = fConst15 * fTemp29;
		double fTemp31 = 2.8284271247461903 * fRec27;
		double fTemp32 = fRec31[1] + fConst6 * (fRec30 - (fTemp31 + fRec32[1]));
		fRec31[0] = fConst8 * fTemp32 - fRec31[1];
		double fTemp33 = fRec32[1] + fConst9 * fTemp32;
		fRec32[0] = 2.0 * fTemp33 - fRec32[1];
		double fRec33 = fConst10 * fTemp32;
		double fRec34 = fTemp33;
		double fTemp34 = fRec34 + fTemp31;
		double fTemp35 = 1.4142135623730951 * fRec33 + fTemp34;
		double fTemp36 = fRec35[1] + fConst6 * (fRec30 - (fTemp35 + fRec36[1]));
		fRec35[0] = fConst8 * fTemp36 - fRec35[1];
		double fTemp37 = fRec36[1] + fConst9 * fTemp36;
		fRec36[0] = 2.0 * fTemp37 - fRec36[1];
		double fRec37 = fConst10 * fTemp36;
		double fRec38 = fTemp37;
		double fTemp38 = fRec30 - (1.4142135623730951 * (fRec33 + fRec37) + fRec38 + fTemp34);
		double fTemp39 = std::fabs(fTemp38);
		double fTemp40 = ((fTemp39 > fRec24[1]) ? fSlow6 : fSlow3);
		fRec24[0] = fTemp39 * (1.0 - fTemp40) + fRec24[1] * fTemp40;
		double fTemp41 = fRec40[1] + fConst6 * (fRec34 - fRec41[1]);
		fRec40[0] = fConst8 * fTemp41 - fRec40[1];
		double fTemp42 = fRec41[1] + fConst9 * fTemp41;
		fRec41[0] = 2.0 * fTemp42 - fRec41[1];
		double fRec42 = fTemp42;
		double fTemp43 = std::fabs(fRec42);
		double fTemp44 = ((fTemp43 > fRec39[1]) ? fSlow6 : fSlow3);
		fRec39[0] = fTemp43 * (1.0 - fTemp44) + fRec39[1] * fTemp44;
		output0[i0] = FAUSTFLOAT(fRec42 * std::pow(1e+01, -(fSlow9 * (1.0 - fSlow8 * std::max<double>(std::min<double>(2e+01 * std::log10(std::max<double>(2.2250738585072014e-308, fRec39[0])) - fSlow0, fSlow7), 0.0)))) + fTemp38 * std::pow(1e+01, -(fSlow9 * (1.0 - fSlow8 * std::max<double>(std::min<double>(2e+01 * std::log10(std::max<double>(2.2250738585072014e-308, fRec24[0])) - fSlow0, fSlow7), 0.0)))) + fRec23 * std::pow(1e+01, -(fSlow9 * (1.0 - fSlow8 * std::max<double>(std::min<double>(2e+01 * std::log10(std::max<double>(2.2250738585072014e-308, fRec20[0])) - fSlow0, fSlow7), 0.0)))) + fTemp20 * std::pow(1e+01, -(fSlow9 * (1.0 - fSlow8 * std::max<double>(std::min<double>(2e+01 * std::log10(std::max<double>(2.2250738585072014e-308, fRec0[0])) - fSlow0, fSlow7), 0.0)))));
		fRec8[1] = fRec8[0];
		fRec9[1] = fRec9[0];
		fRec4[1] = fRec4[0];
		fRec5[1] = fRec5[0];
		fRec1[1] = fRec1[0];
		fRec2[1] = fRec2[0];
		fRec12[1] = fRec12[0];
		fRec13[1] = fRec13[0];
		fRec16[1] = fRec16[0];
		fRec17[1] = fRec17[0];
		fRec0[1] = fRec0[0];
		fRec21[1] = fRec21[0];
		fRec22[1] = fRec22[0];
		fRec20[1] = fRec20[0];
		fRec28[1] = fRec28[0];
		fRec29[1] = fRec29[0];
		fRec25[1] = fRec25[0];
		fRec26[1] = fRec26[0];
		fRec31[1] = fRec31[0];
		fRec32[1] = fRec32[0];
		fRec35[1] = fRec35[0];
		fRec36[1] = fRec36[0];
		fRec24[1] = fRec24[0];
		fRec40[1] = fRec40[0];
		fRec41[1] = fRec41[0];
		fRec39[1] = fRec39[0];
	}
}

