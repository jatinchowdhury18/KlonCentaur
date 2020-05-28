#ifndef FF1CURRENT_H_INCLUDED
#define FF1CURRENT_H_INCLUDED

#include <Audio.h>
#include "PreAmpStage.h"

class FF1Current : public AudioStream
{
public:
    FF1Current(PreAmpStage& preAmp) : AudioStream (0, NULL), preAmp (preAmp) {}

    void update(void) override
    {
        audio_block_t *block;
        block = receiveWritable();
        if (!block) return;

        int16_t i, y;
        float yf;
        for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            yf = preAmp.ff1Current[i];
            y =  static_cast<int> (yf * 32768.0f);
            block->data[i] = 1000 * y;
        }

        transmit(block);
        release(block);
    }

private:
    PreAmpStage& preAmp;
};

#endif // FF1CURRENT_H_INCLUDED
