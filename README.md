
# AURAVOX

AuraVox is a virtual instrument built in C++ that performs real-time timbre transfer.

AuraVox is an initiative part of the thesis ***From Voice to Virtuosity: DDSP-based Timbre Transfer*** by Iker Landarech presented at Universitat Pompeu Fabra (July 2024).

In this project, we aim to take vocal expressiveness to the next level. Combining the organic world of the human voice with the acoustic world opens a whole new world of possibilities keeping this human organic quality remain in the expression and articulation of the instrument sound. AuraVox supports 7 TensorFlow Models.

![AURAVOX - GITREPO_GUI_EXPLAINED_NEW](https://github.com/Ikerlandarech/AuraVox/assets/91797318/f3475995-c541-4a4e-af10-ecbe075d712e)
Regarding the GUI, as it can be observed in Figure 11, AuraVox is essentially composed on two main sections. In the Load Section, the user is able to load files or directly drag and drop the input audio into the target audio player. Upon loading a valid file, the waveform is automatically displayed, and it can be played back in the waveform display using the target audio Play/Pause button. Once the user has selected the desired audio to apply the timbre transfer, it can choose between 7 different pretrained TensorFlow Lite instrument models by directly clicking to the model on the Studio Section. After selecting the instrument model, AuraVox will run the TensorFlow inference pipeline internally on a separate thread and the Synthesized Audio Player will display the converted output file that the user will be able to listen using the synthesized audio Play/Pause button. Finally, the user is able to drag and drop the timbre transferred output file back directly into the Digital Audio Workstation to continue working on the session.
As it can be observed, the design of AuraVox is intentionally minimalistic, featuring the fewest possible features and controls to avoid distracting users from tweaking parameters. Its simplicity allows users to seamlessly integrate it into their workflow, enabling quick and effortless use without spending much time on the plugin.
A demonstration of the plugin is available at [Reki Sounds (Password: auravox)](https://www.rekisounds.com/thesis)
.

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

