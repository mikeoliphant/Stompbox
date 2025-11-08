#include "GuitarConvolver.h"

GuitarConvolver::GuitarConvolver(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
    fileType(folderName, fileExtensions, basePath),
    irLoader(fileType)

{
    Name = "Cabinet";
    Description = "Cabinet impulse response playback";

    NumParameters = CONVOLVER_NUMPARAMETERS;
    CreateParameters(NumParameters);

    Parameters[CONVOLVER_IMPULSE].Name = "Impulse";
    Parameters[CONVOLVER_IMPULSE].SourceVariable = &impulseIndex;
    Parameters[CONVOLVER_IMPULSE].ParameterType = PARAMETER_TYPE_FILE;
    Parameters[CONVOLVER_IMPULSE].FilePath = "Cabinets";
    Parameters[CONVOLVER_IMPULSE].EnumValues = &fileType.GetFileNames();
    Parameters[CONVOLVER_IMPULSE].DefaultValue = -1;
    Parameters[CONVOLVER_IMPULSE].Description = "Selected impulse response";
    Parameters[CONVOLVER_IMPULSE].MinValue = -1;
    Parameters[CONVOLVER_IMPULSE].MaxValue = (float)(fileType.GetFileNames().size()) - 1;

    Parameters[CONVOLVER_DRY].Name = "Dry";
    Parameters[CONVOLVER_DRY].SourceVariable = &dry;
    Parameters[CONVOLVER_DRY].DefaultValue = 0;
    Parameters[CONVOLVER_DRY].Description = "Strength of original signal";

    Parameters[CONVOLVER_WET].Name = "Wet";
    Parameters[CONVOLVER_WET].SourceVariable = &wet;
    Parameters[CONVOLVER_WET].DefaultValue = 1;
    Parameters[CONVOLVER_WET].Description = "Strength of impulse response output";
}

void GuitarConvolver::init(int samplingFreq)
{
    StompBox::init(samplingFreq);

    irLoader.SetSampleRate(samplingFreq);
}

void GuitarConvolver::SetParameterValue(StompBoxParameter *parameter, float value)
{
    StompBox::SetParameterValue(parameter, value);

    if (parameter == &Parameters[CONVOLVER_IMPULSE])
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