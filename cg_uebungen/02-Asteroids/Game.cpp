#include "Game.h"

#include <array>
#include <random>

using namespace glm;

void Game::draw(Pipeline& gfx) const
{
	// draw missles
	gfx.setVertexScale(m_missleRadius);
	for (const auto& m : m_missles)
	{
		gfx.setVertexRotation(m.rotation);
		gfx.setVertexTranslation(m.position);
		gfx.drawTriangleList(m_missleMesh);
	}

	// draw asteroids
	gfx.setVertexScale(m_bigAsteroidRadius);
	for (const auto& a : m_bigAsteroids)
	{
		gfx.setVertexRotation(a.rotation);
		gfx.setVertexTranslation(a.position);
		gfx.drawTriangleList(m_bigAsteroidMesh);
	}

	gfx.setVertexScale(m_midAsteroidRadius);
	for (const auto& a : m_midAsteroids)
	{
		gfx.setVertexRotation(a.rotation);
		gfx.setVertexTranslation(a.position);
		gfx.drawTriangleList(m_midAsteroidMesh);
	}

	gfx.setVertexScale(m_smallAsteroidRadius);
	for (const auto& a : m_smallAsteroids)
	{
		gfx.setVertexRotation(a.rotation);
		gfx.setVertexTranslation(a.position);
		gfx.drawTriangleList(m_smallAsteroidMesh);
	}

	// draw ship
	gfx.setVertexScale(m_shipRadius);
	gfx.setVertexRotation(m_shipRotation);
	gfx.setVertexTranslation(m_shipPosition);

	//Draw fire
	if (m_upKeyDown && !m_gameOver)
	{
		gfx.setFragmentScale(0.5f + 0.5f * sin(m_totalTime * 0.04f));
		gfx.drawTriangleList(m_shipFireMesh);
		gfx.setFragmentScale(1.0f);
	}

	gfx.drawTriangleList(m_shipMesh);
}

void Game::generateShipMesh()
{
	// left part
	auto c1 = vec3(0.17f, 0.66f, 0.29f);
	auto c2 = vec3(1.0f);
	m_shipMesh.push_back(Vertex(vec2(0.0f, 1.0f), c1));
	m_shipMesh.push_back(Vertex(vec2(-0.5f, -1.0f), c2));
	m_shipMesh.push_back(Vertex(vec2(0.0f, -0.5f), c2));

	// right part
	m_shipMesh.push_back(Vertex(vec2(0.0f, 1.0f), c1));
	m_shipMesh.push_back(Vertex(vec2(0.5f, -1.0f), c2));
	m_shipMesh.push_back(Vertex(vec2(0.0f, -0.5f), c2));
}

void Game::generateFireMesh()
{
	auto fire = vec3(1.0f, 0.1f, 0.1f);
	m_shipFireMesh.push_back(Vertex(vec2(-0.25f, -0.75f), fire));
	m_shipFireMesh.push_back(Vertex(vec2(0.25f, -0.75f), fire));
	m_shipFireMesh.push_back(Vertex(vec2(0.0f, -1.2f), fire));

	m_shipFireMesh.push_back(Vertex(vec2(-0.25f, -0.75f), fire));
	m_shipFireMesh.push_back(Vertex(vec2(0.25f, -0.75f), fire));
	m_shipFireMesh.push_back(Vertex(vec2(0.0f, 0.0f), fire));
}

void Game::generateMissleMesh()
{
	auto white = vec3(1.0f);
	m_missleMesh.push_back(Vertex(vec2(0.0f, 1.0f), white));
	m_missleMesh.push_back(Vertex(vec2(-0.5f, -1.0f), white));
	m_missleMesh.push_back(Vertex(vec2(0.5f, -1.0f), white));
}

// data for generating fancy looking asteroids
static const std::array<vec2, 11> s_smallAsteroidPoints = {
	vec2(0.0f, 0.9f),
	vec2(0.46f, 0.67f),
	vec2(0.38f, 0.13f),
	vec2(0.85f, -0.07f),
	vec2(0.62f, -0.8f),
	vec2(0.23f, -0.53f),
	vec2(0.0f, -0.8f),
	vec2(-0.62f, -0.53f),
	vec2(-0.85f, 0.4f),
	vec2(-0.46f, 0.37f),
	vec2(-0.38f, 0.73f)
};

static const std::array<int, 3 * 9> s_smallAsteroidIndices = {
	0, 1, 2,
	0, 9, 10,
	2, 3, 5,
	3, 4, 5,
	2, 5, 7,
	2, 7, 9,
	5, 6, 7,
	7, 8, 9,
	0, 2, 9
};

static const std::array<vec2, 10> s_medAsteroidPoints = {
	vec2(0.03f, 1.0f),
	vec2(0.26f, 0.5f),
	vec2(0.5f, 0.64f),
	vec2(1.0f, 0.4f),
	vec2(1.0f, -0.5f),
	vec2(0.2f, -1.0f),
	vec2(-0.26f, -0.44f),
	vec2(-0.6f, -0.6f),
	vec2(-1.0f, -0.3f),
	vec2(-0.66f, 0.6f)
};

static const std::array<int, 8 * 3> s_medAsteroidIndices = {
	0, 1, 9,
	1, 2, 3,
	1, 3, 4,
	4, 5, 6,
	6, 7, 8,
	6, 8, 9,
	1, 6, 9,
	1, 4, 6
};

static const std::array<vec2, 12> s_bigAsteroidPoints = {
	vec2(-0.5f, 0.91f),
	vec2(-0.1f, 0.73f),
	vec2(0.34, 0.91f),
	vec2(0.79f, 0.45f),
	vec2(0.41f, 0.36f),
	vec2(0.93f, 0.15f),
	vec2(0.41f, -0.76f),
	vec2(-0.17f, -0.7f),
	vec2(-0.34f, -0.88f),
	vec2(-0.93f, -0.27f),
	vec2(-0.76f, 0.0f),
	vec2(-0.93f, 0.58f)
};

static const std::array<int, 10 * 3> s_bigAsteroidIndices = {
	0, 1, 11,
	1, 2, 4,
	2, 3, 4,
	1, 4, 10,
	4, 5, 6,
	4, 6, 7,
	4, 7, 10,
	7, 9, 10,
	7, 8, 9,
	1, 10, 11
};

template<size_t n_indices, size_t n_vertices>
std::vector<Vertex> makeAsteroid(const std::array<int, n_indices>& indices, const std::array<vec2, n_vertices>& vertices)
{
	std::vector<Vertex> list;

	std::mt19937 twister;
	twister.seed(n_indices);
	// random gray color
	auto dist = std::uniform_real_distribution<float>(0.5f, 0.8f);

	for (auto i : indices)
	{
		Vertex v;
		v.pos = vertices[i];
		v.color = vec3(dist(twister));
		list.push_back(v);
	}

	return list;
}

Game::Game(Window& window)
	:
	m_twister(std::random_device()())
{
	// create meshs
	m_bigAsteroidMesh = makeAsteroid(s_bigAsteroidIndices, s_bigAsteroidPoints);
	m_midAsteroidMesh = makeAsteroid(s_medAsteroidIndices, s_medAsteroidPoints);
	m_smallAsteroidMesh = makeAsteroid(s_smallAsteroidIndices, s_smallAsteroidPoints);
	generateShipMesh();
	generateFireMesh();
	generateMissleMesh();

	// load level
	nextLevel();

	// set keyboard controls
	window.setKeyDownCallback([this](Window::Key key)
		{
			switch (key)
			{
			case Window::Key::LEFT:
			case Window::Key::A:
				m_leftKeyDown = true;
				break;
			case Window::Key::RIGHT:
			case Window::Key::D:
				m_rightKeyDown = true;
				break;
			case Window::Key::UP:
			case Window::Key::W:
				m_upKeyDown = true;
				break;
			case Window::Key::DOWN:
			case Window::Key::S:
				m_warpRequest = true;
				break;
			case Window::Key::RIGHT_CONTROL:
			case Window::Key::LEFT_CONTROL:
			case Window::Key::SPACE:
				m_fireDown = true;
				break;
			default:;
			}
		});

	window.setKeyUpCallback([this](Window::Key key)
		{
			switch (key)
			{
			case Window::Key::LEFT:
			case Window::Key::A:
				m_leftKeyDown = false;
				break;
			case Window::Key::RIGHT:
			case Window::Key::D:
				m_rightKeyDown = false;
				break;
			case Window::Key::UP:
			case Window::Key::W:
				m_upKeyDown = false;
				break;
			case Window::Key::RIGHT_CONTROL:
			case Window::Key::LEFT_CONTROL:
			case Window::Key::SPACE:
				m_fireDown = false;
				break;
			case Window::Key::BACKSPACE:
				m_currentLevel = 0;
				m_score = 0;
				m_totalTime = 0.0f;
				nextLevel();
				break;
			default:;
			}
		});
}

void Game::update(float dt)
{
	if (m_gameOver) return;

	m_totalTime += dt;

	// update asteroids
	for (auto& a : m_bigAsteroids)
	{
		a.position += a.velocity * dt;
		a.rotation += dt * 0.0003f;
		clipObject(a.position, a.velocity, m_bigAsteroidRadius);
	}

	for (auto& a : m_midAsteroids)
	{
		a.position += a.velocity * dt;
		a.rotation += dt * 0.0006f;
		clipObject(a.position, a.velocity, m_midAsteroidRadius);
	}

	for (auto& a : m_smallAsteroids)
	{
		a.position += a.velocity * dt;
		a.rotation += dt * 0.0012f;
		clipObject(a.position, a.velocity, m_smallAsteroidRadius);
	}

	// process keyboard inputs
	if (m_warpRequest)
	{
		m_warpRequest = false;
		m_shipVelocity = vec2(0.0f);
		auto posDist = std::uniform_real_distribution<float>(-0.9f, 0.9f);
		m_shipPosition.x = posDist(m_twister);
		m_shipPosition.y = posDist(m_twister);
	}

	if (m_leftKeyDown)
		m_shipRotation += dt * 0.0075f;

	if (m_rightKeyDown)
		m_shipRotation -= dt * 0.0075f;

	if (m_upKeyDown)
	{
		// adjust velocity
		vec2 acceleration = vec2(-sin(m_shipRotation), cos(m_shipRotation)) * dt * 0.000001f;

		m_shipVelocity += acceleration;
		// limit max speed
		float speed = length(m_shipVelocity);
		const float maxSpeed = 0.001f;
		if (speed > maxSpeed)
		{
			m_shipVelocity *= maxSpeed / speed;
		}
	}

	// update ship position
	m_shipPosition += m_shipVelocity * dt;

	// clip the ship
	clipObject(m_shipPosition, m_shipVelocity, m_shipRadius);

	// update missle fire
	m_missleReloadTime -= dt;
	if (m_fireDown && m_missleReloadTime <= 0.0f)
	{
		// 0.2s reload time
		m_missleReloadTime = 200.0f;
		// fire missle
		MissleData d;
		d.position = m_shipPosition;
		d.velocity = vec2(-sin(m_shipRotation), cos(m_shipRotation)) * 0.00175f + m_shipVelocity;
		d.rotation = m_shipRotation;
		// 1 s lifetime
		d.lifetime = 1000.0f;
		m_missles.push_back(d);
	}

	// update missles
	for (auto& m : m_missles)
	{
		m.lifetime -= dt;
		m.position += m.velocity * dt;
		clipObject(m.position, m.velocity, m_missleRadius);
	}

	// remove dead missles
	while (!m_missles.empty() && m_missles.front().lifetime < 0.0f)
		m_missles.pop_front();

	// handle missle/asteroid collission
	auto newEnd = std::remove_if(m_missles.begin(), m_missles.end(), [this](const MissleData& m)
		{
			const float asteroidSlow = 0.75f;
			// test big asteroids
			for (auto& a : m_bigAsteroids)
			{
				// big ones (simple bounding sphere test)
				if (a.alive && distance(a.position, m.position) < m_missleRadius + m_bigAsteroidRadius)
				{
					// collision
					a.alive = false;
					// spawn smaller asteroids
					AsteroidData newAsteroid;
					newAsteroid.position = a.position;
					newAsteroid.velocity = a.velocity * asteroidSlow + generateAsteroidVelocity();
					m_midAsteroids.push_back(newAsteroid);
					newAsteroid.velocity = a.velocity * asteroidSlow + generateAsteroidVelocity();
					m_midAsteroids.push_back(newAsteroid);
					m_score += 20;
					return true;
				}
			}

			// test mid asteroids
			for (auto& a : m_midAsteroids)
			{
				// mid ones
				if (a.alive && distance(a.position, m.position) < m_missleRadius + m_midAsteroidRadius)
				{
					// collision
					a.alive = false;
					// spawn smaller asteroids
					AsteroidData newAsteroid;
					newAsteroid.position = a.position;
					newAsteroid.velocity = a.velocity * asteroidSlow + generateAsteroidVelocity();
					m_smallAsteroids.push_back(newAsteroid);
					newAsteroid.velocity = a.velocity * asteroidSlow + generateAsteroidVelocity();
					m_smallAsteroids.push_back(newAsteroid);
					m_score += 50;
					return true;
				}
			}

			// test small ones
			for (auto& a : m_smallAsteroids)
			{
				// small ones
				if (a.alive && distance(a.position, m.position) < m_missleRadius + m_smallAsteroidRadius)
				{
					// collision
					a.alive = false;
					m_score += 100;
					return true;
				}
			}

			return false;
		});
	m_missles.erase(newEnd, m_missles.end());

	// erase dead asteroids
	eraseDeadAsteroids(m_bigAsteroids);
	eraseDeadAsteroids(m_midAsteroids);
	eraseDeadAsteroids(m_smallAsteroids);

	// test for asteroid/ship collission
	const float fairShipRadius = m_shipRadius * 0.5f;
	for (const auto& a : m_bigAsteroids)
	{
		if (distance(a.position, m_shipPosition) < m_bigAsteroidRadius + fairShipRadius)
		{
			m_gameOver = true;
			return;
		}
	}

	for (const auto& a : m_midAsteroids)
	{
		if (distance(a.position, m_shipPosition) < m_midAsteroidRadius + fairShipRadius)
		{
			m_gameOver = true;
			return;
		}
	}

	for (const auto& a : m_smallAsteroids)
	{
		if (distance(a.position, m_shipPosition) < m_smallAsteroidRadius + fairShipRadius)
		{
			m_gameOver = true;
			return;
		}
	}

	// level clear?
	if (m_bigAsteroids.empty() && m_midAsteroids.empty() && m_smallAsteroids.empty())
	{
		nextLevel();
	}
}

void Game::nextLevel()
{
	// reset ship status
	m_shipPosition = vec2(0.0f);
	m_shipVelocity = vec2(0.0f);
	m_leftKeyDown = false;
	m_rightKeyDown = false;
	m_upKeyDown = false;
	m_shipRotation = 0.0f;
	m_warpRequest = false;

	m_missles.clear();
	m_gameOver = false;

	// reset asteroid status
	m_bigAsteroids.clear();
	m_midAsteroids.clear();
	m_smallAsteroids.clear();

	// generate random asteroids
	auto asteroidSideDist = std::uniform_int_distribution<int>(0, 3);
	auto asteroidPositionDist = std::uniform_real_distribution<float>(-1.0f, 1.0f);
	auto rotationDist = std::uniform_real_distribution<float>(0.0f, 6.3f);

	const int numAsteroids = 4 + 2 * m_currentLevel++;
	for (int i = 0; i < numAsteroids; ++i)
	{
		AsteroidData d;
		// start position
		const float pos = asteroidPositionDist(m_twister);
		switch (asteroidSideDist(m_twister))
		{
		case 0:
			d.position.x = pos;
			d.position.y = -1.0f;
			break;
		case 1:
			d.position.x = pos;
			d.position.y = 1.0f;
			break;
		case 2:
			d.position.y = pos;
			d.position.x = -1.0f;
			break;
		case 3:
			d.position.y = pos;
			d.position.x = 1.0f;
			break;
		}
		// start velocity and rotation
		d.rotation = rotationDist(m_twister);
		d.velocity = generateAsteroidVelocity();

		m_bigAsteroids.push_back(d);
	}
}

void Game::clipObject(vec2& position, const vec2& velocity, float radius) const
{
	// clip to x?
	if (position.x * sign(velocity.x) > 1.0f + radius)
		position.x = -position.x;

	if (position.y * sign(velocity.y) > 1.0f + radius)
		position.y = -position.y;
}

glm::vec2 Game::generateAsteroidVelocity()
{
	auto asteroidPositionDist = std::uniform_real_distribution<float>(-1.0f, 1.0f);
	auto velocityDist = std::uniform_real_distribution<float>(0.000075f, 0.0006f);

	auto vel = vec2(asteroidPositionDist(m_twister), asteroidPositionDist(m_twister));
	vel = normalize(vel) * velocityDist(m_twister);
	return vel;
}

void Game::eraseDeadAsteroids(std::vector<AsteroidData>& vec)
{
	auto end = std::remove_if(vec.begin(), vec.end(), [](const AsteroidData& a)
		{
			return !a.alive;
		});
	vec.erase(end, vec.end());
}
