#pragma once
#include <cstdint>
#include <cmath>

constexpr float M_2PI = 6.28318530717958647692f;

class Pseudorandom {
public:
	Pseudorandom(uint32_t seed) : m_seed{ seed } {}

	void setSeed(uint32_t seed) { m_seed = seed; };

	//- Generiert eine zufällig (gleichverteile determinische) Zufallszahl für den Index (x,y)
	glm::vec2 getRandomDirection(const uint32_t x,const uint32_t y) {
		//Get a random number between [0,1]
		float num = pcg3d(x, y);
		//Convert to [0, 2PI]
		num *= M_2PI;
		//Use Polar coordinates to generate a directional vector with lenght 1
		return glm::vec2(glm::cos(num), glm::sin(num));
	}

private:
	uint32_t m_seed = 758385u;	//Some random number if seed is not set

	//Version of pcg3D that outputs one pseudorandom number between 0 and 1.
	//Based on https://jcgt.org/published/0009/03/02/ by Mark Jarzynski and Marc Olano.
	float pcg3d(const uint32_t x, const uint32_t y) {
		//Pcg3d algorithm
		uint32_t v[3] = { x,y,m_seed };
	
		v[0] = v[0] * 1664525u + 1013904223u;
		v[1] = v[1] * 1664525u + 1013904223u;
		v[2] = v[2] * 1664525u + 1013904223u;

		v[0] += v[1] * v[2];
		v[1] += v[2] * v[0];
		v[2] += v[0] * v[1];

		v[0] ^= v[0] >> 16u;
		v[1] ^= v[1] >> 16u;
		v[2] ^= v[2] >> 16u;

		v[0] += v[1] * v[2];
		//v[1] += v[2] * v[0];	//Not needed
		//v[2] += v[0] * v[1];	//Not needed

		//Convert to [0,1]
		return float(v[0]) / float(0xffffffffu);
	}
};