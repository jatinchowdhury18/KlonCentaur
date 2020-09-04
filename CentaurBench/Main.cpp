#include "JuceHeader.h"
#include <iostream>

namespace 
{
    constexpr double pluginSampleRate = 44100.0;
    constexpr int blockSizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    constexpr int numChannels = 2;
}

std::unique_ptr<AudioPluginInstance> getPlugin (String file);
void createRandomAudioInput (AudioBuffer<float>& buffer, double lengthSeconds);
double timeAudioProcess (AudioPluginInstance* plugin, AudioBuffer<float>& audio, const int blockSize);

int main (int argc, char* argv[])
{
    ignoreUnused (argc, argv);

    ScopedJuceInitialiser_GUI scopedJuce;

    std::cout << "Loading plugin..." << std::endl;

    // load plugin...
    // File pluginFile ("~/Developer/KlonCentaur/build/ChowCentaur/ChowCentaur_artefacts/VST3/ChowCentaur.vst3");
    File pluginFile ("D:/Documents/CCRMA/Research/Klon_Centaur/build/ChowCentaur/ChowCentaur_artefacts/Release/VST3/ChowCentaur.vst3");
    auto plugin = getPlugin (pluginFile.getFullPathName());

    if (plugin.get() == nullptr)
    {
        return 1;
    }

    AudioBuffer<float> audio;
    auto neuralParam = plugin->getParameters()[3];
    constexpr double audioLength = 100.0; // seconds
    
    for (auto blockSize : blockSizes)
    {
        std::cout << "Block size: " << blockSize << std::endl;

        std::cout << "Processing non-ML plugin..." << std::endl;
        createRandomAudioInput (audio, audioLength);
        neuralParam->setValue (0.0f);

        plugin->prepareToPlay (pluginSampleRate, blockSize);
        auto nonMlTime = timeAudioProcess (plugin.get(), audio, blockSize);
        plugin->releaseResources();

        std::cout << "Processing ML plugin..." << std::endl;
        createRandomAudioInput (audio, audioLength);
        neuralParam->setValue (1.0f);

        plugin->prepareToPlay (pluginSampleRate, blockSize);
        auto mlTime = timeAudioProcess (plugin.get(), audio, blockSize);
        plugin->releaseResources();

        std::cout << "Results:" << std::endl;
        std::cout << "NonML: processes 1 second of audio in " << nonMlTime / audioLength << " seconds" << std::endl;;
        std::cout << "ML: processes 1 second of audio in " << mlTime / audioLength << " seconds" << std::endl;;
    }

    return 0;
}

std::unique_ptr<AudioPluginInstance> getPlugin (String file)
{
    AudioPluginFormatManager pluginManager;
    pluginManager.addDefaultFormats();

    OwnedArray<PluginDescription> plugins;
    KnownPluginList pluginList;

    // attempt to load plugin from file
    File pluginFile;
    if (! File::isAbsolutePath (String(file)))
        pluginFile = File (File::getCurrentWorkingDirectory().getFullPathName() + "/" + file);
    else
        pluginFile = File (file);
    pluginList.scanAndAddDragAndDroppedFiles (pluginManager, StringArray (pluginFile.getFullPathName()), plugins);

    if (plugins.isEmpty()) // check if loaded
    {
        std::cout << "Error: unable to load plugin" << std::endl;
        return {};
    }

    // create plugin instance
    String error ("Unable to load plugin");
    auto plugin = pluginManager.createPluginInstance (*plugins.getFirst(), 44100.0, 256, error);

    return std::move (plugin);
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

double timeAudioProcess (AudioPluginInstance* plugin, AudioBuffer<float>& audio, const int blockSize)
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

