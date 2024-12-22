#include "../framework/Window.h"
#include <iostream>
#include "../framework/VertexArrayObject.h"
#include "../framework/Program.h"
#include "../framework/error.h"
#include "../framework/Timer.h"
#include "Terrain.h"

using namespace glm;

// Startwerte für die Optionen
static const float HEIGHT_SCALE = 8.f;
static const int RES = 32;
static const float PERLIN_RESOLUTION = 8.f;
static const int OCTAVES = 4;
static const float PERSISTANCE = 0.5f;

int main()
{
    try
    {
        // Fenster erstellen
        Window wnd = Window(800, 600, "Hardware Renderer");

        // Terrain initialisieren
        Terrain terrain = Terrain(RES, PERLIN_RESOLUTION, OCTAVES, PERSISTANCE, HEIGHT_SCALE);

        // Variablen für die Transformation
        glm::mat4 terrainTransformation = glm::mat4(1.0f);
        float rotationX = 0.0f;
        float rotationY = 0.0f;
        float rotationZ = 0.0f;

        const float rotationSpeed = glm::radians(5.0f); // Rotationsgeschwindigkeit in Radiant

        // Skalierungsfaktor für das Terrain
        float scaleValue = 1.0f;

        // Initiale Transformation berechnen
        terrainTransformation = glm::mat4(1.0f);
        terrainTransformation = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.5f));

        // Tastatureingaben verarbeiten
        wnd.setKeyDownCallback([&](Window::Key key)
        {
            // Steuerung der UI in der Kommandozeile
            terrain.handleUI(key, false);

            // Steuerung der Terrain-Rotation und Skalierung
            switch (key)
            {
                case Window::Key::W:
                    rotationX -= rotationSpeed;
                    break;
                case Window::Key::S:
                    rotationX += rotationSpeed;
                    break;
                case Window::Key::A:
                    rotationY -= rotationSpeed;
                    break;
                case Window::Key::D:
                    rotationY += rotationSpeed;
                    break;
                case Window::Key::Z: // Z-Rotation gegen den Uhrzeigersinn
                    rotationZ -= rotationSpeed;
                    break;
                case Window::Key::X: // Z-Rotation im Uhrzeigersinn
                    rotationZ += rotationSpeed;
                    break;
                case Window::Key::Q:
                    scaleValue *= 1.1f; // Vergrößern
                    break;
                case Window::Key::E:
                    scaleValue *= 0.9f; // Verkleinern
                    break;

                default:
                    break;
            }

            // Transformation neu berechnen
            terrainTransformation = glm::mat4(1.0f);

            // Rotationen anwenden
            terrainTransformation = glm::rotate(terrainTransformation, rotationX, glm::vec3(1, 0, 0));
            terrainTransformation = glm::rotate(terrainTransformation, rotationY, glm::vec3(0, 1, 0));
            terrainTransformation = glm::rotate(terrainTransformation, rotationZ, glm::vec3(0, 0, 1));

            // Skalierung anwenden
            terrainTransformation = glm::scale(terrainTransformation, glm::vec3(scaleValue));
        });

        // Meldungen anzeigen
        std::cout << "Die Optionen fuer das Terrain koennen mit den Pfeiltasten veraendert werden.\n";
        std::cout << "W/S: Rotation um die X-Achse.\n";
        std::cout << "A/D: Rotation um die Y-Achse.\n";
        std::cout << "Z/X: Rotation um die Z-Achse.\n";
        std::cout << "Q/E: Vergrößern/Verkleinern des Terrains.\n";
        terrain.outOptionString();

        // OpenGL-Einstellungen
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe-Modus
        glEnable(GL_DEPTH_TEST); // Tiefentest aktivieren

        // Timer für Frame-Dauer
        Timer t;
        t.start();

        // Hauptschleife
        while (wnd.isOpen())
        {
            t.lap();

            // Bildschirm löschen
            glClearColor(0.7f, 0.9f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Terrain zeichnen mit aktueller Transformation
            terrain.draw(terrainTransformation);

            // Fehlerüberprüfung
            glCheckError("main loop");
            wnd.setTitle("Hardware Renderer | " + std::to_string(t.current()) + " ms");

            // Buffer tauschen und Ereignisse verarbeiten
            wnd.swapBuffer();
            wnd.handleEvents();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERR: " << e.what();
        return 1;
    }
    return 0;
}
