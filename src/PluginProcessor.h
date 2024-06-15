/*
  ==============================================================================

    PluginProcessor.h
    Created: 2 Mar 2024 11:44:02pm
    Author:  Iker

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "audio/tflite/InferencePipeline.h"


class TFGAuroVoxStudioAudioProcessor : public juce::AudioProcessor,
                                       public juce::ChangeBroadcaster
#if JucePlugin_Enable_ARA
    ,
                                       public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    TFGAuroVoxStudioAudioProcessor (bool singleThreaded);
    ~TFGAuroVoxStudioAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    ////////////////////////////////////////////////////////////////////////////////
    void loadInputFile();
    void loadInputFile (const juce::String& path);
    void loadOutputFile (const juce::String& path);
    void processOutputFile ();

    void setInputToggleButtonState (bool newState);
    void setOutputToggleButtonState (bool newState);
    void playInputButtonClicked();
    void playOutputButtonClicked();

    juce::String getSelectedModelName (int selectedModel);

    juce::AudioBuffer<float>& getInputWaveformBuffer() { return inputWaveformBuffer; }
    juce::AudioBuffer<float>& getOutputWaveformBuffer() { return outputWaveformBuffer; }

    std::atomic<bool>& isInputPlayed() { return isInputPlaying; }
    std::atomic<bool>& isOutputPlayed() { return isOutputPlaying; }
    std::atomic<bool>& isFileLoaded() { return isFileLoading; }
    std::atomic<int>& getSampleCount() { return sampleCount; }

    void updateMaxSamples (int newMaxSamples);

    void loadModel (int modelIdx);

    // Getters.
    int getCurrentModel() const;
    float getRMS() const;
    float getPitch() const;
    const ddsp::PredictControlsModel::Metadata getPredictControlsModelMetadata() const;
    juce::AudioProcessorValueTreeState& getValueTree();
    ddsp::ModelLibrary& getModelLibrary();
    juce::String getFileName();
    void setFileName (juce::File);
    juce::File getOutputFile();
    void setIndex(int idx);

private:
    std::atomic<bool> isInputPlaying { false };
    std::atomic<bool> isOutputPlaying { false };
    std::atomic<bool> isFileLoading { false };
    std::atomic<int> sampleCount { 0 };
    juce::String selectedModelName { "" };

    bool singleThreaded = false;
    int maxSamples;
    bool isModelLoaded = false;
    bool outputFileSaved = false;
    int currentModel = 0;
    juce::String fileName;

    juce::AudioFormatManager audioFormatManager;
    juce::AudioFormatReader* audioFormatReader { nullptr };
    std::unique_ptr<juce::AudioFormatReaderSource> playSource;

    juce::AudioBuffer<float> inputWaveformBuffer;
    juce::AudioBuffer<float> processedWaveformBuffer;
    juce::AudioBuffer<float> outputWaveformBuffer;

    juce::AudioTransportSource inputTransport;
    juce::AudioTransportSource outputTransport;

    juce::AudioProcessorValueTreeState mAPVTS;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    ddsp::ModelLibrary modelLibrary;
    ddsp::InferencePipeline ddspPipeline;
    juce::Reverb reverb;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TFGAuroVoxStudioAudioProcessor)
};