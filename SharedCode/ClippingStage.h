#ifndef CLIPPINGSTAGE_H_INCLUDED
#define CLIPPINGSTAGE_H_INCLUDED

#include "wdf.h"

namespace GainStageSpace
{
class ClippingWDF
{
public:
    ClippingWDF() {}

    void reset (double sampleRate)
    {
        C9  = std::make_unique<WaveDigitalFilter::Capacitor> (1.0e-6, sampleRate);
        C10 = std::make_unique<WaveDigitalFilter::Capacitor> (1.0e-6, sampleRate);
        Vbias.setVoltage (4.5);

        I1 = std::make_unique<WaveDigitalFilter::PolarityInverter> (&Vin);
        S1 = std::make_unique<WaveDigitalFilter::WDFSeries> (I1.get(), C9.get());
        S2 = std::make_unique<WaveDigitalFilter::WDFSeries> (S1.get(), &R13);
        S3 = std::make_unique<WaveDigitalFilter::WDFSeries> (C10.get(), &Vbias);
        P1 = std::make_unique<WaveDigitalFilter::WDFParallel> (S2.get(), S3.get());
        D23.connectToNode (P1.get());
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x + 4.5); // bias

        D23.incident (P1->reflected());
        P1->incident (D23.reflected());
        auto y = C10->current();

        return (float) y;
    }

private:
    WaveDigitalFilter::ResistiveVoltageSource Vin;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C9;
    WaveDigitalFilter::Resistor R13 { 1000.0 };
    WaveDigitalFilter::DiodePair D23 { 15e-6, 0.02585 };
    std::unique_ptr<WaveDigitalFilter::Capacitor> C10;
    WaveDigitalFilter::ResistiveVoltageSource Vbias { 47000.0 };

    std::unique_ptr<WaveDigitalFilter::PolarityInverter> I1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S2;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S3;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P1;
};
}

#endif // CLIPPINGSTAGE_H_INCLUDED
