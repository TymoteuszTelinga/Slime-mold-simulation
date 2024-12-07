
#include "Simulation.h"
#include <imgui/imgui.h>

#include "Renderer/Image.h"
#include "Renderer/Program.h"
#include "Renderer/Buffer.h"
#include "Renderer/Renderer.h"

//Images
static uint32_t ImageWidth = 1920, ImageHeight = 1080;
static Scope<Image> sImage;
static Scope<Image> sImageDisplay;
//Agenst arrays
static constexpr uint32_t NumberOfAgents = 500000;
static Scope<Buffer> sBuffer;
//Steps
static Scope<Program> sProgram;
static Scope<Program> sEvaporate;
static Scope<Program> sDifiuse;
//Uniform Buffer
static Scope<Buffer> sParamsBuffer;

struct Agent
{
	float x, y, z, w;
};

struct SimulationParameters
{
	float TimeStep = 1.f;
	//Pheromons params
	int32_t KernelSize = 3; //Not Implemented
	float EvaporationFactor = 0.99f;
	//Agent params
	float SensorAngle = 3.1415f / 8.f;
	float SensorOffset = 9.f;
	int32_t SensorSize = 3; //NotImplemented
	float AgentSpeed = 1.f;
	float AgentRotation = 0.2f * 2.f * 3.14f;
};

static SimulationParameters sParameters;

float randomF()
{
	return (float)rand() / (float)RAND_MAX;
}

float RadianToDegrees(float angle)
{
	return (angle * 180.f / 3.1415f);
}

float DegreesToRadian(float angle)
{
	return (angle * 3.1415f / 180.f);
}

void InitAgents(Agent* agents)
{
	for (uint32_t i = 0; i < NumberOfAgents; i++)
	{
		//Random
		/*
		agents[i].x = rand() % sImage->GetWidth();
		agents[i].y = rand() % sImage->GetHeight();
		agents[i].z = randomF() * 3.14 * 2.f;
		*/

		//circle
		float R = std::min(sImage->GetWidth(), sImage->GetHeight()) / 2.f;
		float r = R * std::sqrt(randomF());
		float theta = randomF() * 2 * 3.14;
		agents[i].x = sImage->GetWidth() / 2.f + r * std::cos(theta);
		agents[i].y = sImage->GetHeight() / 2.f + r * std::sin(theta);
		agents[i].z = randomF() * 2 * 3.14;
	}
}

Simulation::Simulation(const ApplicationSpecification& spec)
	:Application(spec)
{
	bVsync = GetWindow().IsVSync();

	/*ImageWidth = spec.Width;
	ImageHeight = spec.Height;*/

	//4k
	/*ImageWidth = 3840;
	ImageHeight = 2160;*/

	sImage = CreateScope<Image>(ImageWidth, ImageHeight, 0);
	sImageDisplay = CreateScope<Image>(ImageWidth, ImageHeight, 1);
	sEvaporate = CreateScope<Program>("Src\\Shaders\\Evaporate.glsl");
	sDifiuse = CreateScope<Program>("Src\\Shaders\\Difiuse.glsl");
	sProgram = CreateScope<Program>("src\\Shaders\\TestAgents.glsl");
	sParamsBuffer = CreateScope<Buffer>(sizeof(SimulationParameters), 1);
	sParamsBuffer->SetData(&sParameters, sizeof(SimulationParameters));

	InitSimulation();

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

	if (bPouse && !bNextFrame)
	{
		return;
	}
	bNextFrame = false;

	
	//Agents update
	sProgram->Bind();
	sProgram->Dispatch(NumberOfAgents /128, 1);
	//Difiusion
	sDifiuse->Bind();
	sDifiuse->Dispatch(sImage->GetWidth() / 8, sImage->GetHeight() / 8);
	//swap images
	Image::SwapBinding(*sImage.get(), *sImageDisplay.get());
	//Evaporation
	sEvaporate->Bind();
	sEvaporate->Dispatch(sImage->GetWidth() / 8, sImage->GetHeight() / 8);

}

void Simulation::OnRender()
{
	static bool bDifiuseImage = false;
	if (bDifiuseImage)
	{
		sImageDisplay->Bind(0);
	}
	else
	{
		sImage->Bind(0);
	}
	Renderer::DisplayImage();


	ImGui::Begin("Controls");

	if (ImGui::TreeNode("Rendering"))
	{
		ImGui::Text("FPS %d", m_FPS);
		ImGui::Text("Frame Time %f ms", m_FrameTime);
		if (ImGui::Checkbox("VSync", &bVsync))
		{
			GetWindow().SetVSync(bVsync);
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Simulation parameters"))
	{
		bool bParametrChange = false;

		ImGui::Text("Simulation space: %dx%d", sImage->GetWidth(), sImage->GetHeight());
		ImGui::Text("Agents: %d", NumberOfAgents);

		ImGui::Separator();
		ImGui::PushItemWidth(60);
		bParametrChange |= ImGui::DragInt("Kernel size", &sParameters.KernelSize);
		bParametrChange |= ImGui::DragFloat("Evaporation factor", &sParameters.EvaporationFactor, 0.01, 0.f, 1.f, "%.2f");
	
		ImGui::Separator();
		float SensorDegrees = RadianToDegrees(sParameters.SensorAngle);
		bParametrChange |= ImGui::DragFloat("Sensor angle", &SensorDegrees, 1.f, 0.f, 360.f, "%.1f");
		sParameters.SensorAngle = DegreesToRadian(SensorDegrees);
		bParametrChange |= ImGui::DragFloat("Sensor offset", &sParameters.SensorOffset, 0.01, 0.f, 100.f, "%.2f");
		bParametrChange |= ImGui::DragInt("Sensor size", &sParameters.SensorSize);
		bParametrChange |= ImGui::DragFloat("Agent speed", &sParameters.AgentSpeed, 0.01, 0.f, 100.f, "%.2f");
		float RotationDegrees = RadianToDegrees(sParameters.AgentRotation);
		bParametrChange |= ImGui::DragFloat("Agent rotation", &RotationDegrees, 1.f, 0.f, 360.f, "%.1f");
		sParameters.AgentRotation = DegreesToRadian(RotationDegrees);

		if (bParametrChange)
		{
			sParamsBuffer->SetData(&sParameters, sizeof(SimulationParameters));
		}

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Simulation Controls"))
	{
		ImGui::Checkbox("DisplayDifiuse", &bDifiuseImage);
		if (ImGui::Button("Reset"))
		{
			InitSimulation();
		}
		ImGui::SameLine();
		if (ImGui::Button(bPouse ? "Start" : "Stop"))
		{
			bPouse = !bPouse;
		}
		ImGui::SameLine();
		if (ImGui::Button("Next"))
		{
			bNextFrame = true;
		}

		ImGui::TreePop();
	}
	
	ImGui::End();

}

void Simulation::InitSimulation()
{
	sImage->Resize(ImageWidth, ImageHeight);
	sImageDisplay->Resize(ImageWidth, ImageHeight);

	sProgram->Bind();
	sProgram->SetUniform1ui("Agents", NumberOfAgents);

	Agent* agents = new Agent[NumberOfAgents];
	InitAgents(agents);
	sBuffer = CreateScope<Buffer>(sizeof(Agent) * NumberOfAgents, 0);
	sBuffer->SetData(agents, sizeof(Agent) * NumberOfAgents);

	delete[] agents;
}
