#pragma once
#include "../framework/glmmath.h"
#include "../framework/error.h"

struct Vertex
{

	glm::vec2 pos;
	glm::vec3 color;

	Vertex() = default;
	Vertex(const glm::vec2& pos, const glm::vec3& color) 
		:
	pos(pos),
	color(color)
	{}

	/// \brief performs a linear interpolation between two vertices
	/// \param v1 left vertex
	/// \param v2 right vertex
	/// \param l amount [0,1]
	/// \return interpolated vertex
	static Vertex lerp(const Vertex& v1, const Vertex& v2, float l)
	{
		dassert(l >= 0.0f);
		dassert(l <= 1.0f);

		Vertex v;
		v.pos = v1.pos + l * (v2.pos - v1.pos);
		v.color = v1.color + l * (v2.color - v1.color);

		return v;
	}
};