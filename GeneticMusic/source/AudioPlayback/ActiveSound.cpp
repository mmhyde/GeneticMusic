// Morgen Hyde

#include "AudioPlayback/ActiveSound.h"

#include <cstring>
#include <algorithm>

namespace Genetics {

	ActiveSound::ActiveSound()
		: m_audioBuffer(nullptr), m_currentState(env_stopped) {

	}

	ActiveSound::~ActiveSound() {

		if (m_audioBuffer) {
			delete[] m_audioBuffer;
			m_audioBuffer = nullptr;
		}
	}

	void ActiveSound::setSource(float* audioBuffer, uint32_t bufferLen, uint16_t channels) {

		// If this object stores an audio buffer already correctly delete the memory
		if (m_audioBuffer) {
			delete[] m_audioBuffer;
		}

		m_audioBuffer = audioBuffer;
		m_bufferLength = bufferLen;
		m_channelCount = channels;

		m_playbackLoc = 0;

		m_currentState = env_stopped;
	}

	void ActiveSound::getProcessedBuffer(float* outputBuffer, uint32_t frameCount) {

		uint32_t outputSize = frameCount * m_channelCount;

		// If there's no valid buffer or the end has been reached, memset to 0 and return
		if (!m_audioBuffer || m_playbackLoc == m_bufferLength || m_currentState == env_stopped) {

			std::memset(outputBuffer, 0, outputSize * sizeof(float));
			return;
		}

		// Determine the number of samples from the buffer to copy
		uint32_t remainingSamples = static_cast<uint32_t>(m_playbackLoc - m_bufferLength);
		uint32_t copySize = std::min(outputSize, remainingSamples);

		// Perform the copy
		std::memcpy(outputBuffer, m_audioBuffer + m_playbackLoc, sizeof(float) * copySize);
		
		// If we ran out of samples, pad the end with zeros
		if (copySize < outputSize) {

			std::memset(outputBuffer + copySize, 0, sizeof(float) * (outputSize - copySize));
		}

		// Move playback head along
		m_playbackLoc += copySize;

		// Perform boundry checking for next bufer read
		if (m_playbackLoc >= m_bufferLength) {

			m_playbackLoc = m_bufferLength;
			m_currentState = env_stopped;
		}

		// Apply gain envelope to the signal
		for (uint32_t i = 0; i < frameCount; ++i) {

			outputBuffer[i] *= m_currentAttenuation;
			
			if (m_currentState == env_toPlay) {

				m_currentAttenuation += GainAdjust; // Add -20dB of gain till get to unity
				
				// Once we reach unity change state
				if (m_currentAttenuation >= 1.0f) {
					
					m_currentAttenuation = 1.0f;
					m_currentState = env_playing;
				}
			}
			else if (m_currentState == env_toStop) {

				m_currentAttenuation -= GainAdjust; // Remove -20 db of gain till we drop below -60 dB

				if (m_currentAttenuation <= GainThreshold) {

					m_currentAttenuation = GainThreshold;
					m_currentState = env_stopped;
				}
			}
		}
	}

} // namespace Genetics