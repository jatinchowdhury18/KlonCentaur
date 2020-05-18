#ifndef PREAMPSTAGE_H_INCLUDED
#define PREAMPSTAGE_H_INCLUDED

#include "wdf.h"

namespace GainStageSpace
{
class PreAmpWDF
{
public:
    PreAmpWDF() {}

    void setGain (float gain)
    {
        Vbias.setResistanceValue ((double) gain * 100.0e3);
    }

    void reset (double sampleRate)
    {
        C3 = std::make_unique<WaveDigitalFilter::Capacitor> (0.1e-6, sampleRate);
        C5 = std::make_unique<WaveDigitalFilter::Capacitor> (68.0e-9, sampleRate);
        Vbias.setVoltage (0.0);

        C16 = std::make_unique<WaveDigitalFilter::Capacitor> (1.0e-6, sampleRate);
        Vbias2.setVoltage (0.0);
        
        P1 = std::make_unique<WaveDigitalFilter::WDFParallel> (C5.get(), &R6);
        S1 = std::make_unique<WaveDigitalFilter::WDFSeries> (P1.get(), &Vbias);

        P2 = std::make_unique<WaveDigitalFilter::WDFParallel> (&Vbias2, C16.get());
        S2 = std::make_unique<WaveDigitalFilter::WDFSeries> (P2.get(), &R7);

        P3 = std::make_unique<WaveDigitalFilter::WDFParallel> (S1.get(), S2.get());
        S3 = std::make_unique<WaveDigitalFilter::WDFSeries> (P3.get(), C3.get());
        I1 = std::make_unique<WaveDigitalFilter::PolarityInverter> (S3.get());
        Vin.connectToNode (I1.get());
    }

    inline float getFF1() noexcept
    {
        return (float) Vbias2.current();
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        Vin.incident (I1->reflected());
        auto y = Vbias.voltage() + R6.voltage();
        I1->incident (Vin.reflected());

        return (float) y;
    }

private:
    WaveDigitalFilter::IdealVoltageSource Vin;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C3;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C5;
    WaveDigitalFilter::Resistor R6 { 10000.0 };
    WaveDigitalFilter::ResistiveVoltageSource Vbias;

    WaveDigitalFilter::Resistor R7 { 1500.0 };
    std::unique_ptr<WaveDigitalFilter::Capacitor> C16;
    WaveDigitalFilter::ResistiveVoltageSource Vbias2 { 15000.0 };

    std::unique_ptr<WaveDigitalFilter::PolarityInverter> I1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S2;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S3;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S4;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P1;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P2;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P3;
};
}

#endif // PREAMPSTAGE_H_INCLUDED
