#pragma once

namespace Genetics {

	struct Phrase;
	class SynthesizerBase;

	typedef void* PaStream;

	class AudioEngine 
	{
	public:

		AudioEngine();
		~AudioEngine();

		// Delete copy and assignment to enforce singleton 
		AudioEngine(const AudioEngine&) = delete;
		AudioEngine& operator=(const AudioEngine&) = delete;

		void Initialize();
		void Update();
		void Shutdown();

		void SetSynthesizer(SynthesizerBase* outputSynth);

		void Play(Phrase* measure);

		void Callback(float* outputBuffer, unsigned frameCount);

		void StopAll();


	private:

		float* m_outputData;
		float* m_outputPosition;
		unsigned m_outputBufferLen;
		bool m_done;
		
		SynthesizerBase* m_outputSynth;
		PaStream m_outStream;
	};

} // namespace Genetics