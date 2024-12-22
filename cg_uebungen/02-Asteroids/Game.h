#pragma once
#include "../framework/glmmath.h"
#include "Vertex.h"
#include <vector>
#include "Pipeline.h"
#include "../framework/Window.h"
#include <deque>
#include <random>

class Game
{
	struct AsteroidData
	{
		glm::vec2 position;
		glm::vec2 velocity;
		float rotation;
		bool alive = true;
	};
	struct MissleData
	{
		glm::vec2 position;
		glm::vec2 velocity;
		float rotation;
		float lifetime;
	};
public:
	Game(Window& window);
	
	/// \brief draw code for the game 
	/// \param gfx graphics pipeline
	void draw(Pipeline& gfx) const;

	/// \brief game logic (collission, updates for ship, missles and asteroids)
	/// \param dt time delta in milliseconds
	void update(float dt);

	/// \brief returns the current level
	int getLevel() const { return m_currentLevel; }

	/// \brief returns the game score
	int getScore() const { return m_score; }
private:
	/// \brief advance to next level
	void nextLevel();

	/// \brief generates mesh that is used for the ship
	void generateShipMesh();

	/// \brief generates mesh that is used for the ships fire
	void generateFireMesh();

	/// \brief generates mesh that is used for the ships missle
	void generateMissleMesh();

	/// \brief clips and object into the [-1, 1] plane
	void clipObject(glm::vec2& position, const glm::vec2& velocity, float radius) const;

	/// \brief generates random velocity for a new asteroid
	glm::vec2 generateAsteroidVelocity();

	/// \brief erases all asteroids with alive = false status
	void eraseDeadAsteroids(std::vector<AsteroidData>& vec);
private:
	// mersenne twister (random number generator)
	std::mt19937 m_twister;

	// mesh data
	std::vector<Vertex> m_bigAsteroidMesh;
	std::vector<Vertex> m_midAsteroidMesh;
	std::vector<Vertex> m_smallAsteroidMesh;
	std::vector<Vertex> m_shipMesh;
	std::vector<Vertex> m_shipFireMesh;
	std::vector<Vertex> m_missleMesh;
	
	// entity scalings
	const float m_bigAsteroidRadius = 0.1f;
	const float m_midAsteroidRadius = 0.06f;
	const float m_smallAsteroidRadius = 0.035f;
	const float m_shipRadius = 0.05f;
	const float m_missleRadius = 0.01f;

	// active asteroids
	std::vector<AsteroidData> m_bigAsteroids;
	std::vector<AsteroidData> m_midAsteroids;
	std::vector<AsteroidData> m_smallAsteroids;

	// active missles
	std::deque<MissleData> m_missles;

	// level and score
	int m_currentLevel = 0;
	int m_score = 0;
	bool m_gameOver = false;
	float m_totalTime = 0.0f;
	float m_missleReloadTime = 0.0f;

	// ship data
	glm::vec2 m_shipPosition;
	glm::vec2 m_shipVelocity;
	float m_shipRotation = 0.0f;

	// key data
	bool m_leftKeyDown = false;
	bool m_rightKeyDown = false;
	bool m_upKeyDown = false;
	bool m_fireDown = false;
	bool m_warpRequest = false;
};
