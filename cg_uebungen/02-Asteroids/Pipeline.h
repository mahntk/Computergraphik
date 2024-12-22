#pragma once
#include "../framework/Window.h"
#include "Vertex.h"
#include <memory>

class Pipeline
{
public:
	/// \brief initializes the pipeline
	/// \param window image destination window
	Pipeline(Window& window);

	/// \brief should be called before using the pipeline for a new frame
	/// adjusts the screen space transformation and other stuff
	void begin();

	/// \brief draws a triangle
	/// \param v1 triangle edge
	/// \param v2 triangle edge
	/// \param v3 triangle edge
	void drawTriangle(Vertex v1, Vertex v2, Vertex v3);

	/// \brief draws a list of triangles
	/// \param vertices list of triangle vertices (multiple of three)
	void drawTriangleList(const std::vector<Vertex>& vertices);

	/// \brief sets translation for the vertex shader
	void setVertexTranslation(const glm::vec2& translation);

	/// \brief sets the rotation for the vertex shader
	void setVertexRotation(float angle);

	/// \brief sets scaling for the vertex shader
	void setVertexScale(float scale);

	/// \brief sets color scaling for the fragment shader
	void setFragmentScale(float scale);
private:
	
	/// \brief draws a triangle (vertices should be inside the canonical volume)
	/// \param vertices array with the three triangle vertices
	void drawClippedTriangle(std::array<Vertex, 3> vertices);

	/// \brief draws the scanline with interpolated vertices from right.pos.x to left.pos.x
	/// \param y the height of the scanline
	/// \param left the left (x-axis) vertex
	/// \param right the right (x-axis) vertex
	void scanLine(int y, const Vertex& left, const Vertex& right);
	
	/// \brief applies the vertex shader
	/// \param vertex vertex that should be transformed
	void shadeVertex(Vertex& vertex);

	/// \brief returns the pixel color
	/// \param vertex vertex information for the fragment
	glm::vec3 shadeFragment(const Vertex& vertex);

	/// \brief clips a line to a specific axis (x,y or z axis)
	/// \param axis determines which axis should be clipped (0 = x-axis, 1 = y-axis...)
	/// \return true if the polygon is still visible
	bool clipPolygonAxis(int axis);

	/// \brief clips a line to a specific axis side (positive x, negative x...)
	/// \param in list of vertices forming a polygon (input)
	/// \param out list of vertices forming a polygon (output)
	/// \param axis determines which axis should be clipped (0 = x-axis, 1 = y-axis...)
	/// \param side determines the side which should be used for clipping. Either 1.0f or -1.0f
	/// \return true if the line is still visible
	static void clipPolygonComponent(const std::vector<Vertex>& in, std::vector<Vertex>& out, int axis, float side);
private:
	Window& m_window;
	float m_width= 0.0f;
	float m_height = 0.0f;
	glm::vec2 m_translation;
	float m_rotationSine = 0.0f;
	float m_rotationCosine = 1.0f;
	float m_scale = 1.0f;
	float m_colorScale = 1.0f;

	std::vector<Vertex> m_triangleList;
	std::vector<Vertex> m_triangleTmpList;
};
