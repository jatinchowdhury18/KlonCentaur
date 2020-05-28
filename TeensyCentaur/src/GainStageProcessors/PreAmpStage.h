#ifndef PREAMPSTAGE_H_INCLUDED
#define PREAMPSTAGE_H_INCLUDED

#include <Audio.h>
#include <memory>
#include "wdf.h"

class PreAmpStage : public AudioStream
{
public:
    PreAmpStage() : AudioStream (1, inputQueueArray)
    {
        const double sampleRate = AUDIO_SAMPLE_RATE_EXACT;
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

    void setGain (float gain)
    {
        Vbias.setResistanceValue ((double) gain * 100.0e3);
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x);

        Vin.incident (I1->reflected());
        auto y = Vbias.voltage() + R6.voltage();
        I1->incident (Vin.reflected());

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
            yf = processSample(xf);
            y =  static_cast<int> (yf * 32768.0f);
            block->data[i] = y;

            ff1Current[i] = (float) Vbias2.current();
        }

        transmit(block);
        release(block);
    }

    float ff1Current[AUDIO_BLOCK_SAMPLES];

private:
    audio_block_t *inputQueueArray[1];

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

#endif // PREAMPSTAGE_H_INCLUDED
