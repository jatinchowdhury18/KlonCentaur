#pragma once

#include <JuceHeader.h>
#include <iostream>

#include "ChowCentaurPlugin.h"

namespace
{
constexpr double pluginSampleRate = 44100.0;
constexpr int blockSizes[] = { 64, 128, 256, 512, 1024 };
constexpr int numChannels = 2;
} // namespace

void createRandomAudioInput (AudioBuffer<float>& buffer, double lengthSeconds);
double timeAudioProcess (AudioProcessor* plugin, AudioBuffer<float>& audio, const int blockSize);

int benchmarks (int argc, char* argv[])
{
    if (argc > 1 && std::string (argv[1]) == "--help")
    {
        std::cout << "ChowCentaur Benchmarks:" << std::endl;
        std::cout << "Usage: CentaurBench <seconds>" << std::endl;
        return 1;
    }

    ScopedJuceInitialiser_GUI scopedJuce;
    std::cout << "Loading plugin..." << std::endl;
    auto plugin = std::make_unique<ChowCentaur>();

    if (plugin.get() == nullptr)
    {
        std::cout << "Unable to load plugin! Exiting..." << std::endl;
        return 1;
    }

    AudioBuffer<float> audio;
    auto neuralParam = plugin->getParameters()[3];

    double audioLength = 100.0; // seconds
    if (argc > 1)
    {
        double tryAudioLength = std::atof (argv[1]);

        if (tryAudioLength > 0.0 && tryAudioLength < 1000.0)
            audioLength = tryAudioLength;
    }
    std::cout << "Using audio length " << audioLength << " seconds" << std::endl;

    for (auto blockSize : blockSizes)
    {
        std::cout << "Block size: " << blockSize << std::endl;

        std::cout << "Processing non-ML plugin..." << std::endl;
        createRandomAudioInput (audio, audioLength);
        neuralParam->setValueNotifyingHost (0.0f);

        plugin->prepareToPlay (pluginSampleRate, blockSize);
        auto nonMlTime = timeAudioProcess (plugin.get(), audio, blockSize);
        plugin->releaseResources();

        std::cout << "Processing ML plugin..." << std::endl;
        createRandomAudioInput (audio, audioLength);
        neuralParam->setValueNotifyingHost (1.0f);

        plugin->prepareToPlay (pluginSampleRate, blockSize);
        auto mlTime = timeAudioProcess (plugin.get(), audio, blockSize);
        plugin->releaseResources();

        std::cout << "Results:" << std::endl;
        std::cout << "NonML: processes 1 second of audio in " << nonMlTime / audioLength << " seconds" << std::endl;
        std::cout << "ML: processes 1 second of audio in " << mlTime / audioLength << " seconds" << std::endl;
    }

    return 0;
}

void createRandomAudioInput (AudioBuffer<float>& buffer, double lengthSeconds)
{
    const int numSamples = int (lengthSeconds * pluginSampleRate);
    buffer.setSize (numChannels, numSamples);

    Random rand;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        for (int n = 0; n < numSamples; ++n)
        {
            x[n] = rand.nextFloat() * 2.0f - 1.0f;
        }
    }
}

double timeAudioProcess (AudioProcessor* plugin, AudioBuffer<float>& audio, const int blockSize)
{
    Time time;

    auto totalNumSamples = audio.getNumSamples();
    int samplePtr = 0;
    MidiBuffer midi;

    auto start = time.getMillisecondCounterHiRes();
    while (totalNumSamples > 0)
    {
        auto curBlockSize = jmin (totalNumSamples, blockSize);
        totalNumSamples -= curBlockSize;

        AudioBuffer<float> curBuff (audio.getArrayOfWritePointers(), numChannels, samplePtr, curBlockSize);
        plugin->processBlock (curBuff, midi);

        samplePtr += curBlockSize;
    }

    return (time.getMillisecondCounterHiRes() - start) / 1000.0;
}
