#pragma once
#include <vector>
#include <string>
#include "../framework/VertexArrayObject.h"
#include "../framework/Window.h"
#include "../framework/Uniform.h"
#include "../framework/VertexBuffer.h"
#include "../framework/IndexBuffer.h"
#include "../framework/Program.h"

constexpr int NUM_OPTIONS = 5;
constexpr float STEP_PERLIN_RES = 0.25f;
constexpr float STEP_HEIGHT_SCALE = 0.5f;
constexpr float STEP_PERSISTENCE = 0.01f;

class Terrain
{
public:
	Terrain(int resolution, float perlinRes, int octaves, float persistence, float heightScale);

	void draw(const glm::mat4& transform);

	void handleUI(Window::Key key, bool useWASD = false);
	void outOptionString();
	float perlinOctaves(float x, float z);
	float getHeight(int x, int z) const;
	int getResolution() const;
	glm::mat4 getBackupTransformation() const;

private:
	void regenerate();
	void setHeight(int x, int z, float height);
	int calcIndex(int x, int z) const;
	void generate();
	float perlinNoise(float x, float z);
	void createVertexBuffer();
	void switchOptions(bool increase);
	void changeOptionValue(bool increase);

private:
	// Option Strings
	const std::string m_optionStrings[5] = {
		"Terrain Height Scale",
		"Terrain Resolution",
		"Perlin Resolution",
		"Perlin Octaves",
		"Perlin Persistence"
	};

	// Optionen
	int m_selectedOption = 0;
	int m_resolution;
	float m_perlinRes;
	int m_octaves;
	float m_persistence;
	float m_heightScale;

	std::vector<float> m_height;
	VertexBuffer<glm::vec3> m_positions;
	IndexBuffer m_indices;
	VertexArrayObject m_vao;
	Program m_program;

	// Uniform für die Transformationsmatrix hinzufügen
	Uniform<glm::mat4> m_uniformTransform;
};
