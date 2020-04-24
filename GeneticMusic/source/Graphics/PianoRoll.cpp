// Morgen Hyde
#include "Graphics/PianoRoll.h"
#include "imgui/imgui.h"

#include "Graphics/ImGuiHelpers.h"


#include "Phrase.h"
#include "GAControllerInterfaces.h"

#include <iostream>


extern ImU32 whiteKey;
extern ImU32 blackKey;
extern ImU32 keyBorder;

extern ImU32 blackBackground;
extern ImU32 background;
extern ImU32 gridLines;

extern ImU32 timelineBackground;
extern ImU32 timelineColor;
extern ImU32 timelineNotchText;

extern ImU32 noteColor;
extern ImU32 chordColor;
extern ImU32 noteBorderColor;
extern ImU32 noteLineColor;

extern ImU32 ScrollBackground;
extern ImU32 ScrollTrough;
extern ImU32 ScrollBar;

// Editor Colors


namespace Genetics {

	enum {
		e_background = 0,
		e_midBackground,
		e_midForeground,
		e_foreground,
		e_totalLayers
	};

	PianoRoll::PianoRoll(std::unique_ptr<PianoRollInterface> _interface)
		: m_interface(std::move(_interface)) {

		constexpr unsigned numKeys = sizeof(m_keyArray) / sizeof(Key);

		// Populate the key array with their correct number
		for (unsigned i = 0; i < numKeys; ++i) {
			m_keyArray[i].m_noteValue = i;
		}
	}

	PianoRoll::~PianoRoll() {

	}

	void PianoRoll::render() {

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->ChannelsSplit(e_totalLayers);
		m_mouseHoveringThisFrame = ImGui::IsWindowHovered();

		DrawTimelineGrid();

		DrawPhraseOnGrid();

		DrawPianoKeys();

		DrawScrollBars();

		drawList->ChannelsMerge();
	}

	constexpr float dividerLine = 0.01f;

	const float keyDensity           = 24.0f; // Number of keys per screen
	const float keyWidthRatio        =  0.1f; // Percentage horizontal view used by keyboard
	const float blackKeyWidthRatio   =  0.5f; // Horizontal scaling on black keys
	const float blackKeyHeightRatio  =  0.6f; // Vertical scaling on black keys

	const float subDivDensity		 =  2.0f; // How many measures of the smallest subdivision to display
	constexpr float bottomSquareSize = 30.0f;

	float blackKeyShiftRatios[12] = { 0.0f, 0.33f, 0.0f, 0.66f, 0.0f, 0.0f, 0.33f, 0.0f, 0.5f, 0.0f, 0.66f, 0.0f };

	__inline bool IsBlackKey(short keyNum) {
		
		short semitones = keyNum % 12;
		if (semitones == 0 || semitones == 2 || semitones == 5 || semitones == 7 || semitones == 10) {
			return true;
		}
		return false;
	}

	// Static variable definitions
	float PianoRoll::Key::m_keyHeight = 0.0f;
	float PianoRoll::Key::m_keyWidth  = 0.0f;

	char keyNameBuffer[3] = { 'C', '0', '\0' };

	void PianoRoll::Key::Render(const float& scrollingOffset) {

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImVec2 MinCorner = { m_xPos, m_yPos + scrollingOffset };
		float height     = m_keyHeight;
		float width      = m_keyWidth;
		ImU32 keyColor   = whiteKey;

		drawList->ChannelsSetCurrent(e_midBackground); // Set to channel 0 for white key, background element
		if (!m_isWhiteKey) {

			height *= blackKeyHeightRatio;
			width  *= blackKeyWidthRatio;
			drawList->ChannelsSetCurrent(e_midForeground); // Set to channel1 for black key, foreground element
			keyColor = blackKey;
		}

		ImVec2 MaxCorner = { MinCorner.x + width, MinCorner.y + height };

		drawList->AddRectFilled(MinCorner, MaxCorner, keyColor, 0.5f);
		if (m_isWhiteKey) {
			drawList->ChannelsSetCurrent(e_midForeground);
			drawList->AddLine(MinCorner, ImVec2(MinCorner.x + width, MinCorner.y), keyBorder, 3.0f);

			// Check for C keys to render text on top of
			if (m_noteValue % 12 == 0) {

				int keyNumber = (m_noteValue / 12) - 1;
				keyNameBuffer[1] = '0' + keyNumber;
				ImVec2 textPos = ImVec2(MinCorner.x + (width * 0.7f), MinCorner.y + (height * 0.5f));
				drawList->AddText(textPos, blackKey, keyNameBuffer);
			}
		}
	}

	void PianoRoll::DrawPianoKeys() {

		static ImVec2 lastWindowContentRegionSize = ImVec2(0.0f, 0.0f);
		constexpr unsigned numKeys = sizeof(m_keyArray) / sizeof(Key);

		const ImVec2& windowDim = ImGui::GetWindowContentRegionMax();
		if (lastWindowContentRegionSize.x != windowDim.x ||
			lastWindowContentRegionSize.y != windowDim.y) {
			
			// Recalculate the positions of all the keys (shouldn't happen often)
			const ImVec2& cursorPos = ImGui::GetCursorScreenPos();

			float keyWidth  = (windowDim.x - cursorPos.x) * keyWidthRatio;
			float keyHeight =  windowDim.y / keyDensity;
			float blackKeyHeight = keyHeight * blackKeyHeightRatio;

			Key::m_keyHeight = keyHeight;
			Key::m_keyWidth  = keyWidth;

			// Define key 127 (highest MIDI pitch) as being default top key with yCoord 0,
			// Add key width to each successive yCoordinate and account for black keys to generate
			// default coordinates for all piano keys to later be offset by the scroll bar
			int semitoneNumber = (numKeys - 1) % 12;
			float currentYPos = cursorPos.y;

			for (int i = numKeys - 1; i >= 0; --i) {

				Key& currentKey = m_keyArray[i];
				currentKey.m_xPos = cursorPos.x;
				currentKey.m_yPos = currentYPos;

				float keyOffset = blackKeyHeight * blackKeyShiftRatios[semitoneNumber];
				currentKey.m_yPos -= keyOffset; // Should be zero for white keys
				if (keyOffset == 0.0f) {

					currentKey.m_isWhiteKey = true;
					currentYPos += keyHeight;
				}

				// Decrement semitone information to keep correct offset lookups
				--semitoneNumber;
				if (semitoneNumber < 0) {
					semitoneNumber += 12; // Wrap to keep in acceptable range
				}
			}

		}

		for (int i = 0; i < numKeys; ++i) {

			m_keyArray[i].Render(m_scrollingY);
		}
	}

	const float lineThickness	 = 2.0f;
	const float timelineRatio	 = 0.05f;
	
	void PianoRoll::DrawTimelineGrid() {

		constexpr unsigned numKeys = sizeof(m_keyArray) / sizeof(Key);

		// Get initial render coordinates
		ImVec2 MinCorner = ImGui::GetCursorScreenPos();
		ImVec2 MaxCorner = MinCorner + ImGui::GetWindowContentRegionMax();

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Render the background of the window
		drawList->ChannelsSetCurrent(e_background);
		drawList->AddRectFilled(MinCorner, MaxCorner, background);
		
		// Render the timeline and center banner/ribbon
		float timelineHeight = timelineRatio * ImGui::GetWindowContentRegionMax().y;
		MaxCorner.y = timelineHeight + MinCorner.y;

		drawList->ChannelsSetCurrent(e_foreground);
		drawList->AddRectFilled(MinCorner, MaxCorner, timelineBackground);
		MinCorner.y += (timelineHeight / 2.0f);

		MaxCorner.x -= bottomSquareSize;
		MinCorner.x += Key::m_keyWidth;
		
		// Draw central timeline
		drawList->AddRectFilled(MinCorner, MaxCorner, timelineColor, 6.0f);
		MinCorner.y += (timelineHeight / 2.0f);

		MinCorner.x -= Key::m_keyWidth;

		// Save coordinates for an extra box to hide lines and keys
		ImVec2 coverMin = ImGui::GetCursorScreenPos();
		ImVec2 coverMax = MinCorner + ImVec2(Key::m_keyWidth, 0.0f);
		
		ImVec2 rightCoverMin = { MaxCorner.x, coverMin.y };
		ImVec2 rightCoverMax = { rightCoverMin.x + bottomSquareSize, MaxCorner.y };

		// IMPORTANT: Set new cursor screen postion so elements next elements will draw under the banner
		ImGui::SetCursorScreenPos(MinCorner);

		// Loop over the keys to draw the background lines and rectangles
		drawList->ChannelsSetCurrent(e_midBackground);
		for (int i = numKeys - 1; i >= 0; --i) {

			const Key& currentKey = m_keyArray[i];
			if (currentKey.m_isWhiteKey) {

				if (i > 0 && m_keyArray[i - 1].m_isWhiteKey) {

					MinCorner   = { currentKey.m_xPos, m_keyArray[i - 1].m_yPos + m_scrollingY };
					MaxCorner.y = MinCorner.y;
					drawList->AddLine(MinCorner, MaxCorner, blackBackground, 2.0f);
				}
			
				continue;
			}

			MinCorner   = { currentKey.m_xPos, currentKey.m_yPos + m_scrollingY};
			MaxCorner.y = currentKey.m_yPos + (currentKey.m_keyHeight * blackKeyHeightRatio) + m_scrollingY;

			drawList->AddRectFilled(MinCorner, MaxCorner, blackBackground);
		}

		// Next render vertical time lines across the grid to track time position
		unsigned measures = Phrase::_numMeasures + 1;
		unsigned subDivision = Phrase::_smallestSubdivision;

		MinCorner = ImGui::GetCursorScreenPos();
		MaxCorner = MinCorner + ImGui::GetWindowContentRegionMax();
		MinCorner.x += Key::m_keyWidth;

		// Calculate distance between vertical grid lines
		m_gridThickness = (MaxCorner.x - MinCorner.x) / (subDivision * subDivDensity + 1);
		MaxCorner.x = MinCorner.x;

		ImVec2 lineOffset = { lineThickness / 2.0f, 0.0f };
		ImVec2 scrollingOffset = { m_scrollingX, 0.0f };

		unsigned totalLines = measures * subDivision;
		int currMeasure = 1;
		char buffer[2];
		buffer[1] = '\0';
		for (unsigned i = 0; i < totalLines; ++i) {

			// Render the vertical lines
			drawList->ChannelsSetCurrent(e_midBackground);
			drawList->AddLine(MinCorner - lineOffset + scrollingOffset, MaxCorner - lineOffset + scrollingOffset, timelineNotchText, lineThickness);

			// Calculate the height of the notch on the timeline bar
			float notchBottom = ImGui::GetCursorScreenPos().y;
			float notchRatio = 0.25f;
			if (i % 16 == 0) { 
				notchRatio = 1.00f; 
			}
			else if (i % 4  == 0) { 
				notchRatio = 0.75f; 
			}
			else if (i % 2  == 0) { 
				notchRatio = 0.50f; 
			}

			ImVec2 bottom = { MinCorner.x, notchBottom };
			ImVec2 top    = { MinCorner.x, notchBottom - ((timelineHeight / 2.0f) * notchRatio) };

			// Render the notch
			drawList->ChannelsSetCurrent(e_foreground);
			drawList->AddLine(bottom - lineOffset + scrollingOffset, top - lineOffset + scrollingOffset, timelineNotchText, lineThickness);
			
			// If its the first line of a new measure, render the measure number
			if (notchRatio == 1.0f) {

				top.y = notchBottom - (timelineHeight / 2.0f) + 3.0f;
				top.x += 4.0f;
				buffer[0] = '0' + (currMeasure++);
				drawList->AddText(top + scrollingOffset, timelineNotchText, buffer);
			}

			MinCorner.x += m_gridThickness;
			MaxCorner.x += m_gridThickness;
		}

		// Draw final covering squares
		drawList->AddRectFilled(coverMin, coverMax, timelineBackground);
		drawList->AddRectFilled(rightCoverMin, rightCoverMax, timelineBackground);
	}

	void PianoRoll::DrawPhraseOnGrid() {

		Phrase* activePhrase = m_interface->m_getActivePhrase();

		if (!activePhrase) { return; }

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->ChannelsSetCurrent(e_midBackground);
		float currentXCoord = ImGui::GetCursorScreenPos().x + Key::m_keyWidth;

		char* rhythm  = activePhrase->_melodicRhythm;
		char* pitches = activePhrase->_melodicData;

		// Render chord notes first
		constexpr uint16_t chordBoxes = ChordRhythm; // Chords are hardcoded as quarter note length
		uint8_t previousPitch = 0;

		for (uint32_t i = 0, noteIdx = 0; i < activePhrase->_harmonicNotes; ++i) {

			// Setup variables
			uint8_t pitch = pitches[noteIdx];
			const Chord& currentChord = activePhrase->_harmonicData[i];

			// If there's no pitch on this quarter just use the previous one
			if (pitch == 0) { pitch = previousPitch; }

			// Determine root note
			uint8_t root = calculateRootNote(pitch, currentChord);

			// Calculate third and fifth above the root
			uint8_t third = root + NumeralSemitones[ChordNumeral::III];
			uint8_t fifth = root + NumeralSemitones[ChordNumeral::V];

			// Apply the chord type to the third and fifth
			applyChordType(currentChord._type, third, fifth);

			// Render each note on the timeline //
			renderNote(chordBoxes, currentXCoord, root, chordColor); // Render root 	
			renderNote(chordBoxes, currentXCoord, third, chordColor); // Render third
			renderNote(chordBoxes, currentXCoord, fifth, chordColor); // Render fifth

			// Setup for next loop iteration
			previousPitch = pitch;
			noteIdx += ChordRhythm;
			currentXCoord += chordBoxes * m_gridThickness;
		}

		// Reset X coordinate position
		currentXCoord = ImGui::GetCursorScreenPos().x + Key::m_keyWidth;

		// Render melodic line
		uint32_t numNotes = activePhrase->_melodicNotes;

		for (uint32_t i = 0, noteIndex = 0; i < numNotes; ++i) {

			const uint16_t numBoxes   = rhythm[noteIndex];
			const uint8_t pitchNumber = pitches[noteIndex];

			// Don't render a rest note
			if (pitchNumber != 0) {

				renderNote(numBoxes, currentXCoord, pitchNumber, noteColor);
			}

			noteIndex += numBoxes;
			currentXCoord += numBoxes * m_gridThickness;
		}
	}

	constexpr int numWhiteKeys = 75; // 0-119 has 70 white keys (7 per octave, plus 5 between 120-127)

	constexpr float ScrollInternal = 5.0f;

	constexpr float ScrollBarWide = bottomSquareSize - 2.0f * ScrollInternal;
	constexpr float ScrollBarTall = bottomSquareSize - 3.0f * ScrollInternal;

	void PianoRoll::DrawScrollBars() {

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->ChannelsSetCurrent(e_foreground);

		float vertScrollOut = Key::m_keyHeight * (numWhiteKeys - 20) * -1.0f;
		
		float maxSquares = static_cast<float>(Phrase::_smallestSubdivision * Phrase::_numMeasures);
	
		float horizScrollOut = m_gridThickness * (maxSquares - (Phrase::_smallestSubdivision * subDivDensity)) * -1.0f; 

		ImVec2 SliderInternal = { ScrollInternal, ScrollInternal };
		
		// Render the vertical slider bar
		ImVec2 VertTop = ImGui::GetCursorScreenPos();
		VertTop.x = (ImGui::GetWindowContentRegionMax().x - bottomSquareSize) + VertTop.x;

		ImVec2 VertBottom = ImGui::GetWindowContentRegionMax() + ImGui::GetCursorScreenPos();
		VertBottom.y -= (bottomSquareSize + timelineRatio * ImGui::GetWindowContentRegionMax().y);

		drawList->AddRectFilled(VertTop, VertBottom, ScrollBackground);
		drawList->AddRectFilled(VertTop + SliderInternal, VertBottom - SliderInternal, ScrollTrough, 5.0f);

		// To calculate coordinates multiply by total area and add to the min corner y value (x should just be min corner)
		// To convert back just use the value you find position of element in range and divide by max value
		
		static ImVec2 VertScrollRatio = { 0.0f, 0.0f };
		float vertRange = (VertBottom.y - SliderInternal.y) - (VertTop.y + SliderInternal.y);
		float positionAddition = VertScrollRatio.y * vertRange;
		ImVec2 barMin = VertTop + SliderInternal;
		barMin.y += positionAddition;
		drawList->AddRectFilled(barMin, barMin + ImVec2(ScrollBarWide, ScrollBarTall), ScrollBar, 5.0f);

		ImVec2 SavedCursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(barMin);
		ImGui::InvisibleButton("VertScroll", ImVec2(ScrollBarWide, ScrollBarTall));

		if (ImGui::IsItemActive() && ImGui::IsMouseDown(0)) {

			const ImVec2& mousePos = ImGui::GetMousePos();
			barMin.y = mousePos.y;
		}

		float wheelScroll = ImGui::GetIO().MouseWheel;

		if (m_mouseHoveringThisFrame && wheelScroll != 0.0f && !(ImGui::GetIO().KeyShift)) {
			barMin.y += ImGui::GetFontSize() * 1.0f * -wheelScroll;
		}

		if (barMin.y < VertTop.y + ScrollInternal) {
			barMin.y = VertTop.y + ScrollInternal;
		}
		if (barMin.y > VertBottom.y - ScrollInternal - ScrollBarTall) {
			barMin.y = VertBottom.y - ScrollInternal - ScrollBarTall;
		}

		ImGui::SetCursorScreenPos(SavedCursorPos);

		// Calculate vertical scrolling value based off new scroll bar position
		float distance = barMin.y - (VertTop.y + SliderInternal.y);
		VertScrollRatio.y = distance / vertRange;
		m_scrollingY = VertScrollRatio.y * vertScrollOut;
		
		// Render the horizontal slider bar
		ImVec2 HorizLeft = ImGui::GetCursorScreenPos();
		HorizLeft.x += Key::m_keyWidth;
		HorizLeft.y += (1.0f - timelineRatio) * ImGui::GetWindowContentRegionMax().y - bottomSquareSize;

		ImVec2 HorizRight = HorizLeft;
		HorizRight.y += bottomSquareSize;
		HorizRight.x += ImGui::GetWindowContentRegionMax().x - bottomSquareSize - Key::m_keyWidth;
		
		drawList->AddRectFilled(HorizLeft, HorizRight, ScrollBackground);
		drawList->AddRectFilled(HorizLeft + SliderInternal, HorizRight - SliderInternal, ScrollTrough, 5.0f);

		static ImVec2 HorizScrollRatio = { 0.0f, 0.0f };
		float horizRange = (HorizRight.x - SliderInternal.x) - (HorizLeft.x + SliderInternal.x);
		positionAddition = HorizScrollRatio.x * horizRange;
		barMin = HorizLeft + SliderInternal;
		barMin.x += positionAddition;
		drawList->AddRectFilled(barMin, barMin + ImVec2(ScrollBarTall, ScrollBarWide), ScrollBar, 5.0f);

		SavedCursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(barMin);
		ImGui::InvisibleButton("HorizScroll", ImVec2(ScrollBarTall, ScrollBarWide));

		if (ImGui::IsItemActive() && ImGui::IsMouseDown(0)) {

			const ImVec2& mousePos = ImGui::GetMousePos();
			barMin.x = mousePos.x;
		}

		if (m_mouseHoveringThisFrame && wheelScroll != 0.0f && ImGui::GetIO().KeyShift) {
			barMin.x += ImGui::GetFontSize() * 1.5f * -wheelScroll;
		}


		if (barMin.x < HorizLeft.x + ScrollInternal) {
			barMin.x = HorizLeft.x + ScrollInternal;
		}
		if (barMin.x > HorizRight.x - ScrollInternal - ScrollBarTall) {
			barMin.x = HorizRight.x - ScrollInternal - ScrollBarTall;
		}

		ImGui::SetCursorScreenPos(SavedCursorPos);

		// Calculate horizontal scrolling value based off new scroll bar position
		distance = (barMin.x - (HorizLeft.x + SliderInternal.x));
		HorizScrollRatio.x = distance / horizRange;
		m_scrollingX = HorizScrollRatio.x * horizScrollOut;

		// Render extra square to hide some grid lines
		ImVec2 minCover = { HorizRight.x, HorizLeft.y };
		drawList->AddRectFilled(minCover, minCover + ImVec2(bottomSquareSize, bottomSquareSize), timelineBackground);
	}

	float PianoRoll::GetYCoordOfKey(short keyNumber) const {

		if (keyNumber < 0 || keyNumber > 127) {
			return 0.0f;
		}

		Key keyCandidate = m_keyArray[keyNumber];
		if (keyCandidate.m_isWhiteKey) {

			// Fixes an issue with consequtive white notes rendering weirdly
			if (keyNumber < 127 && m_keyArray[keyNumber + 1].m_isWhiteKey) {
				// Intentionally blank
			}
			else if (keyNumber < 127) {

				keyCandidate = m_keyArray[keyNumber + 1];
				return keyCandidate.m_yPos + Key::m_keyHeight * blackKeyHeightRatio;
			}
		}

		return keyCandidate.m_yPos;
	}

	void PianoRoll::renderNote(const short numBoxes, const float xCoordinate, 
							   const uint8_t pitch, const ImU32 barColor = noteBorderColor) const {

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const     float noteHeight = Key::m_keyHeight * blackKeyHeightRatio;

		constexpr float lineIndent = 6.0f;
		const     float lineHeight = 0.7f * noteHeight;

		ImVec2 noteDim = { numBoxes * m_gridThickness, noteHeight };
		ImVec2 noteCorner = { xCoordinate + m_scrollingX, GetYCoordOfKey(pitch) + m_scrollingY };

		drawList->AddRectFilled(noteCorner, noteCorner + noteDim, barColor, 2.5f);
		drawList->AddRect(noteCorner, noteCorner + noteDim, noteBorderColor, 2.5f, 15, 1.5f);

		ImVec2 lineTop = { noteCorner.x + lineIndent, noteCorner.y + (noteHeight - lineHeight) / 2.0f };
		ImVec2 lineBottom = { lineTop.x, lineTop.y + lineHeight };
		drawList->AddLine(lineTop, lineBottom, noteLineColor, 2.0f);
	}

} // namespace Genetics