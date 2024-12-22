#include "Window.h"
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <glm/detail/func_common.hpp>
#include <memory>

// required for the mouse and keyboard callbacks
static Window* s_window = nullptr;

static void errorCallbackGLFW(int error, const char* description)
{
	std::cerr << "ERR: GLFW error, code " << error << " desc: \"" << description << "\"\n";
}

Window::Window(size_t width, size_t height, const std::string& title)
	:
	m_width(width),
	m_height(height)
{
	if (s_window)
		throw std::runtime_error("Only one instance of Window should be open to work properly");

	s_window = this;

	glfwSetErrorCallback(errorCallbackGLFW);
	if (!glfwInit()) throw std::runtime_error("Cannot initialize GLFW!\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, 0);
	

	m_handle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
	if (!m_handle) throw std::runtime_error("Window creation failed!");
	glfwMakeContextCurrent(m_handle);
	glfwSetWindowSizeLimits(m_handle, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);

	if (!gladLoadGL())
		throw std::runtime_error("Cannot initialize Glad/load gl-function pointers!\n");

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glfwSetWindowSizeCallback(m_handle, [](GLFWwindow* window, int width, int height)
	{
		s_window->resize(width, height);
	});
	glfwSetCursorPosCallback(m_handle, [](GLFWwindow* window, double x, double y)
	{
		if (!s_window->m_mouseDisabled) {
			auto prevX = s_window->getMouseXNorm();
			auto prevY = s_window->getMouseYNorm();
			// clamp because mouse coordinates can be outisde of the window when a button is down
			s_window->m_mouseX = static_cast<size_t>(glm::clamp(int(x), 0, int(s_window->m_width - 1)));
			// reverse y because of different coordinate system
			s_window->m_mouseY = static_cast<size_t>(glm::clamp(int(s_window->m_height) - int(y) - 1, 0, int(s_window->m_height - 1)));

			auto newX = s_window->getMouseXNorm();
			auto newY = s_window->getMouseYNorm();

			if (s_window->m_onMouseMove && (prevX != newX || prevY != newY))
				s_window->m_onMouseMove(newX, newY, newX - prevX, newY - prevY);
		}
		else {		
			s_window->m_onMouseMove(0.f, 0.f, static_cast<float>(x / s_window->getWidth()), static_cast<float>(y / s_window->getHeight()));
			glfwSetCursorPos(window, 0.0, 0.0);	//Reset x,y to [0,0]
		}
		
	});
	glfwSetKeyCallback(m_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if(action == GLFW_RELEASE)
		{
			if (s_window->m_onKeyUp)
				s_window->m_onKeyUp(Key(key));
		}
		else if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (s_window->m_onKeyDown)
				s_window->m_onKeyDown(Key(key));
		}
	});
	glfwSetMouseButtonCallback(m_handle, [](GLFWwindow* window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			if (s_window->m_onMouseDown)
				s_window->m_onMouseDown(Button(button), s_window->getMouseXNorm(), s_window->getMouseYNorm());
		}
		else if (action == GLFW_RELEASE)
		{
			if (s_window->m_onMouseUp)
				s_window->m_onMouseUp(Button(button), s_window->getMouseXNorm(), s_window->getMouseYNorm());
		}
	});

#ifdef WINDOW_PUT_PIXEL
	initShader();
#endif

	resize(width, height);
}

Window::~Window()
{
	if (m_handle)
		glfwDestroyWindow(m_handle);
	glfwTerminate();
	s_window = nullptr;
}

void Window::handleEvents()
{
	glfwPollEvents();
	m_open = !glfwWindowShouldClose(m_handle);
}

void Window::swapBuffer() const
{
#ifdef WINDOW_PUT_PIXEL
	// update texture data
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GLsizei(m_width), GLsizei(m_height), GL_RGB, GL_UNSIGNED_BYTE, m_pixels.data());
	
	// draw screenfilling quad
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#endif
	glfwSwapBuffers(m_handle);
}

#ifdef WINDOW_PUT_PIXEL

void Window::putPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
	dassert(x >= 0);
	dassert(y >= 0);
	dassert(x < int(m_width));
	dassert(y < int(m_height));
	m_pixels[(y * m_width + x) * 3 + 0] = r;
	m_pixels[(y * m_width + x) * 3 + 1] = g;
	m_pixels[(y * m_width + x) * 3 + 2] = b;
}

void Window::putPixel(int x, int y, float r, float g, float b)
{
	putPixel(x, y, convertToBits(r), convertToBits(g), convertToBits(b));
}

void Window::initShader()
{
	Shader vertex(GL_VERTEX_SHADER);
	vertex.loadFromSource(R"(
	#version 330 core
	void main() {
		vec4 vertex = vec4(0.0, 0.0, 0.0, 1.0);
		if(uint(gl_VertexID) == 0u) vertex = vec4(1.0, -1.0, 0.0, 1.0);
		if(uint(gl_VertexID) == 1u) vertex = vec4(-1.0, -1.0, 0.0, 1.0);
		if(uint(gl_VertexID) == 2u) vertex = vec4(1.0, 1.0, 0.0, 1.0);
		if(uint(gl_VertexID) == 3u) vertex = vec4(-1.0, 1.0, 0.0, 1.0);
		gl_Position = vertex;
	})");

	Shader fragment(GL_FRAGMENT_SHADER);
	fragment.loadFromSource(R"(
		#version 330 core
		uniform sampler2D tex;
		out vec4 fragColor;
		void main() {
			fragColor = texelFetch(tex, ivec2(gl_FragCoord.xy), 0);
	})");

	m_program = std::make_unique<Program>();
	m_program->attach(vertex).attach(fragment).link();
	m_program->bind();

	// generate empty vertex array object (to use glDrawArrays)
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

void Window::clear()
{
	memset(m_pixels.data(), 0, m_pixels.size() * sizeof(m_pixels[0]));
}

uint8_t Window::convertToBits(float r)
{
	return uint8_t(255.0f * glm::clamp(r, 0.0f, 1.0f));
}
#endif

void Window::setTitle(const std::string& title)
{
	glfwSetWindowTitle(m_handle, title.c_str());
}

void Window::changeMouseMode(const bool disableCursor) {
	if (disableCursor) {
		glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(m_handle, 0.0, 0.0);	//Reset x,y to [0,0]
		m_mouseDisabled = true;
	}
	else {
		glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_mouseDisabled = false;
	}
		
}

void Window::closeWindow() {
	glfwSetWindowShouldClose(m_handle, 1);
}

void Window::resize(size_t width, size_t height)
{
	if (width == 0 || height == 0)
		return; // ignore (minimize)

	m_width = width;
	m_height = height;

	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

#ifdef WINDOW_PUT_PIXEL
	m_pixels.resize(width * height * 3);

	// generate texture with window size
	if (m_texture)
		glDeleteTextures(1, &m_texture);
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, GLsizei(m_width), GLsizei(m_height), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif

	if (m_onSizeChange)
		m_onSizeChange(int(m_width), int(m_height));
}