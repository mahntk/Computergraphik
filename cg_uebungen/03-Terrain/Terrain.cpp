#include "Terrain.h"
#include "Pseudorandom.h"
#include <iostream>
#include <algorithm>

using namespace glm;

Terrain::Terrain(int resolution, float perlinRes, int octaves, float persistence, float heightScale)
    : m_resolution(resolution),
      m_perlinRes(perlinRes),
      m_octaves(octaves),
      m_persistence(persistence),
      m_heightScale(heightScale)
{
    regenerate();

    m_program
        .attach(Shader(GL_VERTEX_SHADER).loadFromFile("shader/terrain.vert"))
        .attach(Shader(GL_FRAGMENT_SHADER).loadFromFile("shader/terrain.frag"))
        .link();

    // Uniform-Variable im Shader suchen und speichern
    m_uniformTransform = m_program.getUniform<glm::mat4>("u_transform", glm::mat4(1.0f));
}

void Terrain::regenerate()
{
    // Initialisieren und Terrain regenerieren
    m_height.assign(m_resolution * m_resolution, 0.0f);

    generate();

    createVertexBuffer();
}

void Terrain::draw(const glm::mat4& transform)
{
    // Programm binden
    m_program.bind();

    // Transformationsmatrix an den Shader übergeben
    m_uniformTransform.set(transform);

    // Terrain malen
    m_vao.drawElements(GL_TRIANGLES, m_program, m_indices);
}

void Terrain::createVertexBuffer()
{
    // Beide Buffer müssen bei jedem Aufruf geleert werden
    m_positions.clear();
    m_indices.clear();

    int resolution = m_resolution;
    float maxCoord = static_cast<float>(resolution - 1);

    // Vertex-Positionen hinzufügen
    for (int z = 0; z < resolution; ++z)
    {
        for (int x = 0; x < resolution; ++x)
        {
            float height = getHeight(x, z); // Höhe an der Position (x, z)

            // Skalierung und Verschiebung in den Bereich [-0.5, +0.5]
            float xPos = ((static_cast<float>(x) / maxCoord) - 0.5f) * 2.0f;
            float zPos = ((static_cast<float>(z) / maxCoord) - 0.5f) * 2.0f;
            float yPos = height / m_heightScale;

            glm::vec3 position = glm::vec3(xPos, yPos, zPos);
            m_positions.addVertex(position);
        }
    }

    // Indizes für die Dreiecke hinzufügen
    for (int z = 0; z < resolution - 1; ++z)
    {
        for (int x = 0; x < resolution - 1; ++x)
        {
            // Vier Eckpunkte des aktuellen Gitters
            int topLeft = calcIndex(x, z);
            int topRight = calcIndex(x + 1, z);
            int bottomLeft = calcIndex(x, z + 1);
            int bottomRight = calcIndex(x + 1, z + 1);

            // Erstes Dreieck
            m_indices.addIndex(topLeft);
            m_indices.addIndex(bottomLeft);
            m_indices.addIndex(topRight);

            // Zweites Dreieck
            m_indices.addIndex(topRight);
            m_indices.addIndex(bottomLeft);
            m_indices.addIndex(bottomRight);
        }
    }

    // Daten zur GPU hochladen
    m_positions.uploadToGpu();
    m_indices.uploadToGpu();

    // Vertex-Buffer an VAO anhängen (an Location 0)
    m_vao.attachBuffer(0, m_positions);
}

float Terrain::perlinOctaves(float x, float z) {
    float total = 0.0f;
    float maxAmplitude = 0.0f;

    float frequency = 1.0f;
    float amplitude = 1.0f;

    for (int i = 0; i < m_octaves; ++i) {
        total += perlinNoise(x * frequency, z * frequency) * amplitude;
        maxAmplitude += amplitude;

        frequency *= 1.8f; // Frequenz moderat steigern (statt 2.0f)
        amplitude *= 0.3f; // Kleinere Amplitudenreduzierung (statt direkt mit persistence multiplizieren)

    }


    // Normalisieren des Ergebnisses
    return (total / maxAmplitude) * 0.5f; // Für kleinere Höhen

}



void Terrain::generate() {
    for (int z = 0; z < m_resolution; ++z) {
        for (int x = 0; x < m_resolution; ++x) {
            // Umrechnung der Gitterkoordinaten auf die Perlin-Noise-Resolution
            float perlinX = (static_cast<float>(x) / (m_resolution - 1)) * (m_perlinRes * 0.9f);
            float perlinZ = (static_cast<float>(z) / (m_resolution - 1)) * (m_perlinRes * 0.9f);




            // Berechnung des Höhenwerts mit Perlin Noise und mehreren Oktaven
            float height = perlinOctaves(perlinX, perlinZ) * 1.2f; // Leichte Verstärkung des Höhenwertes
            setHeight(x, z, height);


        }
    }
}





float Terrain::perlinNoise(float x, float z) {
    // Diese Methode bleibt erhalten, die zweite wird entfernt
    Pseudorandom rng(758385u);

    int x0 = static_cast<int>(std::floor(x));
    int x1 = x0 + 1;
    int z0 = static_cast<int>(std::floor(z));
    int z1 = z0 + 1;

    float dx = x - x0;
    float dz = z - z0;

    glm::vec2 gradient00 = rng.getRandomDirection(x0, z0);
    glm::vec2 gradient10 = rng.getRandomDirection(x1, z0);
    glm::vec2 gradient01 = rng.getRandomDirection(x0, z1);
    glm::vec2 gradient11 = rng.getRandomDirection(x1, z1);

    glm::vec2 dist00(dx, dz);
    glm::vec2 dist10(dx - 1, dz);
    glm::vec2 dist01(dx, dz - 1);
    glm::vec2 dist11(dx - 1, dz - 1);

    float dot00 = glm::dot(gradient00, dist00);
    float dot10 = glm::dot(gradient10, dist10);
    float dot01 = glm::dot(gradient01, dist01);
    float dot11 = glm::dot(gradient11, dist11);

    auto fade = [](float t) { return t * t * (3.0f - 2.0f * t); }; // Weniger aggressive Glättung


    float wx = fade(dx);
    float wz = fade(dz);

    float ix0 = glm::mix(dot00, dot10, wx);
    float ix1 = glm::mix(dot01, dot11, wx);
    float value = glm::mix(ix0, ix1, wz);

    return value * 0.5f + 0.5f;
}





void Terrain::handleUI(Window::Key key, bool useWASD)
{
    if (useWASD)
    {
        switch (key)
        {
        case Window::Key::W:
            changeOptionValue(true);
            break;
        case Window::Key::S:
            changeOptionValue(false);
            break;
        case Window::Key::A:
            switchOptions(false);
            break;
        case Window::Key::D:
            switchOptions(true);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (key)
        {
        case Window::Key::UP:
            changeOptionValue(true);
            break;
        case Window::Key::DOWN:
            changeOptionValue(false);
            break;
        case Window::Key::LEFT:
            switchOptions(false);
            break;
        case Window::Key::RIGHT:
            switchOptions(true);
            break;
        default:
            break;
        }
    }
}

void Terrain::switchOptions(bool increase)
{
    m_selectedOption += increase ? 1 : -1;
    if (m_selectedOption < 0)
        m_selectedOption = NUM_OPTIONS - 1;
    else
        m_selectedOption %= NUM_OPTIONS;
    outOptionString(); // Update String
}

void Terrain::changeOptionValue(bool increase)
{
    switch (m_selectedOption)
    {
    case 0: // Height Scale
        m_heightScale += increase ? STEP_HEIGHT_SCALE : -STEP_HEIGHT_SCALE;
        m_heightScale = std::max(std::min(m_heightScale, 500.f), 1.f);
        break;
    case 1: // Grid Res
        if (increase)
            m_resolution = m_resolution * 2;
        else
            m_resolution = m_resolution / 2;
        m_resolution = std::max(std::min(m_resolution, 256), 4); // Grid Auflösung ist begrenzt zwischen [4,256]
        break;
    case 2: // Perlin Res
        m_perlinRes += increase ? STEP_PERLIN_RES : -STEP_PERLIN_RES;
        m_perlinRes = std::max(std::min(m_perlinRes, 512.f), 1.f + STEP_PERLIN_RES);
        break;
    case 3: // Octaves
        m_octaves += increase ? 1 : -1;
        m_octaves = std::min(std::max(1, m_octaves), 16);
        break;
    case 4: // Persistence
        m_persistence += increase ? STEP_PERSISTENCE : -STEP_PERSISTENCE;
        m_persistence = std::max(std::min(m_persistence, 1.f), STEP_PERSISTENCE);
        break;
    default:
        break;
    }

    regenerate();
    outOptionString(); // Update String
}

void Terrain::outOptionString()
{
    std::cout << "\r                                                                \r"; // Zeile zurücksetzen
    switch (m_selectedOption)
    {
    case 0: // Height Scale
        std::cout << "Height Scale: " + std::to_string(m_heightScale);
        break;
    case 1: // Grid Res
        std::cout << "Grid Resolution: " + std::to_string(m_resolution);
        break;
    case 2: // Perlin Res
        std::cout << "Perlin Resolution: " + std::to_string(m_perlinRes);
        break;
    case 3: // Octaves
        std::cout << "Perlin Octaves: " + std::to_string(m_octaves);
        break;
    case 4: // Persistence
        std::cout << "Perlin Persistence: " + std::to_string(m_persistence);
        break;
    default:
        std::cout << "Err State. Selected Option Value:" + std::to_string(m_selectedOption); // Sollte nicht passieren
        break;
    }
}

void Terrain::setHeight(int x, int z, float height)
{
    if (getHeight(x, z) == 0.0f)
        m_height[calcIndex(x, z)] = height * m_heightScale;

}

float Terrain::getHeight(int x, int z) const
{
    return m_height[calcIndex(x, z)];
}

int Terrain::calcIndex(int x, int z) const
{
    // Assertions können hier optional aktiviert werden
    // dassert(x >= 0);
    // dassert(z >= 0);
    // dassert(x < m_resolution);
    // dassert(z < m_resolution);
    return z * m_resolution + x;
}

glm::mat4 Terrain::getBackupTransformation() const
{
    return mat4(
        0.0466215f, 0.0f,      -0.025469f, 0.0f,
        0.012210f,  0.046621f, 0.0223515f, 0.0f,
        0.022351f, -0.025469f, 0.040914f,  0.0f,
        -1.152413f, 0.221025f, -0.3365f,   1.0f
    );
}
