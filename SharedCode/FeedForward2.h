#ifndef FEEDFORWARD2_H_INCLUDED
#define FEEDFORWARD2_H_INCLUDED

#include "SharedJuceHeader.h"

namespace GainStageSpace
{
class FeedForward2WDF
{
public:
    FeedForward2WDF (double sampleRate);

    void reset();
    void setGain (float gain);

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        Vin.incident (I1.reflected());
        I1.incident (Vin.reflected());
        auto y = R16.current();

        return (float) y;
    }

private:
    using Capacitor = chowdsp::WDF::Capacitor;
    using Resistor = chowdsp::WDF::Resistor;
    using ResVs = chowdsp::WDF::ResistiveVoltageSource;
    using IdealVs = chowdsp::WDF::IdealVoltageSource;

    IdealVs Vin;
    ResVs Vbias;

    Resistor R5 { 5100.0 };
    Resistor R8 { 1500.0 };
    Resistor R9 { 1000.0 };
    Resistor RVTop { 50000.0 };
    Resistor RVBot { 50000.0 };
    Resistor R15 { 22000.0 };
    Resistor R16 { 47000.0 };
    Resistor R17 { 27000.0 };
    Resistor R18 { 12000.0 };

    Capacitor C4;
    Capacitor C6;
    Capacitor C11;
    Capacitor C12;

    using S1Type = chowdsp::WDF::WDFSeriesT<Capacitor, Resistor>;
    S1Type S1 { C12, R18 };

    using P1Type = chowdsp::WDF::WDFParallelT<S1Type, Resistor>;
    P1Type P1 { S1, R17 };

    using S2Type = chowdsp::WDF::WDFSeriesT<Capacitor, Resistor>;
    S2Type S2 { C11, R15 };

    using S3Type = chowdsp::WDF::WDFSeriesT<S2Type, Resistor>;
    S3Type S3 { S2, R16 };

    using P2Type = chowdsp::WDF::WDFParallelT<S3Type, P1Type>;
    P2Type P2 { S3, P1 };

    using P3Type = chowdsp::WDF::WDFParallelT<P2Type, Resistor>;
    P3Type P3 { P2, RVBot };

    using S4Type = chowdsp::WDF::WDFSeriesT<P3Type, Resistor>;
    S4Type S4 { P3, RVTop };

    using S5Type = chowdsp::WDF::WDFSeriesT<Capacitor, Resistor>;
    S5Type S5 { C6, R9 };

    using P4Type = chowdsp::WDF::WDFParallelT<S4Type, S5Type>;
    P4Type P4 { S4, S5 };

    using P5Type = chowdsp::WDF::WDFParallelT<P4Type, Resistor>;
    P5Type P5 { P4, R8 };

    using S6Type = chowdsp::WDF::WDFSeriesT<P5Type, ResVs>;
    S6Type S6 { P5, Vbias };

    using P6Type = chowdsp::WDF::WDFParallelT<Resistor, Capacitor>;
    P6Type P6 { R5, C4 };

    using S7Type = chowdsp::WDF::WDFSeriesT<P6Type, S6Type>;
    S7Type S7 { P6, S6 };

    using I1Type = chowdsp::WDF::PolarityInverterT<S7Type>;
    I1Type I1 { S7 };
};
} // namespace GainStageSpace

#endif // FEEDFORWARD2_H_INCLUDED
