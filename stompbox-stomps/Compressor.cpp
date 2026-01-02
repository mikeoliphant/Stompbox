#include <math.h>
#include <algorithm>

#include "Compressor.h"

Compressor::Compressor()
{
	Name = "Compressor";
	Description = "Audio level compressor";

	fVslider0 = FAUSTFLOAT(1.0);	// wet/dry blend
	fHslider0 = FAUSTFLOAT(-20.0);	// threshold
	fHslider2 = FAUSTFLOAT(2);		// attack
	fHslider1 = FAUSTFLOAT(20);		// release
	fHslider3 = FAUSTFLOAT(3.0);	// knee
	fHslider4 = FAUSTFLOAT(2.0);	// ratio

	auto& threshParam = AddParameter();
	threshParam.Name = "Thresh";
	threshParam.SourceVariable = &fHslider0;
	threshParam.MinValue = -40;
	threshParam.MaxValue = 0;
	threshParam.DefaultValue = fHslider0;
	threshParam.DisplayFormat = "{0:0.0}dB";
	threshParam.Description = "Compression threshold in dB";

	auto& ratioParam = AddParameter();
	ratioParam.Name = "Ratio";
	ratioParam.SourceVariable = &fHslider4;
	ratioParam.MinValue = 1;
	ratioParam.MaxValue = 20;
	ratioParam.DefaultValue = fHslider4;
	ratioParam.DisplayFormat = "{0:0.0}";
	ratioParam.Description = "Compression ratio";

	auto& attackParam = AddParameter();
	attackParam.Name = "Attack";
	attackParam.MinValue = 0.2f;
	attackParam.MaxValue = 50.0f;
	attackParam.Step = 0.001f;
	attackParam.DefaultValue = fHslider2;
	attackParam.SourceVariable = &fHslider2;
	attackParam.DisplayFormat = "{0:0.0}ms";
	attackParam.Description = "Compression attack time (ms)";

	auto& releaseParam = AddParameter();
	releaseParam.Name = "Release";
	releaseParam.MinValue = 0;
	releaseParam.MaxValue = 1000.0f;
	releaseParam.Step = 0.1f;
	releaseParam.DefaultValue = fHslider1;
	releaseParam.SourceVariable = &fHslider1;
	releaseParam.DisplayFormat = "{0:0.0}ms";
	releaseParam.Description = "Compression release time (ms)";

	auto& wetDryParam = AddParameter();
	wetDryParam.Name = "Blend";
	wetDryParam.DefaultValue = fVslider0;
	wetDryParam.SourceVariable = &fVslider0;
	wetDryParam.Description = "Wet/dry effect blend";

	auto& compressionParam = AddParameter();
	compressionParam.Name = "Comp";
	compressionParam.DefaultValue = compression;
	compressionParam.SourceVariable = &compression;
	compressionParam.ParameterType = PARAMETER_TYPE_POWER;
	compressionParam.RangePower = 4;	// Since it is linear, put it through a power function to make smaller values more visible
	compressionParam.IsOutput = true;
	compressionParam.Description = "Current amount of compression";
}

void Compressor::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void Compressor::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSamplingFreq)));
	fConst1 = std::exp(-(1e+01f / fConst0));
	fConst2 = 1.0f - fConst1;
	fConst3 = 1.0f / fConst0;
}

void Compressor::instanceClear()
{
	for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
		fRec1[l0] = 0.0f;
	}
	for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
		fRec0[l1] = 0.0f;
	}
}

void Compressor::compute(int count, float* input0, float* output0)
{
	float fSlow0 = float(fHslider0);
	float fSlow1 = float(fVslider0);
	float fSlow2 = std::exp(-(fConst3 / std::max<float>(fConst3, 0.001f * float(fHslider1))));
	float fSlow3 = std::exp(-(fConst3 / std::max<float>(fConst3, 0.001f * float(fHslider2))));
	float fSlow4 = float(fHslider3);
	float fSlow5 = 1.0f / (fSlow4 + 0.001f);
	float fSlow6 = 1.0f - float(fHslider4);
	float fSlow7 = 0.05f * fSlow6;
	float fSlow8 = 1.0f - fSlow1;
	for (int i0 = 0; i0 < count; i0 = i0 + 1) {
		float fTemp0 = float(input0[i0]);
		fRec1[0] = fConst1 * fRec1[1] + fConst2 * std::fabs(fSlow1 * fTemp0 + 1e-20f);
		float fTemp1 = fSlow3 * float(fRec0[1] < fRec1[0]) + fSlow2 * float(fRec0[1] >= fRec1[0]);
		fRec0[0] = fRec0[1] * fTemp1 + fRec1[0] * (1.0f - fTemp1);
		float fTemp2 = std::max<float>(0.0f, fSlow4 + (2e+01f * std::log10(std::max<float>(1.1754944e-38f, fRec0[0])) - fSlow0));
		float fTemp3 = std::min<float>(1.0f, std::max<float>(0.0f, fSlow5 * fTemp2));
		fVbargraph0 = FAUSTFLOAT(std::pow(1e+01f, fSlow7 * (fTemp2 * fTemp3 / (1.0f - fSlow6 * fTemp3))));
		output0[i0] = FAUSTFLOAT(fTemp0 * (fSlow8 + fSlow1 * fVbargraph0));
		fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];
	}

	compression = 1.0f - fVbargraph0;
}
