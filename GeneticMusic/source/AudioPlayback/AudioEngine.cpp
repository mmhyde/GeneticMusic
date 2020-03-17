
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
		: m_outputData(nullptr), m_outputPosition(nullptr), m_done(true), m_outputBufferLen(0),
		  m_outputSynth(nullptr)
	{

	}

	AudioEngine::~AudioEngine()
	{
		if (m_outputData) { delete[] m_outputData; }
		if (m_outputSynth) { delete m_outputSynth; }
	}

	void AudioEngine::Initialize()
	{
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

	void AudioEngine::Update()
	{
		if (m_done && m_outputData) {
			delete[] m_outputData;
			m_outputData = nullptr;
		}
	}

	void AudioEngine::Shutdown()
	{
		PaError l_error = Pa_StopStream(m_outStream);
		//if (l_error != paNoError)
		//	std::cout << Pa_GetErrorText(l_error) << std::endl;

		l_error = Pa_Terminate();
		//if (l_error != paNoError)
		//	std::cout << Pa_GetErrorText(l_error) << std::endl;
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
	
		if (m_done == false && m_outputPosition) {
			// There's an actively playing back audio file so don't play
			return;
		}

		uint32_t measureCount = measure->_numMeasures;
		uint32_t subdivision = measure->_smallestSubdivision;
		if (m_outputSynth) {
			m_outputBufferLen = m_outputSynth->RenderMIDI(&m_outputData, measure, measureCount, subdivision);
			m_outputPosition = m_outputData;
			m_done = false;
		}
		else {
			std::cout << "No output synthesizer found, set one then try again" << std::endl;
		}

	}

	void AudioEngine::Callback(float* outputBuffer, unsigned frameCount)
	{
		if (!m_outputData || m_done) {
			std::memset(outputBuffer, 0, sizeof(float) * frameCount);
			return;
		}

		unsigned remainingSamples = static_cast<unsigned>(m_outputPosition - m_outputData);
		remainingSamples = m_outputBufferLen - remainingSamples;
		unsigned outputLen = std::min(frameCount, remainingSamples);

		std::memcpy(outputBuffer, m_outputPosition, sizeof(float) * outputLen);
		if (outputLen < frameCount) {
			std::memset(outputBuffer + outputLen, 0, sizeof(float) * (frameCount - outputLen));
		}

		m_outputPosition += outputLen;
		if (m_outputPosition - m_outputData >= m_outputBufferLen) {
			m_done = true;
		}

	}

	void AudioEngine::StopAll()
	{

	}

	int PACallbackFunc(const void* input, void* output, unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
	{
		AudioEngine* engine = reinterpret_cast<AudioEngine*>(userData);
		engine->Callback(reinterpret_cast<float*>(output), frameCount);

		return paContinue;
	}


} // namespace Genetics