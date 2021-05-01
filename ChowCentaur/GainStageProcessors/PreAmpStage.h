#ifndef PREAMPSTAGE_H_INCLUDED
#define PREAMPSTAGE_H_INCLUDED

#include <pch.h>

namespace GainStageSpace
{
class PreAmpWDF
{
public:
    PreAmpWDF (double sampleRate);

    void setGain (float gain);
    void reset();

    inline float getFF1() noexcept
    {
        return (float) Vbias2.current();
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        Vin.incident (I1.reflected());
        auto y = Vbias.voltage() + R6.voltage();
        I1.incident (Vin.reflected());

        return (float) y;
    }

private:
    using Capacitor = chowdsp::WDF::Capacitor;
    using Resistor = chowdsp::WDF::Resistor;
    using IdealVs = chowdsp::WDF::IdealVoltageSource;
    using ResVs = chowdsp::WDF::ResistiveVoltageSource;

    Capacitor C3;
    Capacitor C5;
    Capacitor C16;

    Resistor R6 { 10000.0 };
    Resistor R7 { 1500.0 };

    IdealVs Vin;
    ResVs Vbias2 { 15000.0 };
    ResVs Vbias;

    using P1Type = chowdsp::WDF::WDFParallelT<Capacitor, Resistor>;
    P1Type P1 { C5, R6 };

    using S1Type = chowdsp::WDF::WDFSeriesT<P1Type, ResVs>;
    S1Type S1 { P1, Vbias };

    using P2Type = chowdsp::WDF::WDFParallelT<ResVs, Capacitor>;
    P2Type P2 { Vbias2, C16 };

    using S2Type = chowdsp::WDF::WDFSeriesT<P2Type, Resistor>;
    S2Type S2 { P2, R7 };

    using P3Type = chowdsp::WDF::WDFParallelT<S1Type, S2Type>;
    P3Type P3 { S1, S2 };

    using S3Type = chowdsp::WDF::WDFSeriesT<P3Type, Capacitor>;
    S3Type S3 { P3, C3 };

    using I1Type = chowdsp::WDF::PolarityInverterT<S3Type>;
    I1Type I1 { S3 };
};
} // namespace GainStageSpace

#endif // PREAMPSTAGE_H_INCLUDED
