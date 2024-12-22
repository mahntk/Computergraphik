#include <exception>
#include <iostream>
#include "../framework/Window.h"
#include "Pipeline.h"
#include "../framework/Timer.h"
#include "Game.h"

using namespace glm;

int main()
{
	try
	{
		Window wnd(800, 800, "Software Renderer");
		Pipeline pipe = Pipeline(wnd);

		Timer t;
		t.start();

		Game game(wnd);

		while (wnd.isOpen())
		{
			// time delta in milliseconds
			auto dt = t.lap();
			game.update(dt);

			pipe.begin();

			//game.draw(pipe);

			// (Aufgabe 1) Dreieck in OpenGL Koordinaten:
			pipe.drawTriangle(
				Vertex(vec2(0.7f, 0.9f), vec3(1.0f, 0.0f, 0.0f)),
				Vertex(vec2(-0.6f, -0.1f), vec3(0.0f, 1.0f, 0.0f)),
				Vertex(vec2(0.6f, -0.7f), vec3(0.0f, 0.0f, 1.0f))
			);

			// (Aufgabe 2) Dreieck ausserhalb des Bildschirms (wird aktuell nicht gemalt weil ausserhalb):
			pipe.drawTriangle(
				Vertex(vec2(-1.2f, 0.7f), vec3(1.0f, 0.0f, 0.0f)),
				Vertex(vec2(1.3f, 0.6f), vec3(0.0f, 1.0f, 0.0f)),
				Vertex(vec2(0.0f, -1.4f), vec3(0.0f, 0.0f, 1.0f))
			);


			// measure time before buffer swap
			auto timeMs = t.current();
			wnd.setTitle("Software Renderer | " + std::to_string(timeMs) + 
				" ms | level: " + std::to_string(game.getLevel()) + 
				" | score: " + std::to_string(game.getScore()));

			wnd.swapBuffer();
			wnd.handleEvents();
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "ERR: " << e.what();
		// let the console stay open to read the exception
		while (true);
	}
	return 0;
}
