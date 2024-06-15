# AURAVOX

AuraVox is a virtual instrument built in C++ that performs real-time timbre transfer. This project is part of the thesis ***From Voice to Virtuosity: DDSP-based Timbre Transfer*** by Iker Landarech presented at Universitat Pompeu Fabra, Barcelona in July 2024.

The aim of AuraVox is to elevate vocal expressiveness by merging the organic qualities of the human voice with the acoustic properties of instruments. This fusion opens a new realm of possibilities, preserving the human organic quality in the expression and articulation of the instrument sound.

# GUI Overview
![AURAVOX - GITREPO_GUI_EXPLAINED_NEW](https://github.com/Ikerlandarech/AuraVox/assets/91797318/f3475995-c541-4a4e-af10-ecbe075d712e)

AuraVox comprises two main sections: the Load Section and the Studio Section.

- **Load Section**: Users can load files or drag and drop the input audio into the target audio player. Upon loading a valid file, the waveform is automatically displayed with its name and can be played back using the target audio Play/Pause button.

- **Studio Section**: After selecting the desired audio for timbre transfer, users can choose from **7 different TensorFlow Lite instrument models** by clicking on the model in the Studio Section. Once an instrument model is selected, AuraVox runs the TensorFlow inference pipeline internally on a separate thread. The Synthesized Audio Player then displays the converted output file, which can be played using the synthesized audio Play/Pause button. Finally, users can drag and drop the timbre-transferred output file directly into their Digital Audio Workstation to continue working on their session.

AuraVox is designed with minimalism in mind, featuring the fewest possible controls to avoid distracting users with parameter tweaking. This simplicity ensures seamless integration into users' workflows, allowing quick and effortless use of the plugin.

A demonstration of the plugin is available at:

[***Reki Sounds [Password: auravox]***](https://www.rekisounds.com/thesis).

# ABSTRACT:
Timbre is a crucial but elusive feature of music; it originates in the basic physical structure of sound waves but blossoms to touch upon a range of perceptual and social processes that are fundamental to how we derive meaning and emotion from music. This thesis explores timbre transfer with a new Differential Digital Signal Processing (DDSP)-based approach that leverages strong inductive biases without losing the expressive power of deep neural networks and end-to-end learning. By seamlessly integrating classic signal processing synthesizers as differentiable interpretable functions within a neural network, this method achieves high-fidelity generation without relying on large black-box autoregressive models or adversarial losses. Thus, gaining a domain-appropriate strong inductive bias and enabling a new interpretable representation and modular framework, based on parametric signal processors, to generative modeling. In this physically grounded approach, fundamental frequency and loudness envelopes are explicitly presented as domain-specific conditioning features to the model, which predicts the timbre via time-varying harmonic amplitudes and filtered noise, yielding an efficient end-to-end backpropagation and reducing model complexity and data requirements. This study presents state-of-the-art results through self-recorded trained models and demonstrates AuraVox, a virtual instrument implementation that enables real-time timbre transfer.

# SOMETHING LIKE METHODOLOGY

Initially, the model architecture presented during the prototyping needs to be translated into an audio plugin architecture, in this study this has been done by integrating the models into the C++ codebase using the TensorFlow C API, containing its corresponding CUDA kernels and backward pass implementations.
For model inference, all TensorFlow computations are executed on a separate thread leveraging TensorFlow Lite, a huge optimization alternative for using the models without experiencing buffer underruns in the main audio processing thread.

The following pipeline architecture diagram illustrates the system's structure.

![AURAVOX - GITREPO_BLOCK_DIAGRAM](https://github.com/Ikerlandarech/AuraVox/assets/91797318/e2af76cb-9197-4e5a-bd3d-c1c15df5a7a3)

As outlined in the proposed timbre transfer model architecture, the CREPE Large model serves as the pitch tracking network algorithm to extract the ground truth fundamental frequency. However, due to constraints within the audio plugin architecture, a smaller CREPE model known as CREPE Micro, trained on approximately ~160k parameters, is employed in this implementation. This model predicts logits of the larger CREPE Large model, which contains x137 times more parameters.

Integrating the decoder posed an additional challenge due to the limited selection of built-in operators in TensorFlow Lite. Consequently, a much smaller GRU recurrent neural network was implemented, with the state stored natively in C++. This optimization significantly reduced the TensorFlow Lite binary size from 150Mb to 7Mb. Loudness computation relies on the Root Mean Square (RMS) of the input signal, and in the synthesis phase, parameters for the harmonic and filtered noise synthesizers are still generated by the instrument model, comprising 60 harmonic components for additive synthesis and 65 noise magnitudes for filtering white noise.

# DISTRIBUTION


One of the primary implementation challenges has been the variability in frame rates, stemming from differing user block sizes and sample rates, alongside a fixed model input size (64ms), hop size (20ms), and sample rate (16kHz). This disparity is addressed through resampling, FIFOs at input and output stages, and threading inference separately. Given that the pretrained models are trained at 16kHz, input audio is downsampled to this rate before inference. Subsequently, synthesized audio is upsampled to the original user sample rate. 

