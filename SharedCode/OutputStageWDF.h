#ifndef OUTPUTSTAGEWDF_H_INCLUDED
#define OUTPUTSTAGEWDF_H_INCLUDED

#include <memory>
#include <wdf.h>

class OutputStageWDF
{
public:
    OutputStageWDF() {}

    void reset (double sampleRate)
    {
        C15 = std::make_unique<chowdsp::WDF::Capacitor> (4.7e-6, sampleRate);

        P1 = std::make_unique<chowdsp::WDF::WDFParallel> (&R28, &RVBot);
        S1 = std::make_unique<chowdsp::WDF::WDFSeries> (P1.get(), &R25AndVTop);
        S2 = std::make_unique<chowdsp::WDF::WDFSeries> (S1.get(), C15.get());
        I1 = std::make_unique<chowdsp::WDF::PolarityInverter> (S2.get());
        Vin.connectToNode (I1.get());
    }

    void setLevel (float level)
    {
        R25AndVTop.setResistanceValue (560.0 + (double) ((1.0 - level) * 10000.0));
        RVBot.setResistanceValue (jmax ((double) level * 10000.0, 1.0));
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        Vin.incident (I1->reflected());
        I1->incident (Vin.reflected());
        auto y = R28.voltage();

        return (float) y;
    }

private:
    chowdsp::WDF::IdealVoltageSource Vin;
    std::unique_ptr<chowdsp::WDF::Capacitor> C15;
    chowdsp::WDF::Resistor R25AndVTop { 560 };
    chowdsp::WDF::Resistor RVBot { 50000.0 };
    chowdsp::WDF::Resistor R28 { 100e3 };

    std::unique_ptr<chowdsp::WDF::PolarityInverter> I1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S2;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P1;
};

#endif // OUTPUTSTAGEWDF_H_INCLUDED
