#include <exception>
#include <iostream>
#include "../framework/Window.h"
#include "../framework/Timer.h"
#include "../framework/glmmath.h"

using namespace glm;

int main()
{
	try
	{
		Window wnd(800, 600, "CG Demo");
		
		Timer t;
		t.start();

		while (wnd.isOpen())
		{
			// time delta in milliseconds
			t.lap();
			wnd.clear();

			// draw small rect around mouse
			const int xMax = int(min(wnd.getWidth() - 1, wnd.getMouseX() + 5));
			const int yMax = int(min(wnd.getHeight() - 1, wnd.getMouseY() + 5));
			for(int y = max(int(wnd.getMouseY() - 5), 0); y <= yMax; ++y)
			{
				for (int x = max(int(wnd.getMouseX() - 5), 0); x <= xMax; ++x)
				{
					wnd.putPixel(x, y, 1.0f, 0.0f, 0.0f);
				}
			}

			// measure time before buffer swap
			wnd.setTitle("Software Renderer | " + std::to_string(t.current()) + " ms");

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
