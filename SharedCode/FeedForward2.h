#ifndef FEEDFORWARD2_H_INCLUDED
#define FEEDFORWARD2_H_INCLUDED

#include <memory>
#include <wdf.h>

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
    chowdsp::WDF::IdealVoltageSource Vin;
    chowdsp::WDF::ResistiveVoltageSource Vbias;

    chowdsp::WDF::Resistor R5 { 5100.0 };
    chowdsp::WDF::Resistor R8 { 1500.0 };
    chowdsp::WDF::Resistor R9 { 1000.0 };
    chowdsp::WDF::Resistor RVTop { 50000.0 };
    chowdsp::WDF::Resistor RVBot { 50000.0 };
    chowdsp::WDF::Resistor R15 { 22000.0 };
    chowdsp::WDF::Resistor R16 { 47000.0 };
    chowdsp::WDF::Resistor R17 { 27000.0 };
    chowdsp::WDF::Resistor R18 { 12000.0 };

    std::unique_ptr<chowdsp::WDF::Capacitor> C4;
    std::unique_ptr<chowdsp::WDF::Capacitor> C6;
    std::unique_ptr<chowdsp::WDF::Capacitor> C11;
    std::unique_ptr<chowdsp::WDF::Capacitor> C12;

    std::unique_ptr<chowdsp::WDF::WDFSeries> S1;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S2;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S3;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S4;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S5;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S6;
    std::unique_ptr<chowdsp::WDF::WDFSeries> S7;
    
    std::unique_ptr<chowdsp::WDF::PolarityInverter> I1;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P1;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P2;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P3;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P4;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P5;
    std::unique_ptr<chowdsp::WDF::WDFParallel> P6;
};
}

#endif // FEEDFORWARD2_H_INCLUDED
