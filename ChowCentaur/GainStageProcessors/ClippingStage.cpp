#include "ClippingStage.h"

using namespace GainStageSpace;

DiodePair::DiodePair (double Is, double Vt) : WDFNode ("DiodePair"),
                                              Is (Is),
                                              Vt (Vt)
{
    if (! lutIsInitialised)
    {
        wrightOmegaLUT.initialise ([] (double x) { return std::real (wrightomega (x)); }, -1.0f, 1.0f, 1 << 18);
        lutIsInitialised = true;
    }
}

dsp::LookupTableTransform<double> DiodePair::wrightOmegaLUT;
bool DiodePair::lutIsInitialised = false;

ClippingWDF::ClippingWDF (double sampleRate) : C9 (1.0e-6, sampleRate),
                                               C10 (1.0e-6, sampleRate)
{
    reset();
}

void ClippingWDF::reset()
{
    Vbias.setVoltage (0.0f);
    D23.connectToNode (&P1);
}
