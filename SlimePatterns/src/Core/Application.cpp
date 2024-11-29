#include "Application.h"
#include "Renderer/Renderer.h"

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification& spec)
	:m_Specification(spec)
{
	s_Instance = this;

	m_Window = CreateScope<Window>(m_Specification.Width, m_Specification.Height);//Window::Create();
	//m_Window->Init();
	m_Window->SetEventCallback(BIND_EVENT(Application::OnEventApp));

	Renderer::Init();

	//setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	//setup ImGui style
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 5.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

Application::~Application()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//m_Window->Shutdown();
}

void Application::Run()
{
	m_Runing = true;
	ImGuiIO& io = ImGui::GetIO();

	const float maxUpdateTime = 1.f / (m_Specification.MinUpdateFrameRate-2);

	//main loop
	m_LastTime = glfwGetTime();
	while (m_Window->IsOpen() && m_Runing)
	{
		m_Window->OnUpdate();

		if (!m_Minimized)
		{
			int32_t loopcount = 0;
			while (m_DeltaTime > maxUpdateTime)
			{
				OnUpdate(maxUpdateTime);
				m_DeltaTime -= maxUpdateTime;
				loopcount++;
				if (loopcount >= 10)
				{
					break;
				}
			}

			OnUpdate(m_DeltaTime >= maxUpdateTime ? maxUpdateTime : m_DeltaTime);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

			Renderer::Clear();
			OnRender();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
		}

		float time = glfwGetTime();
		m_DeltaTime = time - m_LastTime;
		m_LastTime = time;
	}
}

void Application::ResetDelta()
{
	float time = glfwGetTime();
	m_DeltaTime = time - m_LastTime;
	m_LastTime = time;
}

void Application::OnEventApp(Event& e)
{
	EventDispatcher Dispacher(e);
	Dispacher.Dispatch<WindowResizeEvent>(BIND_EVENT(ResizeVieport));
	Dispacher.Dispatch<WindowMovedEvent>(BIND_EVENT(WindowMoved));

	OnEvent(e);

}

bool Application::ResizeVieport(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimized = true;
		return false;
	}

	m_Minimized = false;
	Renderer::Resize(e.GetWidth(), e.GetHeight());

	return false;
}

bool Application::WindowMoved(WindowMovedEvent& e)
{
	ResetDelta();
	return false;
}
