#include "PreAmpStage.h"

using namespace GainStageSpace;

void PreAmpWDF::setGain (float gain)
{
    Vbias.setResistanceValue ((double) gain * 100.0e3);
}

void PreAmpWDF::reset (double sampleRate)
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
