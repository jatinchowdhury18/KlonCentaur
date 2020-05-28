#ifndef FEEDFORWARD2_H_INCLUDED
#define FEEDFORWARD2_H_INCLUDED

#include <Audio.h>
#include <memory>
#include "wdf.h"

class FeedForward2 : public AudioStream
{
public:
    FeedForward2() : AudioStream(1, inputQueueArray)
    {
        const double sampleRate = AUDIO_SAMPLE_RATE_EXACT;
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

    void setGain (float gain)
    {
        RVTop.setResistanceValue (std::max ((double) gain * 100e3, 1.0));
        RVBot.setResistanceValue (std::max ((1.0 - (double) gain) * 100e3, 1.0));
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        Vin.incident (I1->reflected());
        I1->incident (Vin.reflected());
        auto y = R16.current();

        return (float) y;
    }

    void update(void) override
    {
        audio_block_t *block;
        block = receiveWritable();
        if (!block) return;

        int16_t i, x, y;
        float xf, yf;
        for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            x = block->data[i];
            xf = static_cast<float> (x) / 32768.0f;
            yf = 1000 * processSample(xf);
            y =  static_cast<int> (yf * 32768.0f);
            block->data[i] = y;
        }

        transmit(block);
        release(block);
    }

private:
    audio_block_t *inputQueueArray[1];

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

#endif // FEEDFORWARD2_H_INCLUDED
