#ifndef FEEDFORWARD2_H_INCLUDED
#define FEEDFORWARD2_H_INCLUDED

#include "wdf.h"

namespace GainStageSpace
{
class FeedForward2WDF
{
public:
    FeedForward2WDF() {}

    void reset (double sampleRate);
    void setGain (float gain);

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        Vin.incident (I1->reflected());
        I1->incident (Vin.reflected());
        auto y = R16.current();

        return (float) y;
    }

private:
    WaveDigitalFilter::IdealVoltageSource Vin;
    WaveDigitalFilter::ResistiveVoltageSource Vbias;

    WaveDigitalFilter::Resistor R5 { 5100.0 };
    WaveDigitalFilter::Resistor R8 { 1500.0 };
    WaveDigitalFilter::Resistor R9 { 1000.0 };
    WaveDigitalFilter::Resistor RVTop { 50000.0 };
    WaveDigitalFilter::Resistor RVBot { 50000.0 };
    WaveDigitalFilter::Resistor R15 { 22000.0 };
    WaveDigitalFilter::Resistor R16 { 47000.0 };
    WaveDigitalFilter::Resistor R17 { 27000.0 };
    WaveDigitalFilter::Resistor R18 { 12000.0 };

    std::unique_ptr<WaveDigitalFilter::Capacitor> C4;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C6;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C11;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C12;

    std::unique_ptr<WaveDigitalFilter::WDFSeries> S1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S2;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S3;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S4;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S5;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S6;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S7;
    
    std::unique_ptr<WaveDigitalFilter::PolarityInverter> I1;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P1;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P2;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P3;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P4;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P5;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P6;
};
}

#endif // FEEDFORWARD2_H_INCLUDED
