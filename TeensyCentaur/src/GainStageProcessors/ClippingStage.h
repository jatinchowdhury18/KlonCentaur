#ifndef CLIPPINGSTAGE_H_INCLUDED
#define CLIPPINGSTAGE_H_INCLUDED

#include <Audio.h>
#include <memory>
#include "wdf.h"

extern "C"{
  int __exidx_start(){ return -1;}
  int __exidx_end(){ return -1; }
}

class ClippingStage : public AudioStream
{
public:
    ClippingStage() : AudioStream(1, inputQueueArray)
    {
        const double sampleRate = AUDIO_SAMPLE_RATE_EXACT;
        C9  = std::make_unique<WaveDigitalFilter::Capacitor> (1.0e-6, sampleRate);
        C10 = std::make_unique<WaveDigitalFilter::Capacitor> (1.0e-6, sampleRate);
        Vbias.setVoltage (4.5);

        I1 = std::make_unique<WaveDigitalFilter::PolarityInverter> (&Vin);
        S1 = std::make_unique<WaveDigitalFilter::WDFSeries> (I1.get(), C9.get());
        S2 = std::make_unique<WaveDigitalFilter::WDFSeries> (S1.get(), &R13);
        S3 = std::make_unique<WaveDigitalFilter::WDFSeries> (C10.get(), &Vbias);
        P1 = std::make_unique<WaveDigitalFilter::WDFParallel> (S2.get(), S3.get());
        D23.connectToNode (P1.get());
    }

    inline float processSample (float x)
    {
        Vin.setVoltage ((double) x + 4.5); // bias

        D23.incident (P1->reflected());
        P1->incident (D23.reflected());
        auto y = C10->current();

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

    WaveDigitalFilter::ResistiveVoltageSource Vin;
    std::unique_ptr<WaveDigitalFilter::Capacitor> C9;
    WaveDigitalFilter::Resistor R13 { 1000.0 };
    WaveDigitalFilter::DiodePair D23 { 15e-6, 0.02585 };
    std::unique_ptr<WaveDigitalFilter::Capacitor> C10;
    WaveDigitalFilter::ResistiveVoltageSource Vbias { 47000.0 };

    std::unique_ptr<WaveDigitalFilter::PolarityInverter> I1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S1;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S2;
    std::unique_ptr<WaveDigitalFilter::WDFSeries> S3;
    std::unique_ptr<WaveDigitalFilter::WDFParallel> P1;
};

#endif // CLIPPINGSTAGE_H_INCLUDED
