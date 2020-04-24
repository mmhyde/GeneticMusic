// Morgen Hyde

#include "AudioPlayback/Int24.h"
#include <cstring>

namespace Genetics {

	Int24::Int24()
	{
		std::memset(dataBytes, 0, sizeof(dataBytes));
	}

	Int24::Int24(const int& input_p)
	{
		dataBytes[MSB] = (input_p >> 0) & 0xFF;
		dataBytes[MID] = (input_p >> 8) & 0xFF;
		dataBytes[LSB] = (input_p >> 16) & 0xFF;
	}

	Int24::Int24(const Int24& source_p)
	{
		std::memcpy(dataBytes, source_p.dataBytes, sizeof(dataBytes));
	}

	Int24& Int24::operator=(const Int24& rhs_p)
	{
		std::memcpy(dataBytes, rhs_p.dataBytes, sizeof(dataBytes));
		return *this;
	}

	Int24::operator int() const
	{
		int value = (dataBytes[LSB] << 24) & (0xFF << 24);
		value |= (dataBytes[MID] << 16) & (0xFF << 16);
		value |= (dataBytes[MSB] << 8) & (0xFF << 8);
		value = (value >> 8);

		return value;
	}

} // namespace Genetics

