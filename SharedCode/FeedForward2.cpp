#include "FeedForward2.h"
#include "SharedJuceHeader.h"

using namespace GainStageSpace;

void FeedForward2WDF::reset (double sampleRate)
{
    C4 = std::make_unique<WaveDigitalFilter::Capacitor> (68e-9, sampleRate);
    C6 = std::make_unique<WaveDigitalFilter::Capacitor> (390e-9, sampleRate);
    C11 = std::make_unique<WaveDigitalFilter::Capacitor> (2.2e-9, sampleRate);
    C12 = std::make_unique<WaveDigitalFilter::Capacitor> (27e-9, sampleRate);
    Vbias.setVoltage (0.0);

    S1 = std::make_unique<WaveDigitalFilter::WDFSeries> (C12.get(), &R18);
    P1 = std::make_unique<WaveDigitalFilter::WDFParallel> (S1.get(), &R17);
    S2 = std::make_unique<WaveDigitalFilter::WDFSeries> (C11.get(), &R15);
    S3 = std::make_unique<WaveDigitalFilter::WDFSeries> (S2.get(), &R16);
    P2 = std::make_unique<WaveDigitalFilter::WDFParallel> (S3.get(), P1.get());
    P3 = std::make_unique<WaveDigitalFilter::WDFParallel> (P2.get(), &RVBot);

    S4 = std::make_unique<WaveDigitalFilter::WDFSeries> (P3.get(), &RVTop);
    S5 = std::make_unique<WaveDigitalFilter::WDFSeries> (C6.get(), &R9);
    P4 = std::make_unique<WaveDigitalFilter::WDFParallel> (S4.get(), S5.get());
    P5 = std::make_unique<WaveDigitalFilter::WDFParallel> (P4.get(), &R8);
    S6 = std::make_unique<WaveDigitalFilter::WDFSeries> (P5.get(), &Vbias);

    P6 = std::make_unique<WaveDigitalFilter::WDFParallel> (&R5, C4.get());
    S7 = std::make_unique<WaveDigitalFilter::WDFSeries> (P6.get(), S6.get());
    I1 = std::make_unique<WaveDigitalFilter::PolarityInverter> (S7.get());
        
    Vin.connectToNode (I1.get());
}

void FeedForward2WDF::setGain (float gain)
{
    RVTop.setResistanceValue (jmax ((double) gain * 100e3, 1.0));
    RVBot.setResistanceValue (jmax ((1.0 - (double) gain) * 100e3, 1.0));
}
