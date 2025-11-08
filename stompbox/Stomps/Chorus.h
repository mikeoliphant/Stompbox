#pragma once

#include "StompBox.h"

namespace chorus
{


	enum
	{
		//CHORUS_LEVEL,
		CHORUS_RATE,
		CHORUS_DEPTH,
		CHORUS_NUMPARAMETERS
	};

	class mydspSIG0 {

	private:

		int iRec5[2];

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
			for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
				iRec5[l1] = 0;

			}

		}

		void fillmydspSIG0(int count, double* output) {
			for (int i = 0; (i < count); i = (i + 1)) {
				iRec5[0] = (iRec5[1] + 1);
				output[i] = sin((9.5873799242852573e-05 * double((iRec5[0] + -1))));
				iRec5[1] = iRec5[0];

			}

		}
	};

	static mydspSIG0* newmydspSIG0() { return (mydspSIG0*)new mydspSIG0(); }
	static void deletemydspSIG0(mydspSIG0* dsp) { delete dsp; }

	static double ftbl0mydspSIG0[65536];

	class Chorus : public StompBox
	{
	private:
		int fSamplingFreq;

		FAUSTFLOAT fHslider0;
		double fRec0[2];
		double fConst0;
		double fConst1;
		FAUSTFLOAT fHslider1;
		double fConst2;
		FAUSTFLOAT fHslider2;
		double fRec5[2];
		double fConst3;
		double fConst4;
		double fRec1[2];
		double fRec2[2];
		double fRec3[2];
		double fRec4[2];
		FAUSTFLOAT fVslider0;
		int IOTA;
		double fVec0[131072];

	public:
		Chorus();
		virtual ~Chorus() {}
		virtual void init(int samplingFreq);
		virtual void instanceConstants(int samplingFreq);
		virtual void instanceClear();
		virtual void compute(int count, float* input, float* output);
	};
}