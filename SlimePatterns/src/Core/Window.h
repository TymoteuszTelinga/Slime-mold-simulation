#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include "Base.h"
#include "Events/WindowEvent.h"

class Window
{
public:
	Window(uint32_t width, uint32_t height);
	~Window();

	using EventCallbackFn = std::function<void(Event&)>;

	bool Init();
	void Shutdown();
	void OnUpdate();
	bool IsOpen() const;
	void SetVSync(bool enable);
	bool IsVSync() const { return m_Data.bVSync; };

	void SetEventCallback(const EventCallbackFn& callback);
	GLFWwindow* GetWindow() const { return m_Window; }

private:
	struct WindowData
	{
		EventCallbackFn callback;
		uint32_t width;
		uint32_t height;
		bool bVSync;
	};

	WindowData m_Data;
	GLFWwindow* m_Window;
};