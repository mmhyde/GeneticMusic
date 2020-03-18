
#include "AudioPlayback/AudioEngine.h"
#include "portaudio.h"
#include "AudioPlayback/SynthesizerBase.h"
#include "Phrase.h"

#include <algorithm>
#include <iostream>

namespace Genetics {

	int PACallbackFunc(const void* p_input, void* p_output, unsigned long p_frameCount,
		const PaStreamCallbackTimeInfo* p_timeInfo,
		PaStreamCallbackFlags p_statusFlags, void* p_userData);

	AudioEngine::AudioEngine()
		: m_outputSynth(nullptr) {

	}

	AudioEngine::~AudioEngine() {

		if (m_outputSynth) { delete m_outputSynth; }
	}

	void AudioEngine::Initialize() {

		PaError error = Pa_Initialize();
		//if (error != paNoError)
		//	std::cout << Pa_GetErrorText(error) << std::endl;

		// setup the audio output stream
		PaStreamParameters streamParam;
		streamParam.device = Pa_GetDefaultOutputDevice();
		streamParam.channelCount = 1;
		streamParam.sampleFormat = paFloat32;
		streamParam.suggestedLatency = Pa_GetDeviceInfo(streamParam.device)->defaultHighOutputLatency;
		//std::cout << streamParam.suggestedLatency << std::endl;
		streamParam.hostApiSpecificStreamInfo = 0;

		Pa_OpenStream(&m_outStream, 0, &streamParam, 44100,
			256, 0, PACallbackFunc, this);
		error = Pa_StartStream(m_outStream);
		if (error)
			std::cout << "Error on stream start: " << Pa_GetErrorText(error) << std::endl;
	}

	void AudioEngine::Update() {

	}

	void AudioEngine::Shutdown() {

		// If there's a sound playing back, stop it before shutting down the stream
		if (!m_activeSound.isAvailable()) {

			m_activeSound.stop();
			while (!m_activeSound.isAvailable()) { /* Do Nothing */ }
		}

		PaError l_error = Pa_StopStream(m_outStream);
		l_error = Pa_Terminate();
	}

	void AudioEngine::SetSynthesizer(SynthesizerBase* outputSynth)
	{
		if (m_outputSynth) {
			delete m_outputSynth;
		}

		m_outputSynth = outputSynth;
	}

	void AudioEngine::Play(Phrase* measure)
	{

		if (m_outputSynth) {

			if (m_activeSound.isAvailable()) {

				float* tempBuffer = nullptr;
				uint32_t bufferLen = m_outputSynth->RenderMIDI(&tempBuffer, measure, Phrase::_numMeasures, Phrase::_smallestSubdivision);

				m_activeSound.setSource(tempBuffer, bufferLen, 1);
				m_activeSound.play();
			}
		}
		else {

			std::cout << "No output synthesizer found" << std::endl;
		}
	}

	void AudioEngine::Callback(float* outputBuffer, unsigned frameCount) {

		m_activeSound.getProcessedBuffer(outputBuffer, frameCount);
	}

	void AudioEngine::StopAll() {

		m_activeSound.stop();
	}

	int PACallbackFunc(const void* input, void* output, unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {

		AudioEngine* engine = reinterpret_cast<AudioEngine*>(userData);
		engine->Callback(reinterpret_cast<float*>(output), frameCount);

		return paContinue;
	}


} // namespace Genetics