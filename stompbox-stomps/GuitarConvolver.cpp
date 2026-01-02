#include "GuitarConvolver.h"

GuitarConvolver::GuitarConvolver(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
    fileType(folderName, fileExtensions, basePath),
    irLoader(fileType)

{
    Name = "Cabinet";
    Description = "Cabinet impulse response playback";

    auto& impulseParam = AddParameter();
    impulseParam.Name = "Impulse";
    impulseParam.SourceVariable = &impulseIndex;
    impulseParam.ParameterType = PARAMETER_TYPE_FILE;
    impulseParam.FilePath = "Cabinets";
    impulseParam.EnumValues = &fileType.GetFileNames();
    impulseParam.DefaultValue = -1;
    impulseParam.Description = "Selected impulse response";
    impulseParam.MinValue = -1;
    impulseParam.MaxValue = (float)(fileType.GetFileNames().size()) - 1;

    auto& dryParam = AddParameter();
    dryParam.Name = "Dry";
    dryParam.SourceVariable = &dry;
    dryParam.DefaultValue = 0;
    dryParam.Description = "Strength of original signal";

    auto& wetParam = AddParameter();
    wetParam.Name = "Wet";
    wetParam.SourceVariable = &wet;
    wetParam.DefaultValue = 1;
    wetParam.Description = "Strength of impulse response output";
}

void GuitarConvolver::init(int samplingFreq)
{
    StompBox::init(samplingFreq);

    irLoader.SetSampleRate(samplingFreq);
}

void GuitarConvolver::SetParameterValue(StompBoxParameter &parameter, float value)
{
    StompBox::SetParameterValue(parameter, value);

    if (parameter.Name == "Impulse")
    {
        irLoader.LoadIndex((int)impulseIndex);
    }
}

void GuitarConvolver::compute(int count, float* input, float* output)
{
    auto conv = irLoader.GetCurrentData();

    if (conv == nullptr)
    {
        memcpy(output, input, count * sizeof(float));

        return;
    }

    conv->ConvolutionEngine.Add((WDL_FFT_REAL**)&input, count, 1);

    int copiedSoFar = 0;

    do
    {
        int nAvail = conv->ConvolutionEngine.Avail(count - copiedSoFar);

        WDL_FFT_REAL* convo = conv->ConvolutionEngine.Get()[0];

        if ((wet < 1) || (dry > 1))
        {
            float* dest = output + copiedSoFar;
            float* src = input + copiedSoFar;
                
            for (int sample = 0; sample < nAvail; sample++)
            {
                dest[sample] = (convo[sample] * wet) + (src[sample] * dry);
            }
        }
        else
        {
            memcpy(output + copiedSoFar, convo, nAvail * sizeof(float));
        }

        conv->ConvolutionEngine.Advance(nAvail);
        
        copiedSoFar += nAvail;
    }
    while (copiedSoFar < count);
}