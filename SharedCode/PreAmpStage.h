#ifndef PREAMPSTAGE_H_INCLUDED
#define PREAMPSTAGE_H_INCLUDED

#include "wdf.h"

namespace GainStageSpace
{
class PreAmpWDF
{
public:
    PreAmpWDF() {}

    void setGain (float gain);
    void reset (double sampleRate);

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
