#ifndef CLIPPINGSTAGE_H_INCLUDED
#define CLIPPINGSTAGE_H_INCLUDED

#include "SharedJuceHeader.h"

namespace GainStageSpace
{
/** WDF Diode pair using 3rd-order Wright Omega function approximation to save cycles */
class CheapDiodePair : public chowdsp::WDF::WDFNode
{
public:
    /** Creates a new WDF diode pair, with the given diode specifications.
     * @param Is: reverse saturation current
     * @param Vt: thermal voltage
     */
    CheapDiodePair (double Is, double Vt) : chowdsp::WDF::WDFNode ("CheapDiodePair"),
                                            Is (Is),
                                            Vt (Vt)
    {
    }

    virtual ~CheapDiodePair() {}

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
        b = a + 2 * lambda * (next->R * Is - Vt * chowdsp::Omega::omega3 (std::log (next->R * Is / Vt) + (lambda * a + next->R * Is) / Vt));
        return b;
    }

private:
    const double Is; // reverse saturation current
    const double Vt; // thermal voltage
};

class ClippingWDF
{
public:
    ClippingWDF() {}

    void reset (double sampleRate)
    {
        C9 = std::make_unique<chowdsp::WDF::Capacitor> (1.0e-6, sampleRate);
        C10 = std::make_unique<chowdsp::WDF::Capacitor> (1.0e-6, sampleRate);
        Vbias.setVoltage (0.0f);

        I1 = std::make_unique<chowdsp::WDF::PolarityInverter> (&Vin);
        S1 = std::make_unique<chowdsp::WDF::WDFSeries> (I1.get(), C9.get());
        S2 = std::make_unique<chowdsp::WDF::WDFSeries> (S1.get(), &R13);
        S3 = std::make_unique<chowdsp::WDF::WDFSeries> (C10.get(), &Vbias);
        P1 = std::make_unique<chowdsp::WDF::WDFParallel> (S2.get(), S3.get());
        D23.connectToNode (P1.get());
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        D23.incident (P1->reflected());
        P1->incident (D23.reflected());
        auto y = C10->current();

        return (float) y;
    }

private:
    chowdsp::WDF::ResistiveVoltageSource Vin;
    std::unique_ptr<chowdsp::WDF::Capacitor> C9;
    chowdsp::WDF::Resistor R13 { 1000.0 };

#if JUCE_IOS
    CheapDiodePair D23 { 15e-6, 0.02585 };
#else
    chowdsp::WDF::DiodePair D23 { 15e-6, 0.02585 };
#endif

    std::unique_ptr<chowdsp::WDF::Capacitor> C10;
    chowdsp::WDF::ResistiveVoltageSource Vbias { 47000.0 };

    std::unique_ptr<chowdsp::WDF::PolarityInverter> I1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S2;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S3;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P1;
};

} // namespace GainStageSpace

#endif // CLIPPINGSTAGE_H_INCLUDED
