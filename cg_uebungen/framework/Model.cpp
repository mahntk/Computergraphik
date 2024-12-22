#include "Model.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include "../dependencies/tiny_obj_loader.h"
//#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "error.h"

//Define a hash for the vertex object
namespace std {
	template<> struct hash<Model::Vertex> {
		size_t operator()(Model::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec3>()(vertex.normals) << 1)) >> 1)^
				(hash<glm::vec2>()(vertex.texCoords) << 1);
		}
	};
}

Model::Model(const std::string path) {
	loadFromFile(path);
}

void Model::loadFromFile(const std::string path,const bool loadTextureCoordinates, const bool calcTangentAndBitangent) {

	m_positions.clear();
	m_normals.clear();
	m_indices.clear();

	tinyobj::ObjReaderConfig reader_config;
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(path, reader_config)) {
		throw std::runtime_error("ERROR: ModelLoder: Error while reading file; " + reader.Error());
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			//Skip every non triangle
			if (fv != 3) {
				std::cerr << "Warning: ModelLoader: Object contains face with more or less than 3 vertices; Model: " << path;
				index_offset += fv;
				continue;
			}

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				auto idx = shapes[s].mesh.indices[index_offset + v];
				Vertex vert{};
				//Fill normals
				for (uint32_t i = 0; i < 3; i++)
					vert.position[i] = attrib.vertices[3 * size_t(idx.vertex_index) + i];
				//Fill normals
				if (idx.normal_index >= 0) {
					for (uint32_t i = 0; i < 3; i++)
						vert.normals[i] = attrib.normals[3 * size_t(idx.normal_index) + i];
				}
				else { //Fallback
					vert.normals = glm::vec3(0, 1, 0);
				}
				//Fill Tex Coords
				if (idx.texcoord_index >= 0 && loadTextureCoordinates) {
					for (uint32_t i = 0; i < 2; i++)
						vert.texCoords[i] = attrib.texcoords[2 * size_t(idx.texcoord_index) + i];
				}
				else {
					vert.texCoords = glm::vec2(0, 0);
				}

				//Check if vertex was used before
				if (uniqueVertices.count(vert) == 0) {
					uniqueVertices[vert] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vert);
				}

				indices.push_back(uniqueVertices[vert]);
				
			}
			index_offset += fv;
		}
	}

	//Calculate Tangent and Bitangent for the model
	
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	if (calcTangentAndBitangent && loadTextureCoordinates) {
		tangents.resize(vertices.size());
		bitangents.resize(vertices.size());

		//Make sure it is initialized to zero
		std::fill(tangents.begin(), tangents.end(), glm::vec3(0));
		std::fill(bitangents.begin(), bitangents.end(), glm::vec3(0));

		for (size_t i = 0; i < indices.size(); i += 3) {
			Vertex v0 = vertices[indices[i]];
			Vertex v1 = vertices[indices[i + 1]];
			Vertex v2 = vertices[indices[i + 2]];

			glm::vec3 deltaPos1 = v1.position - v0.position;
			glm::vec3 deltaPos2 = v2.position - v0.position;

			// UV delta
			glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
			glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			for (size_t j = 0; j < 3; j++) {
				tangents[indices[i + j]] += tangent;
				bitangents[indices[i + j]] += bitangent;
			}
		}
	}

	//Fill our openGL position and normal buffer
	for (size_t i = 0; i < vertices.size(); i++) {
		m_positions.addVertex(vertices[i].position);
		m_normals.addVertex(vertices[i].normals);
		if (loadTextureCoordinates)
			m_texCoords.addVertex(vertices[i].texCoords);
		if (calcTangentAndBitangent && loadTextureCoordinates) {
			m_tangent.addVertex(glm::normalize(tangents[i]));
			m_bitangent.addVertex(glm::normalize(bitangents[i]));
		}
	}

	for (size_t i = 0; i < indices.size(); i++) {
		m_indices.addIndex(indices[i]);
	}

	//Upload to gpu and attach to vao
	m_positions.uploadToGpu();
	m_normals.uploadToGpu();
	if (loadTextureCoordinates) m_texCoords.uploadToGpu();
	if (calcTangentAndBitangent && loadTextureCoordinates) {
		m_tangent.uploadToGpu();
		m_bitangent.uploadToGpu();
	}

	m_indices.uploadToGpu();

	m_vao.attachBuffer(0, m_positions);
	m_vao.attachBuffer(1, m_normals);
	if (loadTextureCoordinates)
		m_vao.attachBuffer(2, m_texCoords);
	if (calcTangentAndBitangent && loadTextureCoordinates) {
		m_vao.attachBuffer(3, m_tangent);
		m_vao.attachBuffer(4, m_bitangent);
	}
}

void Model::draw(Program& program) {
	m_vao.drawElements(GL_TRIANGLES, program, m_indices);
}