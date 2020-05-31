#ifndef GAINSTAGEMLPROC_H_INCLUDED
#define GAINSTAGEMLPROC_H_INCLUDED

#include <Audio.h>
#include <memory>
#include "GainStageML.h"

#include "Models/Model_gain0.h"
#include "Models/Model_gain25.h"
#include "Models/Model_gain50.h"
#include "Models/Model_gain75.h"
#include "Models/Model_gain100.h"

class GainStageMLProc : public AudioStream
{
public:
    GainStageMLProc() : AudioStream (1, inputQueueArray)
    {
        gainStageML[0] = std::make_unique<GainStageML<8>> (ModelGain0);
        gainStageML[1] = std::make_unique<GainStageML<8>> (ModelGain25);
        gainStageML[2] = std::make_unique<GainStageML<8>> (ModelGain50);
        gainStageML[3] = std::make_unique<GainStageML<8>> (ModelGain75);
        gainStageML[4] = std::make_unique<GainStageML<8>> (ModelGain100);

        for (int i = 0; i < 5; ++i)
            gainStageML[i]->reset();
    }

    void setGain(float gain)
    {
        modelIdx = std::min (std::max (int (5 * gain), 0), 4);
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
            yf = gainStageML[modelIdx]->processSample (2 * xf);
            y =  static_cast<int> (yf * 32768.0f);
            block->data[i] = y;
        }

        transmit(block);
        release(block);
    }

private:
    audio_block_t *inputQueueArray[1];

    std::unique_ptr<GainStageML<8>> gainStageML[5];
    int modelIdx = 0;
};

#endif // GAINSTAGEMLPROC_H_INCLUDED
