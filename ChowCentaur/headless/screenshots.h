#include "ChowCentaurPlugin.h"

/**
  * Process audio through the plugin so the screenshots have
  * some signal show up in the meters and scopes.
  */
void processAudio (AudioProcessor* plugin)
{
    constexpr double fs = 48000.0;
    constexpr int blockSize = 1024;
    constexpr float freq = 200.0f;

    AudioBuffer<float> buffer (2, blockSize);
    for (int ch = 0; ch < 2; ++ch)
    {
        auto* data = buffer.getWritePointer (ch);
        for (int n = 0; n < blockSize; ++n)
            data[n] = -0.7f * std::cos (MathConstants<float>::twoPi * freq * (float) n / (float) fs);
    }

    plugin->prepareToPlay (fs, blockSize);

    MidiBuffer midi;
    plugin->processBlock (buffer, midi);
}

void screenshotForBounds (Component* editor, Rectangle<int> bounds, const File& dir, const String& filename)
{
    auto screenshot = editor->createComponentSnapshot (bounds);

    File pngFile = dir.getChildFile (filename);
    pngFile.deleteFile();
    pngFile.create();
    auto pngStream = pngFile.createOutputStream();

    if (pngStream->openedOk())
    {
        PNGImageFormat pngImage;
        pngImage.writeImageToStream (screenshot, *pngStream.get());
    }
}

void takeScreenshots()
{
    File outputDir = File::getSpecialLocation (File::SpecialLocationType::currentExecutableFile);
    outputDir = outputDir.getParentDirectory().getParentDirectory().getChildFile ("Paper/Figures");
    std::cout << "Generating screenshots... Saving to " << outputDir.getFullPathName() << std::endl;

    std::unique_ptr<AudioProcessor> plugin (createPluginFilterOfType (AudioProcessor::WrapperType::wrapperType_Standalone));
    std::unique_ptr<AudioProcessorEditor> editor (plugin->createEditorIfNeeded());
    processAudio (plugin.get());

    screenshotForBounds (editor.get(), editor->getLocalBounds(), outputDir, "Plugin.png");

    plugin->editorBeingDeleted (editor.get());
}
