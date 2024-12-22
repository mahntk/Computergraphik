#pragma once
#include <string>
#include "glmmath.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArrayObject.h"
#include "Program.h"

//Small model loader which can load positions and normals from objects
class Model
{
public:
	Model() {}
	Model(const std::string path);

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normals;
		glm::vec2 texCoords;

		bool operator==(const Vertex& other) const {
			return position == other.position && normals == other.normals && texCoords == other.texCoords;
		}
	};

	void loadFromFile(const std::string path, const bool loadTextureCoordinates = false, const bool calcTangentAndBitangent = false);

	void draw(Program& program);

private:

	VertexBuffer<glm::vec3> m_positions;
	VertexBuffer<glm::vec3> m_normals;
	VertexBuffer<glm::vec2> m_texCoords;
	VertexBuffer<glm::vec3> m_tangent;
	VertexBuffer<glm::vec3> m_bitangent;
	IndexBuffer m_indices;
	VertexArrayObject m_vao;
};
