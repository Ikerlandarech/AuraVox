/*
  ==============================================================================

    PluginProcessor.cpp
    Created: 2 Mar 2024 11:44:02pm
    Author:  Iker

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ui/ParamInfo.h"
#include "util/InputUtils.h"
#include <juce_audio_utils/juce_audio_utils.h>

using namespace ddsp;

//==============================================================================
TFGAuroVoxStudioAudioProcessor::TFGAuroVoxStudioAudioProcessor (bool st)
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
#endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                          ),
#endif
        singleThreaded (st),
        mAPVTS (*this, nullptr, "PARAMETERS", createParameterLayout()),
        ddspPipeline (mAPVTS)
{
    ddspPipeline.reset();
    audioFormatManager.registerBasicFormats();
}

TFGAuroVoxStudioAudioProcessor::~TFGAuroVoxStudioAudioProcessor() { audioFormatReader = nullptr; }

//==============================================================================
const juce::String TFGAuroVoxStudioAudioProcessor::getName() const { return JucePlugin_Name; }

bool TFGAuroVoxStudioAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TFGAuroVoxStudioAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool TFGAuroVoxStudioAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double TFGAuroVoxStudioAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int TFGAuroVoxStudioAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int TFGAuroVoxStudioAudioProcessor::getCurrentProgram() { return 0; }

void TFGAuroVoxStudioAudioProcessor::setCurrentProgram (int index) {}

const juce::String TFGAuroVoxStudioAudioProcessor::getProgramName (int index) { return {}; }

void TFGAuroVoxStudioAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

//==============================================================================
void TFGAuroVoxStudioAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // The pitch detection model needs a full 64ms frame to get an accurate reading.
    // Therefore the pulgin latency is set to 64ms.
    setLatencySamples ((kTotalInferenceLatency_ms / 1000.0f) * sampleRate);

    reverb.setSampleRate (sampleRate);

    ddspPipeline.prepareToPlay (sampleRate, samplesPerBlock);

    if (isNonRealtime())
    {
        DBG ("PrepareToPlay non real time");
        ddspPipeline.reset();
    }
    else
    {
        DBG ("PrepareToPlay realtime");
        loadModel (currentModel);
    }

    if (! singleThreaded)
    {
        ddspPipeline.startTimer (kModelInferenceTimerCallbackInterval_ms);
    }
    inputTransport.prepareToPlay (samplesPerBlock, sampleRate);
}

void TFGAuroVoxStudioAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    // Only engage timer thread if not in single-threaded mode.
    if (! singleThreaded)
    {
        ddspPipeline.stopTimer();
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TFGAuroVoxStudioAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void TFGAuroVoxStudioAudioProcessor::processOutputFile () {
    // Mono mixdown.
    processedWaveformBuffer = inputWaveformBuffer;

    if (processedWaveformBuffer.getNumChannels() > 1)
    {
        processedWaveformBuffer.applyGain (0.5f);
        processedWaveformBuffer.addFrom (0, 0, processedWaveformBuffer, 1, 0, processedWaveformBuffer.getNumSamples());
    }

    ddspPipeline.processBlock (processedWaveformBuffer);

    //// Synchronous model inference block.
    if (singleThreaded || isNonRealtime())
    {
        ddspPipeline.stopTimer();
        ddspPipeline.render();
    }

    ddspPipeline.getNextBlock (
        processedWaveformBuffer);

    // Convert to stereo.
    if (processedWaveformBuffer.getNumChannels() > 1)
    {
        processedWaveformBuffer.copyFrom (1, 0, processedWaveformBuffer, 0, 0, processedWaveformBuffer.getNumSamples());
    }
    //////////////
    outputWaveformBuffer = processedWaveformBuffer;
}

void TFGAuroVoxStudioAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (! isModelLoaded)
    {
        buffer.clear();
        return;
    }

    juce::ScopedNoDenormals noDenormals;
    processOutputFile();

    if (isInputPlaying)
    {
        juce::AudioSourceChannelInfo inputBufferToFill (buffer);
        inputTransport.getNextAudioBlock (inputBufferToFill); //Copies audio data from transport to buffer.
        sampleCount += buffer.getNumSamples();
        if (sampleCount >= maxSamples)
        {
            sampleCount = 0;
            isInputPlaying = false;
        }
    }
    if (isOutputPlaying)
    {
        juce::AudioSourceChannelInfo outputBufferToFill (buffer);
        outputTransport.getNextAudioBlock (outputBufferToFill);
        sampleCount += buffer.getNumSamples();
        if (sampleCount >= maxSamples)
        {
            sampleCount = 0;
            isOutputPlaying = false;
        }
    }
    if (! isInputPlaying && ! isOutputPlaying)
    {
        sampleCount = 0;
    }
}

//==============================================================================
bool TFGAuroVoxStudioAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TFGAuroVoxStudioAudioProcessor::createEditor()
{
    return new TFGAuroVoxStudioAudioProcessorEditor (*this);
}

//==============================================================================
void TFGAuroVoxStudioAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void TFGAuroVoxStudioAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

void TFGAuroVoxStudioAudioProcessor::loadInputFile()
{
    juce::FileChooser fileChooser { "Choose your Audio File" };
    if (fileChooser.browseForFileToOpen())
    {
        auto file = fileChooser.getResult();
        audioFormatReader = audioFormatManager.createReaderFor (file);
        auto sampleLength = static_cast<int> (audioFormatReader->lengthInSamples);
        inputWaveformBuffer.setSize (1, sampleLength);
        audioFormatReader->read (&inputWaveformBuffer, 0, sampleLength, 0, true, false);

        auto buffer = inputWaveformBuffer.getReadPointer (0);
        updateMaxSamples (sampleLength);

        std::unique_ptr<juce::AudioFormatReaderSource> tempSource (
            new juce::AudioFormatReaderSource (audioFormatReader, true));
        inputTransport.setSource (tempSource.get());
        playSource.reset (tempSource.release());
        isFileLoading = true;
    }
}

void TFGAuroVoxStudioAudioProcessor::loadInputFile (const juce::String& path)
{
    auto file = juce::File (path);
    audioFormatReader = audioFormatManager.createReaderFor (file);
    auto sampleLength = static_cast<int> (audioFormatReader->lengthInSamples);
    inputWaveformBuffer.setSize (1, sampleLength);
    audioFormatReader->read (&inputWaveformBuffer, 0, sampleLength, 0, true, false);

    auto buffer = inputWaveformBuffer.getReadPointer (0);
    updateMaxSamples (sampleLength);

    std::unique_ptr<juce::AudioFormatReaderSource> tempSource (
        new juce::AudioFormatReaderSource (audioFormatReader, true));
    inputTransport.setSource (tempSource.get());
    playSource.reset (tempSource.release());
    isFileLoading = true;
}

void TFGAuroVoxStudioAudioProcessor::loadOutputFile()
{
    juce::FileChooser fileChooser { "Choose your Audio File" };
    if (fileChooser.browseForFileToOpen())
    {
        auto file = fileChooser.getResult();
        audioFormatReader = audioFormatManager.createReaderFor (file);
    }
    auto sampleLength = static_cast<int> (audioFormatReader->lengthInSamples);
    inputWaveformBuffer.setSize (1, sampleLength);
    audioFormatReader->read (&inputWaveformBuffer, 0, sampleLength, 0, true, false);

    auto buffer = inputWaveformBuffer.getReadPointer (0);
    updateMaxSamples (sampleLength);

    std::unique_ptr<juce::AudioFormatReaderSource> tempSource (
        new juce::AudioFormatReaderSource (audioFormatReader, true));
    inputTransport.setSource (tempSource.get());
    playSource.reset (tempSource.release());
}

void TFGAuroVoxStudioAudioProcessor::loadOutputFile (const juce::String& path)
{
    auto file = juce::File (path);
    audioFormatReader = audioFormatManager.createReaderFor (file);
    auto sampleLength = static_cast<int> (audioFormatReader->lengthInSamples);
    inputWaveformBuffer.setSize (1, sampleLength);
    audioFormatReader->read (&inputWaveformBuffer, 0, sampleLength, 0, true, false);

    auto buffer = inputWaveformBuffer.getReadPointer (0);
    updateMaxSamples (sampleLength);

    std::unique_ptr<juce::AudioFormatReaderSource> tempSource (
        new juce::AudioFormatReaderSource (audioFormatReader, true));
    inputTransport.setSource (tempSource.get());
    playSource.reset (tempSource.release());
}

void TFGAuroVoxStudioAudioProcessor::loadModel (int modelIdx)
{
    isModelLoaded = false;
    ddspPipeline.loadModel (modelLibrary.getModelList()[modelIdx]);
    currentModel = modelIdx;
    isModelLoaded = true;
}

int TFGAuroVoxStudioAudioProcessor::getCurrentModel() const { return currentModel; }

float TFGAuroVoxStudioAudioProcessor::getRMS() const { return ddspPipeline.getRMS(); }

float TFGAuroVoxStudioAudioProcessor::getPitch() const { return ddspPipeline.getPitch(); }

const PredictControlsModel::Metadata TFGAuroVoxStudioAudioProcessor::getPredictControlsModelMetadata() const
{
    return PredictControlsModel::getMetadata (modelLibrary.getModelList()[currentModel]);
}

juce::AudioProcessorValueTreeState& TFGAuroVoxStudioAudioProcessor::getValueTree() { return mAPVTS; }

ModelLibrary& TFGAuroVoxStudioAudioProcessor::getModelLibrary() { return modelLibrary; }

juce::String TFGAuroVoxStudioAudioProcessor::getFileName () { return fileName; }

void TFGAuroVoxStudioAudioProcessor::setFileName (juce::File myFile) 
{
    fileName = myFile.getFileNameWithoutExtension();
}


void TFGAuroVoxStudioAudioProcessor::setInputToggleButtonState (bool newState) { isInputPlaying = newState; }
void TFGAuroVoxStudioAudioProcessor::setOutputToggleButtonState (bool newState) { isOutputPlaying = newState; }

void TFGAuroVoxStudioAudioProcessor::updateMaxSamples (int newMaxSamples) { maxSamples = newMaxSamples; }

void TFGAuroVoxStudioAudioProcessor::playInputButtonClicked()
{
    if (isInputPlaying == true)
    {
        inputTransport.start();
    }
    if (isInputPlaying == false)
    {
        inputTransport.stop();
        inputTransport.setPosition (0.0);
    }
}

void TFGAuroVoxStudioAudioProcessor::playOutputButtonClicked()
{
    if (isOutputPlaying == true)
    {
        outputTransport.start();
    }
    if (isOutputPlaying == false)
    {
        outputTransport.stop();
        outputTransport.setPosition (0.0);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TFGAuroVoxStudioAudioProcessor (kInferenceOnAudioThread);
}