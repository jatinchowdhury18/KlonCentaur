#ifndef INPUTBUFFERWDF_H_INCLUDED
#define INPUTBUFFERWDF_H_INCLUDED

#include "wdf.h"

class InputBufferWDF
{
public:
    InputBufferWDF() {}

    void reset (double sampleRate)
    {
        C1 = std::make_unique<WaveDigitalFilter::Capacitor> (0.1e-6, sampleRate);
        Vbias.setVoltage (4.5);

        I1 = std::make_unique<WaveDigitalFilter::PolarityInverter> (&Vin);
        S1 = std::make_unique<WaveDigitalFilter::WDFSeries> (I1.get(), C1.get());
        S2 = std::make_unique<WaveDigitalFilter::WDFSeries> (S1.get(), &R2);
        Vbias.connectToNode (S2.get());
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        Vbias.incident (S2->reflected());
        auto y = Vbias.voltage() + R2.voltage();
        S2->incident (Vbias.reflected());

        return (float) y;
    }

private:
    WaveDigitalFilter::ResistiveVoltageSource Vin { 10000.0 };
    WaveDigitalFilter::Resistor R2 { 1.0e6 };
    std::unique_ptr<WaveDigitalFilter::Capacitor> C1;
    WaveDigitalFilter::IdealVoltageSource Vbias;

    std::unique_ptr<WaveDigitalFilter::PolarityInverter> I1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S2;
};

#endif // INPUTBUFFERWDF_H_INCLUDED
