#include "Pipeline.h"
#include <array>

using namespace glm;

void Pipeline::drawTriangle(Vertex v1, Vertex v2, Vertex v3)
{
    // Vertex Shader: (Optionale Transformationen können hier ausgeführt werden)
    shadeVertex(v1);
    shadeVertex(v2);
    shadeVertex(v3);

    bool isInWindow =
        std::abs(v1.pos.x) <= 1.0f && std::abs(v1.pos.y) <= 1.0f &&
        std::abs(v2.pos.x) <= 1.0f && std::abs(v2.pos.y) <= 1.0f &&
        std::abs(v3.pos.x) <= 1.0f && std::abs(v3.pos.y) <= 1.0f;
    if(isInWindow)
    {
        // Wenn alle Punkte im Fenster, Dreieck ohne Clipping zeichnen
        drawClippedTriangle({ v1, v2, v3 });
        return;
    }

    // Entfernen Sie diese Zeile (Aufgabe 2)
    // if (!isInWindow) return;

    // Clip triangle
    m_triangleList.resize(0);
    m_triangleList.push_back(v1);
    m_triangleList.push_back(v2);
    m_triangleList.push_back(v3);

    if(clipPolygonAxis(0) && clipPolygonAxis(1)) // an x und y Achse clippen
    {
        // Überprüfung, ob wirklich alles geclippt ist:
        for(const auto& v : m_triangleList)
        {
            dassert(v.pos.x <= 1.0f); // Positiv x okay?
            dassert(v.pos.y <= 1.0f); // Positiv y okay?
            dassert(v.pos.x >= -1.0f); // Negativ x okay?
            dassert(v.pos.y >= -1.0f); // Negativ y okay?
        }

        // Triangle Fan zeichnen mit den Punkten in m_triangleList (Aufgabe 2)
        for (size_t i = 1; i + 1 < m_triangleList.size(); ++i)
        {
            drawClippedTriangle({ m_triangleList[0], m_triangleList[i], m_triangleList[i + 1] });
        }
    }
}

void Pipeline::drawClippedTriangle(std::array<Vertex, 3> vertices)
{
    // Vertices nach y-Wert sortieren (vertices[0] soll den kleinsten y-Wert haben)
    if(vertices[0].pos.y > vertices[1].pos.y)
        std::swap(vertices[0], vertices[1]);
    if(vertices[1].pos.y > vertices[2].pos.y)
        std::swap(vertices[1], vertices[2]);
    if(vertices[0].pos.y > vertices[1].pos.y)
        std::swap(vertices[0], vertices[1]);

    // Umrechnung von [-1, 1] auf [0, m_width] für pos.x und von [-1, 1] auf [0, m_height] für pos.y (OpenGL->Pixel)
    for (auto& vert : vertices)
    {
        vert.pos.x = (vert.pos.x + 1.0f) * 0.5f * m_width;
        vert.pos.y = (vert.pos.y + 1.0f) * 0.5f * m_height;
    }

    // Bottom-left rule
    const float fStart = vertices[0].pos.y;
    const float fMid = vertices[1].pos.y;
    const float fEnd = vertices[2].pos.y;
    const int yStart = int(std::ceil(fStart - 0.5f));
    const int yMid = int(std::ceil(fMid - 0.5f));
    const int yEnd = int(std::ceil(fEnd - 0.5f));

    // draw lower half
    for(int y = yStart; y < yMid; ++y)
    {
        // interpolate start to end vertex
        auto edge1 = Vertex::lerp(vertices[0], vertices[2], (float(y) + 0.5f - fStart) / (fEnd - fStart));
        // interpolate start to mid
        auto edge2 = Vertex::lerp(vertices[0], vertices[1], (float(y) + 0.5f - fStart) / (fMid - fStart));

        if(edge1.pos.x < edge2.pos.x)
            scanLine(y, edge1, edge2);
        else
            scanLine(y, edge2, edge1);
    }

    // draw upper half
    for(int y = yMid; y < yEnd; ++y)
    {
        // Interpolation von mid zu end für die linke Kante
        auto edge1 = Vertex::lerp(vertices[1], vertices[2], (float(y) + 0.5f - fMid) / (fEnd - fMid));
        // Interpolation von start zu end für die rechte Kante
        auto edge2 = Vertex::lerp(vertices[0], vertices[2], (float(y) + 0.5f - fStart) / (fEnd - fStart));

        if(edge1.pos.x < edge2.pos.x)
            scanLine(y, edge1, edge2);
        else
            scanLine(y, edge2, edge1);
    }
}

void Pipeline::clipPolygonComponent(const std::vector<Vertex>& in, std::vector<Vertex>& out, int axis, float side)
{
    out.clear();
    size_t size = in.size();

    for (size_t i = 0; i < size; ++i)
    {
        const Vertex& currentVertex = in[i];
        const Vertex& previousVertex = in[(i + size - 1) % size];

        // Extrahieren der Positionskomponenten entlang der Achse
        float currentPos = currentVertex.pos[axis];
        float previousPos = previousVertex.pos[axis];

        // Bestimmen, ob die Punkte innerhalb oder außerhalb der Clip-Kante liegen
        bool currentInside = side * currentPos <= 1.0f;
        bool previousInside = side * previousPos <= 1.0f;

        if (currentInside)
        {
            if (!previousInside)
            {
                // Schnittpunkt berechnen und hinzufügen
                float t = (1.0f - side * previousPos) / (side * (currentPos - previousPos));
                Vertex intersect = Vertex::lerp(previousVertex, currentVertex, t);
                out.push_back(intersect);
            }
            // Aktuellen Punkt hinzufügen
            out.push_back(currentVertex);
        }
        else if (previousInside)
        {
            // Schnittpunkt berechnen und hinzufügen
            float t = (1.0f - side * previousPos) / (side * (currentPos - previousPos));
            Vertex intersect = Vertex::lerp(previousVertex, currentVertex, t);
            out.push_back(intersect);
        }
        // Wenn beide Punkte außerhalb liegen, wird nichts hinzugefügt
    }

    // Clipping-Überprüfung
    for (const auto& v : out)
    {
        if (side > 0.0f) // Clipping an positiver Seite korrekt?
        {
            dassert(v.pos[axis] <= 1.0f);
        }
        else // Clipping an negativer Seite korrekt?
        {
            dassert(v.pos[axis] >= -1.0f);
        }
    }
}

// FERTIGE FUNKTIONEN: ///////////////////////////////////////////////

bool Pipeline::clipPolygonAxis(int axis)
{
    // clip to 1.0
    clipPolygonComponent(m_triangleList, m_triangleTmpList, axis, 1.0f);
    if (m_triangleTmpList.empty())
        return false;

    // clip to -1.0
    clipPolygonComponent(m_triangleTmpList, m_triangleList, axis, -1.0f);
    return !m_triangleList.empty();
}

Pipeline::Pipeline(Window& window)
    :
    m_window(window)
{
    m_triangleList.reserve(16);
    m_triangleTmpList.reserve(16);
}

void Pipeline::begin()
{
    // clear window screen
    m_window.clear();

    m_width = float(m_window.getWidth());
    m_height = float(m_window.getHeight());
}

void Pipeline::scanLine(int y, const Vertex& left, const Vertex& right)
{
    // Ist left wirklich links?
    dassert(left.pos.x <= right.pos.x);
    // Sind die Vertices im Bildschirm?
    dassert(left.pos.x >= 0.0f);
    dassert(left.pos.y >= 0.0f);
    dassert(left.pos.x <= m_width);
    dassert(left.pos.y <= m_height);
    dassert(right.pos.x >= 0.0f);
    dassert(right.pos.y >= 0.0f);
    dassert(right.pos.x <= m_width);
    dassert(right.pos.y <= m_height);

    // determine x start and end (bottom left rule)
    const float fStart = left.pos.x;
    const float fEnd = right.pos.x;
    const int xStart = int(std::ceil(fStart - 0.5f));
    const int xEnd = int(std::ceil(fEnd - 0.5f));

    // draw pixels
    for (int x = xStart; x < xEnd; ++x)
    {
        auto vert = Vertex::lerp(left, right, (float(x) + 0.5f - fStart) / (fEnd - fStart));
        auto color = shadeFragment(vert);
        m_window.putPixel(x, y, color.r, color.g, color.b);
    }
}

void Pipeline::shadeVertex(Vertex& vertex)
{
    vertex.pos *= m_scale;
    vertex.pos = vec2(vertex.pos.x * m_rotationCosine - vertex.pos.y * m_rotationSine, vertex.pos.y * m_rotationCosine + vertex.pos.x * m_rotationSine);
    vertex.pos += m_translation;
}

glm::vec3 Pipeline::shadeFragment(const Vertex& vertex)
{
    return vertex.color * m_colorScale;
}

void Pipeline::drawTriangleList(const std::vector<Vertex>& vertices)
{
    dassert(vertices.size() % 3 == 0);
    for (size_t i = 0; i < vertices.size(); i += 3)
        drawTriangle(vertices[i], vertices[i + 1], vertices[i + 2]);
}

void Pipeline::setVertexTranslation(const glm::vec2& translation)
{
    m_translation = translation;
}

void Pipeline::setVertexRotation(float angle)
{
    m_rotationSine = sin(angle);
    m_rotationCosine = cos(angle);
}

void Pipeline::setVertexScale(float scale)
{
    m_scale = scale;
}

void Pipeline::setFragmentScale(float scale)
{
    m_colorScale = scale;
}
