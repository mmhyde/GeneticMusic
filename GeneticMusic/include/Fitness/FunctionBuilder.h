// Morgen Hyde
#pragma once

#include "RuleTypes.h"

#include <vector>
#include <string>

namespace Genetics {

	struct Vertex;

	typedef std::vector<Vertex>::iterator VertexIterator;
	typedef std::vector<Vertex>::const_iterator ConstVertexIterator;

	class Function {

	public:
		// Constructor / Destructor //

		Function(const std::string& functionName, FunctionID id);

		// We don't want functions getting created with duplicate IDs or names
		Function(const Function& rhs) = delete;
		
		// However assigning to an already named and ID'd function is OK
		Function& operator=(const Function& rhs);

		// Move constructor is OK too because it's stealing from the other object
		Function(Function&& rhs);

		~Function();

		// Get output value from some input //

		float operator()(short inputVal) const;

		// Add new elements //

		void addVertex(const Vertex& vert);
		void addVertex(short xPos = 0, float yPos = 0.0f);

		// Remove existing elements //

		void removeVertex(short xPos, float yPos);

		// Edit existing elements //

		void moveVertexXPos(short oldX, short newX);
		void moveVertexYPos(short vertX, float vertY, float newY);

		// Access elements //

		const std::vector<Vertex>& getVertices() const;

		ConstVertexIterator findVertex(short xValue) const;
		VertexIterator findVertex(short xValue);

		ConstVertexIterator begin() const;
		VertexIterator begin();

		ConstVertexIterator end() const;
		VertexIterator end();

		// Identifiers //

		const std::string& getName() const;
		void setName(const std::string& name);

		FunctionID getFunctionID() const;

	private:
		float calculateSlope(const Vertex& vertMin, const Vertex& vertMax) const;

		ConstVertexIterator findVertex(ConstVertexIterator begin, ConstVertexIterator end, short xPos, float yPos = -1.0f) const;
		VertexIterator findVertex(VertexIterator begin, VertexIterator end, short xPos, float yPos = -1.0f);

		std::string m_functionName;
		FunctionID m_functionID;

		std::vector<Vertex> m_vertexList;
	};

	struct Vertex {

		Vertex(short xPos = 0, float yPos = 0.0f);

		explicit Vertex(const Vertex& rhs);
		Vertex& operator=(const Vertex& rhs);

		Vertex(const Vertex&& rhs) noexcept; // noexcept required for vector to call move
		Vertex& operator=(const Vertex&& rhs) noexcept;
		
		short _xPos;
		float _yPos;
	
		uint32_t _vertID;
		static uint32_t _vertCount;
	};

	bool operator>(const Vertex& lhs, const Vertex& rhs);

	bool operator==(const Vertex& lhs, short xPosition);

	// Helpers for using STL algorithms for sorting
	struct VertexSorter {
		bool operator()(const Vertex& lhs, const Vertex& rhs) {

			if (lhs._xPos < rhs._xPos) {
				
				return true;
			}
			else if (lhs._xPos == rhs._xPos) {

				return lhs._yPos > rhs._yPos;
			}
			else {

				return false;
			}
		}
	};

} // namespace Genetics