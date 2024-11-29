#include "Window.h"

#include <iostream>

bool Window::Init()
{
	//Initialize the library
	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwWindowHint(GLFW_SAMPLES,8);

	//Create a windowed mode window and its OpenGL context
	m_Window = glfwCreateWindow(m_Data.width, m_Data.height, "Slime mold", NULL, NULL);
	if (!m_Window)
	{
		std::cout << "Createion error\n";
		return false;
	}

	//Make the window's context current
	glfwMakeContextCurrent(m_Window);

	glfwSetWindowUserPointer(m_Window, &m_Data);

	// Set GLFW callbacks
	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);	
			data.callback(event);
		});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoff, double yoff)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			ScrollEvent event(xoff, yoff);
			data.callback(event);
		});

	glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowMovedEvent event(xpos, ypos);
			data.callback(event);
		});

	//glfwSetWindowTitle();
	SetVSync(true);

	return true;
}

void Window::Shutdown()
{
	glfwTerminate();
}

void Window::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

bool Window::IsOpen() const
{
	return !glfwWindowShouldClose(m_Window);
}

void Window::SetVSync(bool enable)
{
	if (enable)
	{
		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(0);
	}

	m_Data.bVSync = enable;
}

void Window::SetEventCallback(const EventCallbackFn& callback)
{
	m_Data.callback = callback;
}

Window::Window(uint32_t width, uint32_t height)
	:m_Window(nullptr)
{
	m_Data.width = width;
	m_Data.height = height;

	Init();
}

Window::~Window()
{
	Shutdown();
}
