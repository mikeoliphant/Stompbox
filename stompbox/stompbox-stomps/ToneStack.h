#pragma once

#include "StompBox.h"

enum
{
	TONESTACK_BASS,
	TONESTACK_MIDDLE,
	TONESTACK_TREBLE,
	TONESTACK_PRESET,
	TONESTACK_NUMPARAMETERS
};

struct TonestackDefinition
{
	std::string Name;
	double R1;
	double R2;
	double R3;
	double R4;
	double C1;
	double C2;
	double C3;
};

enum
{
	TONESTACK_PRESET_BASSMAN,
	TONSTACK_PRESET_TWINREVERB,
	TONSTACK_PRESET_PRINCETON,
	TONSTACK_PRESET_FENDERBLUESJR,
	TONSTACK_PRESET_FENDERDEVILLE,
	TONSTACK_PRESET_JCM800,
	TONSTACK_PRESET_JCM2000,
	TONSTACK_PRESET_JTM45,
	TONSTACK_PRESET_AC30,
	TONSTACK_PRESET_AC15,
	TONSTACK_PRESET_MESABOOGIE,
	TONSTACK_PRESET_SOLDANO,
	TONSTACK_NUMPRESETS
};

extern TonestackDefinition tonestacks[];

class Tonestack : public StompBox
{
private:
	int fSamplingFreq;

	double R1;
	double R2;
	double R3;
	double R4;
	double C1;
	double C2;
	double C3;
	float preset;
	bool needUpdate = false;

	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	FAUSTFLOAT fVslider0;
	double fConst6;
	FAUSTFLOAT fVslider1;
	double fConst7;
	double fConst8;
	double fConst9;
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	double fConst18;
	double fConst19;
	double fConst20;
	double fConst21;
	double fConst22;
	double fConst23;
	double fConst24;
	double fConst25;
	double fConst26;
	double fConst27;
	double fConst28;
	double fConst29;
	FAUSTFLOAT fVslider2;
	double fConst30;
	double fConst31;
	double fConst32;
	double fConst33;
	double fConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	double fConst38;
	double fConst39;
	double fConst40;
	double fRec0[4];
	double fConst41;

public:
	Tonestack();
	virtual ~Tonestack() {}
	void SetParameterValue(StompBoxParameter* parameter, float value);
	virtual void SetPreset(int preset);
	virtual void SetParameters(double r1, double r2, double r3, double r4, double c1, double c2, double c3);
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
