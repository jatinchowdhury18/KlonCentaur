#ifndef PREAMPSTAGE_H_INCLUDED
#define PREAMPSTAGE_H_INCLUDED

#include <memory>
#include <wdf.h>

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
    chowdsp::WDF::IdealVoltageSource Vin;
    std::unique_ptr<chowdsp::WDF::Capacitor> C3;
    std::unique_ptr<chowdsp::WDF::Capacitor> C5;
    chowdsp::WDF::Resistor R6 { 10000.0 };
    chowdsp::WDF::ResistiveVoltageSource Vbias;

    chowdsp::WDF::Resistor R7 { 1500.0 };
    std::unique_ptr<chowdsp::WDF::Capacitor> C16;
    chowdsp::WDF::ResistiveVoltageSource Vbias2 { 15000.0 };

    std::unique_ptr<chowdsp::WDF::PolarityInverter> I1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S2;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S3;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S4;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P1;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P2;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P3;
};
}

#endif // PREAMPSTAGE_H_INCLUDED
