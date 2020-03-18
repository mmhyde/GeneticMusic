#pragma once

#include <cstdint>

#include "AudioPlayback/AudioDefinitions.h"

namespace Genetics {

	struct PhraseConfig {

		int numMeasures;
		int smallestSubdivision;
	};

	constexpr uint16_t DefaultPopulationSize = 8;
	constexpr uint16_t DefaultMeasureCount = 4;
	constexpr uint16_t DefaultSubdivision = 16;

	constexpr uint16_t DefaultGenCount = 10;

	constexpr MeterInfo DefaultMeter = { 80, 4, 4 };

	constexpr uint8_t MaxPitch = 108;
	constexpr uint8_t MinPitch = 21;


} // namespace Genetics