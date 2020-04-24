// Morgen Hyde
#pragma once

#include <cstdint>
#include <string>

// This file contains objects and functions to perform regular error checking on a particular object to detect 
// an error and alert the user to the problem

namespace Genetics {

	struct Phrase;
	typedef uint32_t GA_Error;

	std::string readErrorMessage(GA_Error errorCode);
	void printErrorMessage(GA_Error errorCode);

	GA_Error validateNoteCount(Phrase* phrase);

	GA_Error validateNoteLengths(Phrase* phrase);

	GA_Error validateRestOccurances(Phrase* phrase);

} // namespace Genetics