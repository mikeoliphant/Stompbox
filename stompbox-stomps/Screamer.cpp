#include <math.h>
#include <algorithm>

#include "Screamer.h"

static double fmydspSIG0Wave0[100] = { 0.0,-0.029699014822700001,-0.059978067699199999,-0.090823164328100003,-0.122163239629,-0.15376009788,-0.18493800718200001,-0.214177260107,-0.23933543421299999,-0.25923257501899999,-0.274433909887,-0.28618330835400002,-0.29553854444,-0.30322232347700001,-0.30970624997700003,-0.315301338712,-0.320218440785,-0.32460498228099999,-0.32856712070299998,-0.33218335697500001,-0.33551312471900002,-0.33860236542,-0.34148724692999999,-0.34419670700799998,-0.34675423371699998,-0.34917913798,-0.351487480543,-0.35369275887000001,-0.35580642415199998,-0.35783827599500001,-0.35979676765500002,-0.36168924491900001,-0.36352213510499998,-0.36530109811299999,-0.36703114828900002,-0.36871675358799999,-0.37036191694300002,-0.37197024353699998,-0.37354499682800002,-0.37508914554400002,-0.37660540334600001,-0.378096262548,-0.37956402293800001,-0.381010816596,-0.38243862937700002,-0.38384931964300001,-0.385244634694,-0.386626225283,-0.387995658543,-0.38935442956499999,-0.39070397188,-0.392045667012,-0.39338085328799999,-0.39471083403000001,-0.39603688526899999,-0.39736026309799999,-0.39868221075299998,-0.40000396554700002,-0.40132676573300002,-0.402651857394,-0.40398050147100001,-0.40531398099900001,-0.40665360869200001,-0.40800073496,-0.409356756504,-0.410723125631,-0.41210136043899998,-0.41349305608499998,-0.414899897347,-0.41632367274499998,-0.41776629055600001,-0.41922979709699998,-0.42071639775899999,-0.42222848137699998,-0.42376864865399999,-0.42533974555800003,-0.42694490282800002,-0.42858758305700001,-0.43027163722400003,-0.43200137310199999,-0.43378163874600001,-0.43561792528600002,-0.43751649469199999,-0.43948454025700001,-0.44153039042300002,-0.44366377089800002,-0.44589614632199998,-0.44824117243400002,-0.45071530466100002,-0.45333863298799998,-0.45613605234999999,-0.45913894466999999,-0.46238766699,-0.46593535901100003,-0.469854010456,-0.47424461741099999,-0.47925525745100001,-0.48511588606,-0.49221272624399998,-0.50127272363099995 };

class mydspSIG0 {

private:

	int fmydspSIG0Wave0_idx;

public:

	int getNumInputsmydspSIG0() {
		return 0;

	}
	int getNumOutputsmydspSIG0() {
		return 1;

	}
	int getInputRatemydspSIG0(int channel) {
		int rate;
		switch (channel) {
		default: {
			rate = -1;
			break;
		}

		}
		return rate;

	}
	int getOutputRatemydspSIG0(int channel) {
		int rate;
		switch (channel) {
		case 0: {
			rate = 0;
			break;
		}
		default: {
			rate = -1;
			break;
		}

		}
		return rate;

	}

	void instanceInitmydspSIG0(int samplingFreq) {
		fmydspSIG0Wave0_idx = 0;

	}

	void fillmydspSIG0(int count, double* output) {
		for (int i = 0; (i < count); i = (i + 1)) {
			output[i] = fmydspSIG0Wave0[fmydspSIG0Wave0_idx];
			fmydspSIG0Wave0_idx = ((1 + fmydspSIG0Wave0_idx) % 100);

		}

	}
};

mydspSIG0* newmydspSIG0() { return (mydspSIG0*)new mydspSIG0(); }
void deletemydspSIG0(mydspSIG0* dsp) { delete dsp; }

static double ftbl0mydspSIG0[100];


TS9::TS9()
{
	Name = "Screamer";
	Description = "Tube screamer drive effect";

	fVslider0 = FAUSTFLOAT(-6.0);
	fVslider1 = FAUSTFLOAT(1000.0);
	fVslider2 = FAUSTFLOAT(0.5);

	NumParameters = SCREAMER_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[SCREAMER_DRIVE].Name = "Drive";
	Parameters[SCREAMER_DRIVE].SourceVariable = &fVslider2;
	Parameters[SCREAMER_DRIVE].DefaultValue = fVslider2;
	Parameters[SCREAMER_DRIVE].Description = "Drive/strength";

	Parameters[SCREAMER_TONE].Name = "Tone";
	Parameters[SCREAMER_TONE].MinValue = 100.0;
	Parameters[SCREAMER_TONE].MaxValue = 1000.0;
	Parameters[SCREAMER_TONE].SourceVariable = &fVslider1;
	Parameters[SCREAMER_TONE].DefaultValue = fVslider1;
	Parameters[SCREAMER_TONE].DisplayFormat = "{0:0}hz";
	Parameters[SCREAMER_TONE].Description = "Amount of high frequency roll-off";

	Parameters[SCREAMER_LEVEL].Name = "Level";
	Parameters[SCREAMER_LEVEL].MinValue = -20.0;
	Parameters[SCREAMER_LEVEL].MaxValue = 20.0;
	Parameters[SCREAMER_LEVEL].SourceVariable = &fVslider0;
	Parameters[SCREAMER_LEVEL].DefaultValue = fVslider0;
	Parameters[SCREAMER_LEVEL].Description = "Output volume level";
}

void TS9::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	mydspSIG0* sig0 = newmydspSIG0();
	sig0->instanceInitmydspSIG0(samplingFreq);
	sig0->fillmydspSIG0(100, ftbl0mydspSIG0);
	deletemydspSIG0(sig0);

	instanceConstants(samplingFreq);

	instanceClear();
}

void TS9::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (3.1415926535897931 / fConst0);
	fConst2 = (0.00044179999999999995 * fConst0);
	fConst3 = (1.0 / (fConst2 + 1.0));
	fConst4 = (1.0 - fConst2);
	fConst5 = (9.3999999999999995e-08 * fConst0);
}

void TS9::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fRec0[l0] = 0.0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		fVec0[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec2[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		fVec1[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		fRec1[l4] = 0.0;

	}
}

void TS9::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider0))));
	double fSlow1 = (1.0 / tan((fConst1 * double(fVslider1))));
	double fSlow2 = (1.0 / (fSlow1 + 1.0));
	double fSlow3 = (1.0 - fSlow1);
	double fSlow4 = (fConst5 * ((500000.0 * double(fVslider2)) + 55700.0));
	double fSlow5 = (fSlow4 + 1.0);
	double fSlow6 = (1.0 - fSlow4);
	for (int i = 0; (i < count); i = (i + 1)) {
		fRec0[0] = (fSlow0 + (0.999 * fRec0[1]));
		double fTemp0 = double(input0[i]);
		fVec0[0] = fTemp0;
		fRec2[0] = (0.0 - (fConst3 * ((fConst4 * fRec2[1]) - ((fSlow5 * fTemp0) + (fSlow6 * fVec0[1])))));
		double fTemp1 = (fRec2[0] - fTemp0);
		double fTemp2 = fabs(fTemp1);
		double fTemp3 = (101.97 * (fTemp2 / (fTemp2 + 3.0)));
		double fTemp4 = std::max<double>(0.0, std::min<double>(98.0, floor(fTemp3)));
		double fTemp5 = ((0.0 < fTemp4) ? 0.0 : ((fTemp4 < 98.0) ? (fTemp3 - fTemp4) : 98.0));
		double fTemp6 = (fTemp0 - (double((((fTemp2 * double(((fTemp1 < 0.0) ? 1 : -1))) < 0.0) ? -1 : 1)) * fabs(((ftbl0mydspSIG0[int(fTemp4)] * (1.0 - fTemp5)) + (ftbl0mydspSIG0[int((fTemp4 + 1.0))] * fTemp5)))));
		fVec1[0] = fTemp6;
		fRec1[0] = (0.0 - (fSlow2 * ((fSlow3 * fRec1[1]) - (fVec1[1] + fTemp6))));
		output0[i] = FAUSTFLOAT((fRec0[0] * fRec1[0]));
		fRec0[1] = fRec0[0];
		fVec0[1] = fVec0[0];
		fRec2[1] = fRec2[0];
		fVec1[1] = fVec1[0];
		fRec1[1] = fRec1[0];

	}
}