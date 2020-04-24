//Morgen Hyde

#pragma once

namespace Genetics {

	const float MAX_24 = (float((1 << 23) - 1));

	class Int24
	{
	public:
		Int24();
		Int24(const int& data_p);
		Int24(const Int24& source_p);

		operator int() const;
		Int24& operator=(const Int24& rhs_p);

	protected:
		char dataBytes[3];

	private:
		enum byteID {
			MSB = 0,
			MID = 1,
			LSB = 2
		};

	};

} // namespace Genetics