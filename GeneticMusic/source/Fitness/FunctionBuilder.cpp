// Morgen Hyde
#include "Fitness/FunctionBuilder.h"

#include <algorithm>

#ifdef _DEBUG 
#include <iostream>
#endif

#define XOR(a, b) (!(a && b) && (a || b))

namespace Genetics {

	Function::Function(const std::string& functionName, FunctionID id)
		: m_functionName(functionName), m_functionID(id) {
	}

	Function::Function(Function&& rhs)
		: m_functionName(std::move(rhs.m_functionName)), 
		  m_functionID(std::move(rhs.m_functionID)),
		  m_vertexList(std::move(rhs.m_vertexList)) {
	}

	Function& Function::operator=(const Function& rhs) {

		m_vertexList.clear();
		for (const Vertex& vert : rhs.m_vertexList) {

			Vertex vertCopy(vert);
			m_vertexList.push_back(vertCopy);
		}

		return *this;
	}

	Function::~Function() {

		// Intentionally blank
	}

	// Get output value from some input //

	float Function::operator()(short inputVal) const {

		// Handle inputs outside the function bounds
		if (inputVal < m_vertexList.front()._xPos) {
			return 0.0f;
		}
	
		if (inputVal > m_vertexList.back()._xPos) {
			return 0.0f;
		}
		
		// Loop over all vertices
		uint16_t numVertices = static_cast<uint16_t>(m_vertexList.size());
		for (uint16_t i = 1; i < numVertices; ++i) {

			const Vertex& current = m_vertexList[i];
			// Check if the input is on a vertex
			if (current._xPos == inputVal) {

				// Return the current vertex's value
				return current._yPos;
			}
			else if (current._xPos > inputVal) {
				
				// Get the previous vertex
				const Vertex& previous = m_vertexList[i - 1];

				// Find the slope between the two vertices
				float slope = calculateSlope(previous, current);

				// Calculate output value
				return (inputVal - previous._xPos) * slope + previous._yPos;
			}
		}

		// Value was not found return 0.0f;
		return 0.0f;
	}


	// Add new elements //

	void Function::addVertex(const Vertex& vert) {
		
		VertexIterator insertPos = m_vertexList.begin();
		VertexIterator endPos    = m_vertexList.end();

		Vertex insertVert(vert);

		while (insertPos != endPos) {

			// Vector is sorted so first element larger than it should be insertion point
			if ((*insertPos) > vert) {

				// Insert and return, all done
				m_vertexList.insert(insertPos, std::move(insertVert));
				return;
			}

			++insertPos;
		}

		// If we get here we're the biggest element so just push back
		m_vertexList.push_back(std::move(insertVert));
	}

	void Function::addVertex(short xPos, float yPos) {

		VertexIterator insertPos = m_vertexList.begin();
		VertexIterator endPos = m_vertexList.end();

		Vertex insertVert(xPos, yPos);

		while (insertPos != endPos) {

			// Check the left and right sides for a place to put this vert
			if (insertPos->_xPos == insertVert._xPos) {

				// If the insert point is the front of the list or the next smallest element is
				// not adjacent, its safe to shift this placement left 1
				if (insertPos == m_vertexList.begin() || (insertPos - 1)->_xPos < xPos - 1) {

					insertVert._xPos -= 1;
					m_vertexList.insert(insertPos, std::move(insertVert));
				}
				// If the vertex to the right is more than 1 unit away, safe to shift placement
				// by one to the right
				else if (insertPos + 1 != endPos && (insertPos + 1)->_xPos > xPos + 1) {

					insertVert._xPos += 1;
					m_vertexList.insert(++insertPos, std::move(insertVert));
				}
				// There's no space for a vertex to be placed so do some error handling thing(?)
				else {

					// Error Handling

				}

				return;
			}

			// Vector is sorted so first element larger than it should be insertion point
			if ((*insertPos) > insertVert) {

				// Insert and return, all done
				m_vertexList.insert(insertPos, std::move(insertVert));
				return;
			}

			++insertPos;
		}

		// If we get here we're the biggest element so just push back
		m_vertexList.push_back(std::move(insertVert));
	}

	// Remove existing elements //

	void Function::removeVertex(short xPos, float yPos) {

		VertexIterator position = findVertex(m_vertexList.begin(), m_vertexList.end(), xPos, yPos);
		if (position != m_vertexList.end()) {

			// This shouldn't require a re-sort
			m_vertexList.erase(position);
		}
	}


	// Edit existing elements //

	void Function::moveVertexXPos(short oldX, short newX) {

		VertexIterator position = findVertex(m_vertexList.begin(), m_vertexList.end(), oldX);
		if (position != m_vertexList.end()) {

			// Find the largest x smaller than the old x, (position by default is first)
			short minRange = std::numeric_limits<short>::min();
			if (position != m_vertexList.begin()) {
				minRange = (position - 1)->_xPos + 1;
				newX = std::max(minRange, newX);
			}

			// Find the smallest x larger than the old x
			while (position != m_vertexList.end() && position->_xPos == oldX) {
				++position;
			}
			--position;

			short maxRange = std::numeric_limits<short>::max();
			if ((position + 1) != m_vertexList.end()) {
				maxRange = (position + 1)->_xPos - 1;
				newX = std::min(maxRange, newX);
			}
			short xCoordinate = std::max(std::min(maxRange, newX), minRange);
	
			position->_xPos = xCoordinate;

			// Because we clip the vertex into a valid range between it's neighbors
			// the vector -should- be guaranteed to stay sorted
			//std::sort(m_vertexList.begin(), m_vertexList.end(), VertexSorter());
		}
	}

	void Function::moveVertexYPos(short vertX, float oldY, float newY) {

		VertexIterator position = findVertex(m_vertexList.begin(), m_vertexList.end(), vertX, oldY);
		if (position != m_vertexList.end()) {
			position->_yPos = newY;
		}
		//std::sort(m_vertexList.begin(), m_vertexList.end(), VertexSorter());
	}

	// Access elements //

	const std::vector<Vertex>& Function::getVertices() const {

		return m_vertexList;
	}

	ConstVertexIterator Function::findVertex(short xValue) const {

		return findVertex(m_vertexList.begin(), m_vertexList.end(), xValue);
	}

	VertexIterator Function::findVertex(short xValue) {
		
		return findVertex(m_vertexList.begin(), m_vertexList.end(), xValue);
	}

	ConstVertexIterator Function::begin() const {

		return m_vertexList.begin();
	}

	VertexIterator Function::begin() {
		
		return m_vertexList.begin();
	}

	ConstVertexIterator Function::end() const {

		return m_vertexList.end();
	}

	VertexIterator Function::end() {

		return m_vertexList.end();
	}

	// Identifiers //

	const std::string& Function::getName() const {

		return m_functionName;
	}

	void Function::setName(const std::string& name) {

		m_functionName = name;
	}

	FunctionID Function::getFunctionID() const {

		return m_functionID;
	}

	// Private member functions //

	float Function::calculateSlope(const Vertex& vertMin, const Vertex& vertMax) const {

		return (vertMax._yPos - vertMin._yPos) / (vertMax._xPos - vertMin._xPos);
	}

	ConstVertexIterator Function::findVertex(
		ConstVertexIterator begin, ConstVertexIterator end, short xPos, float yPos) const {

		ConstVertexIterator finder = begin;
		while (finder != end) {
			
			if (finder->_xPos == xPos) { 

				// Allegedly this works...
				if (yPos < 0.0f || finder->_yPos == yPos) { return finder; } 
			}
			++finder;
		}

		return finder;
	}

	VertexIterator Function::findVertex(VertexIterator begin, VertexIterator end, short xPos, float yPos) {

		VertexIterator finder = begin;
		while (finder != end) {

			if (finder->_xPos == xPos) {

				// Allegedly this works...
				if (yPos < 0.0f || finder->_yPos == yPos) { return finder; }
			}
			++finder;
		}

		return finder;
	}

	// Vertex Member functions //

	uint32_t Vertex::_vertCount = 0;

	Vertex::Vertex(short xPos, float yPos)
		: _xPos(xPos), _yPos(yPos), _vertID(_vertCount++) {

	}

	Vertex::Vertex(const Vertex& rhs)
		: _xPos(rhs._xPos), _yPos(rhs._yPos), _vertID(_vertCount++) {
	}

	Vertex& Vertex::operator=(const Vertex& rhs) {

		// Check for self-assignment
		if (&rhs == this) {
			return *this;
		}

		_xPos = rhs._xPos;
		_yPos = rhs._yPos;

		return *this;
	}

	Vertex::Vertex(const Vertex&& rhs) noexcept
		: _xPos(rhs._xPos), _yPos(rhs._yPos),  _vertID(rhs._vertID) {

	}

	Vertex& Vertex::operator=(const Vertex&& rhs) noexcept {

		if (&rhs == this) {
			return *this;
		}

		_xPos = rhs._xPos;
		_yPos = rhs._yPos;
		_vertID = rhs._vertID;

		return *this;
	}

	bool operator>(const Vertex& lhs, const Vertex& rhs) {

		if (lhs._xPos > rhs._xPos) {

			return true;
		}
		else if (lhs._xPos == rhs._xPos) {

			return lhs._yPos < rhs._yPos;
		}
		else {

			return false;
		}
	}

	bool operator==(const Vertex& lhs, short xPosition) {

		if (lhs._xPos == xPosition) { return true; }
		return false;
	}

} // namespace Genetics