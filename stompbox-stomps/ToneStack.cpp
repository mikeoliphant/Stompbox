#include <math.h>
#include <algorithm>

#include "ToneStack.h"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

//k = *(1e3);
//M = *(1e6);
//nF = *(1e-9);
//pF = *(1e-12);

TonestackDefinition tonestacks[TONSTACK_NUMPRESETS] =
{
	{ "Bassman", 250 * 1e3, 1 * 1e6, 25 * 1e3, 56 * 1e3, 250 * 1e-12, 20 * 1e-9, 20 * 1e-9 },
	{ "TwinReverb", 250 * 1e3, 250 * 1e3, 10 * 1e3, 100 * 1e3, 120 * 1e-12, 100 * 1e-9, 47 * 1e-9 },
	{ "Princeton", 250 * 1e3, 250 * 1e3, 4.8 * 1e3, 100 * 1e3, 250 * 1e-12, 100 * 1e-9, 47 * 1e-9 },
	{"FenderBluesJR", 250 * 1e3, 250 * 1e3, 25 * 1e3, 100 * 1e3, 250 * 1e-12, 22 * 1e-9, 22 * 1e-9},
	{ "FenderDeville", 250 * 1e3, 250 * 1e3, 25 * 1e3, 130 * 1e3, 250 * 1e-12, 100 * 1e-9, 22 * 1e-9 },
	{ "JCM800", 220 * 1e3, 1 * 1e6, 22 * 1e3, 33 * 1e3, 470 * 1e-12, 22 * 1e-9, 22 * 1e-9 },
	{ "JCM2000", 250 * 1e3, 1 * 1e6, 25 * 1e3, 56 * 1e3, 500 * 1e-12, 22 * 1e-9, 22 * 1e-9 },
	{ "JTM45", 250 * 1e3, 1 * 1e6, 25 * 1e3, 33 * 1e3, 270 * 1e-12, 22 * 1e-9, 22 * 1e-9 },
	{ "AC30", 1 * 1e6, 1 * 1e6, 10 * 1e3, 100 * 1e3, 50 * 1e-12, 22 * 1e-9, 22 * 1e-9 },
	{ "AC15", 220 * 1e3, 220 * 1e3, 220 * 1e3, 100 * 1e3, 470 * 1e-12, 100 * 1e-9, 47 * 1e-9 },
	{ "MesaBoogie", 250 * 1e3, 250 * 1e3, 25 * 1e3, 100 * 1e3, 250 * 1e-12, 100 * 1e-9, 47 * 1e-9 },
	{ "Soldano", 250 * 1e3, 1 * 1e6, 25 * 1e3, 47 * 1e3, 470 * 1e-12, 20 * 1e-9, 20 * 1e-9 },
};

std::vector<std::string> presetValues;

Tonestack::Tonestack()
{
	Name = "ToneStack";

	fVslider0 = FAUSTFLOAT(5);
	fVslider1 = FAUSTFLOAT(5);
	fVslider2 = FAUSTFLOAT(5);

	NumParameters = TONESTACK_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[TONESTACK_BASS].Name = "Bass";
	Parameters[TONESTACK_BASS].SourceVariable = &fVslider1;
	Parameters[TONESTACK_BASS].MaxValue = 10;
	Parameters[TONESTACK_BASS].DefaultValue = fVslider1;
	Parameters[TONESTACK_BASS].DisplayFormat = "{0:0.0}";

	Parameters[TONESTACK_MIDDLE].Name = "Middle";
	Parameters[TONESTACK_MIDDLE].SourceVariable = &fVslider0;
	Parameters[TONESTACK_MIDDLE].MaxValue = 10;
	Parameters[TONESTACK_MIDDLE].DefaultValue = fVslider0;
	Parameters[TONESTACK_MIDDLE].DisplayFormat = "{0:0.0}";

	Parameters[TONESTACK_TREBLE].Name = "Treble";
	Parameters[TONESTACK_TREBLE].SourceVariable = &fVslider2;
	Parameters[TONESTACK_TREBLE].MaxValue = 10;
	Parameters[TONESTACK_TREBLE].DefaultValue = fVslider2;
	Parameters[TONESTACK_TREBLE].DisplayFormat = "{0:0.0}";

	Parameters[TONESTACK_PRESET].Name = "Preset";
	Parameters[TONESTACK_PRESET].SourceVariable = &preset;
	Parameters[TONESTACK_PRESET].MaxValue = TONSTACK_NUMPRESETS;
	Parameters[TONESTACK_PRESET].DefaultValue = 0;
	Parameters[TONESTACK_PRESET].ParameterType = PARAMETER_TYPE_ENUM;
	Parameters[TONESTACK_PRESET].EnumValues = &presetValues;

	for (int i = 0; i < TONSTACK_NUMPRESETS; i++)
	{
		presetValues.push_back(tonestacks[i].Name);
	}
}

void Tonestack::SetParameterValue(StompBoxParameter *parameter, float value)
{
	StompBox::SetParameterValue(parameter, value);

	if (parameter == &Parameters[TONESTACK_PRESET])
	{
		needUpdate = true;
	}
}

void Tonestack::SetPreset(int newPreset)
{
	SetParameters(tonestacks[newPreset].R1, tonestacks[newPreset].R2, tonestacks[newPreset].R3, tonestacks[newPreset].R4, tonestacks[newPreset].C1, tonestacks[newPreset].C2, tonestacks[newPreset].C3);
}

void Tonestack::SetParameters(double r1, double r2, double r3, double r4, double c1, double c2, double c3)
{
	R1 = r1;
	R2 = r2;
	R3 = r3;
	R4 = r4;
	C1 = c1;
	C2 = c2;
	C3 = c3;

	NeedsInit = true;
}

void Tonestack::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);
	instanceClear();
}


void Tonestack::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = (2.0 * std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq))));
	fConst1 = (C1 * (R1 + R3));
	fConst2 = ((C2 * (R4 + R3)) + fConst1);
	fConst3 = (C1 + C2);
	fConst4 = (R2 * fConst3);
	fConst5 = (0.10000000000000001 * fConst4);
	fConst6 = (0.10000000000000001 * R3);
	fConst7 = mydsp_faustpower2_f(fConst0);
	fConst8 = ((R4 * R3) * C3);
	fConst9 = (R3 * C2);
	fConst10 = (C3 + C2);
	fConst11 = ((fConst8 * C2) + (C1 * (fConst8 + (((R1 * R3) * C2) + (R4 * (fConst9 + (R1 * fConst10)))))));
	fConst12 = (R3 * C3);
	fConst13 = (R1 * C1);
	fConst14 = (C1 * R3);
	fConst15 = (0.10000000000000001 * (0.0 - (fConst12 * ((((R4 * C2) - fConst13) - fConst14) - fConst9))));
	fConst16 = (0.010000000000000002 * ((fConst12 * R2) * fConst3));
	fConst17 = (0.010000000000000002 * ((mydsp_faustpower2_f(R3) * C3) * fConst3));
	fConst18 = (R4 * C3);
	fConst19 = (R4 + R1);
	fConst20 = (fConst18 + (C2 * fConst19));
	fConst21 = (C1 * fConst20);
	fConst22 = (0.10000000000000001 * (R2 * ((fConst18 * C2) + fConst21)));
	fConst23 = ((fConst14 * C3) * C2);
	fConst24 = (R4 * R1);
	fConst25 = (0.10000000000000001 * (0.0 - (fConst23 * (fConst24 - (R3 * fConst19)))));
	fConst26 = (0.010000000000000002 * (fConst23 * fConst19));
	fConst27 = (((fConst24 * C1) * C3) * C2);
	fConst28 = (0.10000000000000001 * R2);
	fConst29 = (R3 * fConst3);
	fConst30 = (0.10000000000000001 * (fConst1 + fConst9));
	fConst31 = (0.010000000000000002 * fConst29);
	fConst32 = (0.010000000000000002 * fConst4);
	fConst33 = (0.10000000000000001 * C1);
	fConst34 = (fConst24 * fConst10);
	fConst35 = (R2 * fConst20);
	fConst36 = (((C1 * C3) * C2) * fConst0);
	fConst37 = (0.010000000000000002 * (fConst24 * R2));
	fConst38 = (0.10000000000000001 * fConst24);
	fConst39 = (0.010000000000000002 * fConst24);
	fConst40 = (3.0 * fConst0);
	fConst41 = (3.0 * fConst36);
}

void Tonestack::instanceClear() {
	for (int l0 = 0; (l0 < 4); l0 = (l0 + 1)) {
		fRec0[l0] = 0.0;

	}
}

void Tonestack::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0)
{
	if (needUpdate)
	{
		SetPreset((int)preset);

		needUpdate = false;
	}

	double fSlow0 = double(fVslider0);
	double fSlow1 = double(fVslider1);
	double fSlow2 = (fConst0 * (fConst2 + ((fConst5 * fSlow0) + (C3 * (R4 + (fConst6 * fSlow1))))));
	double fSlow3 = (fConst11 + ((fSlow1 * ((fConst15 + (fConst16 * fSlow0)) - (fConst17 * fSlow1))) + (fConst22 * fSlow0)));
	double fSlow4 = ((R3 * fSlow1) - (R2 * fSlow0));
	double fSlow5 = ((fSlow1 * (fConst25 - (fConst26 * fSlow4))) + (fConst27 * (R3 + (fConst28 * fSlow0))));
	double fSlow6 = (fConst0 * fSlow5);
	double fSlow7 = (1.0 / (-1.0 - (fSlow2 + (fConst7 * (fSlow3 + fSlow6)))));
	double fSlow8 = double(fVslider2);
	double fSlow9 = (fConst0 * (fConst29 + (0.10000000000000001 * ((fConst4 * fSlow0) + ((fConst12 * fSlow1) + (fConst13 * fSlow8))))));
	double fSlow10 = ((R3 * (fConst21 + (C3 * (fSlow1 * ((fConst30 - (fConst31 * fSlow1)) + (fConst32 * fSlow0)))))) + (fConst33 * ((fConst34 * fSlow8) + (fConst35 * fSlow0))));
	double fSlow11 = ((fConst37 * (fSlow0 * fSlow8)) + (R3 * (((fConst19 * (fSlow1 * (fConst6 - (0.010000000000000002 * fSlow4)))) + (fConst38 * fSlow8)) - (fConst39 * (fSlow1 * fSlow8)))));
	double fSlow12 = (fConst36 * fSlow11);
	double fSlow13 = (0.0 - (fSlow9 + (fConst7 * (fSlow10 + fSlow12))));
	double fSlow14 = (fConst40 * fSlow5);
	double fSlow15 = ((fConst7 * (fSlow3 + fSlow14)) + (-3.0 - fSlow2));
	double fSlow16 = ((fSlow2 + (fConst7 * (fSlow3 - fSlow14))) + -3.0);
	double fSlow17 = (fSlow2 + (-1.0 - (fConst7 * (fSlow3 - fSlow6))));
	double fSlow18 = (fConst41 * fSlow11);
	double fSlow19 = ((fConst7 * (fSlow10 + fSlow18)) - fSlow9);
	double fSlow20 = (fSlow9 - (fConst7 * (fSlow10 - fSlow12)));
	double fSlow21 = (fSlow9 + (fConst7 * (fSlow10 - fSlow18)));
	for (int i = 0; (i < count); i = (i + 1)) {
		fRec0[0] = (double(input0[i]) - (fSlow7 * (((fSlow15 * fRec0[1]) + (fSlow16 * fRec0[2])) + (fSlow17 * fRec0[3]))));
		output0[i] = FAUSTFLOAT((fSlow7 * ((((fSlow13 * fRec0[0]) + (fSlow19 * fRec0[1])) + (fSlow20 * fRec0[3])) + (fSlow21 * fRec0[2]))));
		for (int j0 = 3; (j0 > 0); j0 = (j0 - 1)) {
			fRec0[j0] = fRec0[(j0 - 1)];

		}

	}
}
