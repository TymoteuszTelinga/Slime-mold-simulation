
#include "Simulation.h"
#include <imgui/imgui.h>

#include "Renderer/Image.h"
#include "Renderer/Program.h"
#include "Renderer/Buffer.h"
#include "Renderer/Renderer.h"

static Scope<Image> sImage;
static constexpr uint32_t NumberOfAgents = 10000;
static Scope<Buffer> sBuffer;
static Scope<Program> sProgram;
static Scope<Program> sEvaporate;

struct Agent
{
	float x, y, z, w;
};

float randomF()
{
	return (float)rand() / (float)RAND_MAX;
}

Simulation::Simulation(const ApplicationSpecification& spec)
	:Application(spec)
{
	sImage = CreateScope<Image>(spec.Width, spec.Height);
	//sImage = CreateScope<Image>(800, 800);
	sImage->Bind(0);
	sProgram = CreateScope<Program>("src\\Shaders\\TestAgents.glsl");
	sProgram->Bind();
	sProgram->SetUniform1ui("Agents", NumberOfAgents);

	sEvaporate = CreateScope<Program>("Src\\Shaders\\Evaporate.glsl");

	Agent *agents = new Agent[NumberOfAgents];
	for (size_t i = 0; i < NumberOfAgents; i++)
	{
		agents[i].x = rand() % sImage->GetWidth();
		agents[i].y = rand() % sImage->GetHeight();
		agents[i].z = randomF() * 3.14 * 2.f;
	}

	sBuffer = CreateScope<Buffer>(sizeof(Agent) * NumberOfAgents, 0);
	sBuffer->SetData(agents, sizeof(Agent) * NumberOfAgents);

	delete [] agents;

	Renderer::Resize(spec.Width, spec.Height);
}

void Simulation::OnEvent(Event& e)
{
}

void Simulation::OnUpdate(float DeltaTime)
{
	m_Count++;
	m_CountTime += DeltaTime;

	if (m_CountTime >= 1.0 / 5.0)
	{
		m_FPS = (1.0 / m_CountTime) * m_Count;
		m_FrameTime = m_CountTime / m_Count * 1000;

		m_CountTime = 0.0f;
		m_Count = 0;
	}

	//Agents update
	sProgram->Bind();
	sProgram->Dispatch(NumberOfAgents /10, 1);
	//Evaporation
	sEvaporate->Bind();
	sEvaporate->Dispatch(sImage->GetWidth() / 8, sImage->GetHeight() / 8);
}

void Simulation::OnRender()
{
	Renderer::DisplayImage();

	ImGui::Begin("Controls");
	ImGui::Text("FPS %d", m_FPS);
	ImGui::Text("Frame Time %f ms", m_FrameTime);
	ImGui::Separator();
	ImGui::Text("Simulation space: %dx%d", sImage->GetWidth(), sImage->GetHeight());
	ImGui::Text("Agents: %d", NumberOfAgents);
	ImGui::End();

	//ImGui::Begin("VP");
	//uint32_t textureID = sImage->GetRendererID();
	//ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(sImage->GetWidth(), sImage->GetHeight()), ImVec2{0,1}, ImVec2{1,0});
	//ImGui::End();

}
