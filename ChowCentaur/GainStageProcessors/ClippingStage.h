#ifndef CLIPPINGSTAGE_H_INCLUDED
#define CLIPPINGSTAGE_H_INCLUDED

#include <pch.h>

namespace GainStageSpace
{
class DiodePair : public chowdsp::WDF::WDFNode
{
public:
    /** Creates a new WDF diode pair, with the given diode specifications.
     * @param Is: reverse saturation current
     * @param Vt: thermal voltage
     */
    DiodePair (double Is, double Vt) : WDFNode ("DiodePair"),
                                       Is (Is),
                                       Vt (Vt)
    {
        wrightOmegaLUT.initialise ([] (double x) { return std::real (wrightomega (x)); }, -1.0f, 1.0f, 1 << 18);
    }

    virtual ~DiodePair() {}

    inline void calcImpedance() override {}

    /** Accepts an incident wave into a WDF diode pair. */
    inline void incident (double x) noexcept override
    {
        a = x;
    }

    /** Propogates a reflected wave from a WDF diode pair. */
    inline double reflected() noexcept override
    {
        // See eqn (18) from reference paper
        double lambda = (double) chowdsp::WDF::signum (a);
        double wrightIn = std::log (next->R * Is / Vt) + (lambda * a + next->R * Is) / Vt;

        // Stefano D'Angelo's Wright Omega function is good at most values,
        // but has errors near zero, which cause audible distortion on very
        // quiet signal. So for quiet signals we use an LUT.
        if (std::abs (wrightIn) > 0.5)
        {
            b = a + 2 * lambda * (next->R * Is - Vt * chowdsp::Omega::omega4 (wrightIn));
        }
        else
        {
            b = a + 2 * lambda * (next->R * Is - Vt * wrightOmegaLUT.processSampleUnchecked (wrightIn));
        }

        return b;
    }

private:
    const double Is; // reverse saturation current
    const double Vt; // thermal voltage

    dsp::LookupTableTransform<double> wrightOmegaLUT;
};

class ClippingWDF
{
public:
    ClippingWDF (double sampleRate) : C9 (1.0e-6, sampleRate),
                                      C10 (1.0e-6, sampleRate)
    {
        reset();
    }

    void reset()
    {
        Vbias.setVoltage (0.0f);
        D23.connectToNode (&P1);
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        D23.incident (P1.reflected());
        P1.incident (D23.reflected());
        auto y = C10.current();

        return (float) y;
    }

private:
    using Capacitor = chowdsp::WDF::Capacitor;
    using Resistor = chowdsp::WDF::Resistor;
    using ResVs = chowdsp::WDF::ResistiveVoltageSource;

    ResVs Vin;
    Capacitor C9;
    Resistor R13 { 1000.0 };
    DiodePair D23 { 15e-6, 0.02585 };

    Capacitor C10;
    ResVs Vbias { 47000.0 };

    using I1Type = chowdsp::WDF::PolarityInverterT<ResVs>;
    I1Type I1 { Vin };

    using S1Type = chowdsp::WDF::WDFSeriesT<I1Type, Capacitor>;
    S1Type S1 { I1, C9 };

    using S2Type = chowdsp::WDF::WDFSeriesT<S1Type, Resistor>;
    S2Type S2 { S1, R13 };

    using S3Type = chowdsp::WDF::WDFSeriesT<Capacitor, ResVs>;
    S3Type S3 { C10, Vbias };

    using P1Type = chowdsp::WDF::WDFParallelT<S2Type, S3Type>;
    P1Type P1 { S2, S3 };
};

} // namespace GainStageSpace

#endif // CLIPPINGSTAGE_H_INCLUDED
