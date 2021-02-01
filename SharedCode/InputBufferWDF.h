#ifndef INPUTBUFFERWDF_H_INCLUDED
#define INPUTBUFFERWDF_H_INCLUDED

#include <memory>
#include <wdf.h>

class InputBufferWDF
{
public:
    InputBufferWDF() {}

    void reset (double sampleRate)
    {
        C1 = std::make_unique<chowdsp::WDF::Capacitor> (0.1e-6, sampleRate);
        Vbias.setVoltage (4.5);

        I1 = std::make_unique<chowdsp::WDF::PolarityInverter> (&Vin);
        S1 = std::make_unique<chowdsp::WDF::WDFSeries> (I1.get(), C1.get());
        S2 = std::make_unique<chowdsp::WDF::WDFSeries> (S1.get(), &R2);
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
    chowdsp::WDF::ResistiveVoltageSource Vin { 10000.0 };
    chowdsp::WDF::Resistor R2 { 1.0e6 };
    std::unique_ptr<chowdsp::WDF::Capacitor> C1;
    chowdsp::WDF::IdealVoltageSource Vbias;

    std::unique_ptr<chowdsp::WDF::PolarityInverter> I1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S2;
};

#endif // INPUTBUFFERWDF_H_INCLUDED
