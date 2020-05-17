#ifndef OUTPUTSTAGEWDF_H_INCLUDED
#define OUTPUTSTAGEWDF_H_INCLUDED

#include "wdf.h"

class OutputStageWDF
{
public:
    OutputStageWDF() {}

    void reset (double sampleRate)
    {
        C15 = std::make_unique<WaveDigitalFilter::Capacitor> (4.7e-6, sampleRate);
        
        P1 = std::make_unique<WaveDigitalFilter::WDFParallel> (&R28, &RVBot);
        S1 = std::make_unique<WaveDigitalFilter::WDFSeries> (P1.get(), &R25AndVTop);
        S2 = std::make_unique<WaveDigitalFilter::WDFSeries> (S1.get(), C15.get());
        I1 = std::make_unique<WaveDigitalFilter::PolarityInverter> (S2.get());
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
    WaveDigitalFilter::IdealVoltageSource Vin;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C15;
    WaveDigitalFilter::Resistor R25AndVTop { 560 };
    WaveDigitalFilter::Resistor RVBot { 50000.0 };
    WaveDigitalFilter::Resistor R28 { 100e3 };

    std::unique_ptr<WaveDigitalFilter::PolarityInverter> I1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S2;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P1;
};

#endif // OUTPUTSTAGEWDF_H_INCLUDED
