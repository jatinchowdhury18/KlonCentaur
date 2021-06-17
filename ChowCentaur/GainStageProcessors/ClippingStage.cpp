#include "ClippingStage.h"

using namespace GainStageSpace;

template <typename T, typename Next>
CustomDiodePairT<T, Next>::CustomDiodePairT (T Is, T Vt, Next& n) : Is (Is),
                                                                    Vt (Vt),
                                                                    oneOverVt ((T) 1 / Vt),
                                                                    next (n)
{
    next.connectToParent (this);
    calcImpedance();

    if (! lutIsInitialised)
    {
        wrightOmegaLUT.initialise ([] (double x) { return std::real (wrightomega (x)); }, -1.0f, 1.0f, 1 << 18);
        lutIsInitialised = true;
    }
}

template <typename T, typename Next>
dsp::LookupTableTransform<double> CustomDiodePairT<T, Next>::wrightOmegaLUT;

template <typename T, typename Next>
bool CustomDiodePairT<T, Next>::lutIsInitialised = false;

//======================================================================
ClippingWDF::ClippingWDF (double sampleRate) : C9 (1.0e-6, sampleRate),
                                               C10 (1.0e-6, sampleRate)
{
    reset();
}

void ClippingWDF::reset()
{
    Vbias.setVoltage (0.0f);
}
