#pragma once

#include <string_view>

namespace ddsp
{

// If true, model inference is carried out on the audio thread instead of the timer thread.
constexpr bool kInferenceOnAudioThread = false;
constexpr bool kEnableReverb = true;

// Model related constants.
// Input and output tensor names as string refs.
inline constexpr std::string_view kInputTensorName_F0 = "call_f0_scaled:0";
inline constexpr std::string_view kInputTensorName_Loudness = "call_pw_scaled:0";
inline constexpr std::string_view kInputTensorName_State = "call_state:0";

inline constexpr std::string_view kOutputTensorName_Amplitude = "StatefulPartitionedCall:0";
inline constexpr std::string_view kOutputTensorName_Harmonics = "StatefulPartitionedCall:1";
inline constexpr std::string_view kOutputTensorName_NoiseAmps = "StatefulPartitionedCall:2";
inline constexpr std::string_view kOutputTensorName_State = "StatefulPartitionedCall:3";

// The models were trained at 16 kHz sample rate.
constexpr float kModelSampleRate_Hz = 16000.0f;
constexpr float kModelInferenceTimerCallbackInterval_ms = 20.0f;
constexpr float kTotalInferenceLatency_ms = 64.0f;
constexpr int kModelFrameSize = 1024;
constexpr int kModelHopSize = 320;

constexpr int kNumPredictControlsInputTensors = 3;
constexpr int kNumPredictControlsOutputTensors = 4;
constexpr int kNumFeatureExtractionThreads = 4;
constexpr int kNumPredictControlsThreads = 1;
constexpr int kNoiseAmpsSize = 65;
constexpr int kHarmonicsSize = 60;
constexpr int kAmplitudeSize = 1;
constexpr int kLoudnessSize = 1;
constexpr int kF0Size = 1;
constexpr int kNumEmbeddedPredictControlsModels = 11;
constexpr int kGruModelStateSize = 512;
} // namespace ddsp
