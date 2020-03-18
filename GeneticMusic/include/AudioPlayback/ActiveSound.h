// Morgen Hyde
#pragma once

#include <cstdint>

namespace Genetics {

	enum envState {
		env_toPlay,
		env_playing,
		env_toStop,
		env_stopped,
	};

	constexpr float GainThreshold = 0.0001f;
	constexpr float GainAdjust = 0.001f;

	class ActiveSound {

	public:
		ActiveSound();
		~ActiveSound();

		void setSource(float* audioBuffer, uint32_t bufferLen, uint16_t channels);
		void getProcessedBuffer(float* outputBuffer, uint32_t frameCount);

		void play() {

			m_currentState = env_toPlay;
			m_currentAttenuation = GainThreshold;
		}

		void stop() {

			m_currentState = env_toStop;
		}

		bool isAvailable() const {
			return m_currentState == env_stopped;
		}


	private:
		float* m_audioBuffer;
		uint32_t m_bufferLength;
		uint16_t m_channelCount;

		uint32_t m_playbackLoc;
	
		uint8_t m_currentState;
		float m_currentAttenuation;
	};


} // namespace Genetics