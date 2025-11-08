#pragma once

#include "StompBox.h"
#include "NAM.h"
#include "Crossover.h"

enum
{
	NAMMULTIBAND_CROSSOVER_FREQ,
	NAMMULTIBAND_MODEL_GAIN,
	NAMMULTIBAND_MODEL_VOLUME,
	NAMMULTIBAND_MODEL,
	NAMMULTIBAND_NUMPARAMETERS
};

class NAMMultiBand : public StompBox
{
private:
	NAM nam;
	Crossover crossover;

	float crossoverFreq = 300;
	// hacky - won't work for buffer sizes > 4096
	float splitBuf[2][4096];

public:

	NAMMultiBand(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath);
	virtual ~NAMMultiBand() {}
	virtual void init(int samplingFreq);
	float GetParameterValue(StompBoxParameter* parameter) override;
	void SetParameterValue(StompBoxParameter* parameter, float value) override;
	virtual void compute(int count, float* input, float* output);
};