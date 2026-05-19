#include "NAM.h"

NAM::NAM(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
    fileType(folderName, fileExtensions, basePath),
    namLoader(fileType)
{
    Name = "NAM";
    Description = "Neural Amp Modeler capture playback";

    auto& modelParam = AddParameter();
    modelParam.Name = "Model";
    modelParam.SourceVariable = &modelIndex;
    modelParam.ParameterType = PARAMETER_TYPE_FILE;
    modelParam.FilePath = "NAM";
    modelParam.EnumValues = &fileType.GetFileNames();
    modelParam.DefaultValue = -1;
    modelParam.Description = "Selected NAM model";
    modelParam.MinValue = -1;
    modelParam.MaxValue = (float)(fileType.GetFileNames().size()) - 1;

    auto& qualityParam = AddParameter();
    qualityParam.Name = "Quality";
    qualityParam.SourceVariable = &qualityScale;
    qualityParam.ParameterType = PARAMETER_TYPE_KNOB;
    qualityParam.DefaultValue = 1.0f;
    qualityParam.Description = "Model quality scale factor";
    qualityParam.MinValue = 0.0f;
    qualityParam.MaxValue = 1.0f;
    qualityParam.IsAdvanced = true;
}

void NAM::init(int samplingFreq)
{
    StompBox::init(samplingFreq);
}

void NAM::SetParameterValue(StompBoxParameter& parameter, float value)
{
    StompBox::SetParameterValue(parameter, value);

    if (parameter.Name == "Model")
    {
        namLoader.LoadIndex((int)modelIndex);
    }
}

void NAM::compute(int count, float* input, float* output)
{
    auto activeModel = namLoader.GetCurrentData();

    if (activeModel == nullptr)
    {
        memcpy(output, input, count * sizeof(float));

        return;
    }

    if (qualityScale != currentQualityScale)
    {
        activeModel->SetQualityScaleFactor(qualityScale);

        currentQualityScale = qualityScale;
    }

    activeModel->Process(input, output, count);

    float adjustDB = activeModel->GetRecommendedOutputDBAdjustment();

    float modelLoudnessAdjustmentGain = (float)pow(10.0, adjustDB / 20.0);

    for (int i = 0; i < count; i++)
    {
        output[i] *= modelLoudnessAdjustmentGain;
    }
}