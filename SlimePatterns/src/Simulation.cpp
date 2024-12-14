
#include "Simulation.h"
#include <imgui/imgui.h>

#include "Renderer/Image.h"
#include "Renderer/Program.h"
#include "Renderer/Buffer.h"
#include "Renderer/Renderer.h"

//Images
static Ref<Image> sImage;
static Ref<Image> sImageDisplay;
//Agenst arrays
static uint32_t NumberOfAgents = 10000;
static uint32_t AgentsSize = NumberOfAgents;
static Scope<Buffer> sBuffer;
//Steps
static Scope<Program> sProgram;
static Scope<Program> sEvaporate;
static Scope<Program> sDifiuse;
//Uniform Buffer
static Scope<Buffer> sParamsBuffer;
static Scope<Buffer> sSpeciesBuffer;

struct Agent
{
	float x, y; //vec2
	float angle;
	int32_t SpeciesIndex = 0;
	int32_t r = 0, g = 0, b = 0, a = 0; //ivec4
};

struct SpeciesSettings
{
	//Agent params
	float SensorAngle = 3.1415f / 8.f;
	float SensorOffset = 9.f;
	int32_t SensorSize = 1; ///Haf size
	float AgentSpeed = 1.f;
	float AgentRotation = 3.1415f / 8.f;
};

static SpeciesSettings sSpecies[3];

struct SimulationParameters
{
	float TimeStep = 1.f;
	int32_t Width = 1920;
	int32_t Height = 1080;
	//Pheromons params
	int32_t KernelSize = 1; //Haf size
	float EvaporationFactor = 0.1f;
	float DifiusonFactor = 1.f;
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
	for (uint32_t i = 0; i < AgentsSize; i++)
	{
		//Random
		
		agents[i].x = rand() % sImage->GetWidth();
		agents[i].y = rand() % sImage->GetHeight();
		agents[i].angle = randomF() * 3.14 * 2.f;
		

		//circle
		//float R = std::min(sImage->GetWidth(), sImage->GetHeight()) / 2.f;
		//float r = R * std::sqrt(randomF());
		//float theta = randomF() * 2 * 3.14;
		//agents[i].x = sImage->GetWidth() / 2.f + r * std::cos(theta);
		//agents[i].y = sImage->GetHeight() / 2.f + r * std::sin(theta);
		//agents[i].angle = randomF() * 2 * 3.14;
		switch (rand() % 3)
		{
		case 0:
			agents[i].r = 1;
			agents[i].SpeciesIndex = 0;
			break;
		case 1:
			agents[i].g = 1;
			agents[i].SpeciesIndex = 1;
			break;
		case 2:
			agents[i].b = 1;
			agents[i].SpeciesIndex = 2;
			break;
		}
		//agents[i].r = 1;
		//agents[i].g = 1;
		//agents[i].b = 1;
		agents[i].a = 0;
	}
}

Simulation::Simulation(const ApplicationSpecification& spec)
	:Application(spec)
{
	bVsync = GetWindow().IsVSync();

	//sParameters.Width = spec.Width/2;
	//sParameters.Height = spec.Height/2;

	//4k
	/*sParameters.Width = 3840;
	sParameters.Height = 2160;*/


	sImage = CreateRef<Image>(sParameters.Width, sParameters.Height, 0);
	sImageDisplay = CreateRef<Image>(sParameters.Width, sParameters.Height, 1);
	sEvaporate = CreateScope<Program>("Src\\Shaders\\Evaporate.glsl");
	sDifiuse = CreateScope<Program>("Src\\Shaders\\Difiuse.glsl");
	sProgram = CreateScope<Program>("src\\Shaders\\TestAgents.glsl");
	sParamsBuffer = CreateScope<Buffer>(sizeof(SimulationParameters), 1);
	sParamsBuffer->SetData(&sParameters, sizeof(SimulationParameters));
	sSpeciesBuffer = CreateScope<Buffer>(sizeof(sSpecies), 2);
	sSpeciesBuffer->SetData(&sSpecies, sizeof(sSpecies));

	InitSimulation();

	Renderer::Resize(spec.Width, spec.Height);
	//Renderer::Resize(600, 600);

	m_Timer = CreateScope<Timer>();
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
	//sParameters.TimeStep = DeltaTime;
	//sParamsBuffer->SetData(&sParameters, sizeof(SimulationParameters));

	
	//Agents update
	m_Timer->Clear();
	sProgram->Bind();
	sProgram->Dispatch(AgentsSize / 64, 1);
	m_AgentTime = m_Timer->ElapsedMiliseconds();
	//Difiusion
	sDifiuse->Bind();
	sDifiuse->Dispatch(sImage->GetWidth() / 8, sImage->GetHeight() / 8);
	m_DifiuseTime = m_Timer->ElapsedMiliseconds();
	//swap images
	Image::SwapBinding(*sImage.get(), *sImageDisplay.get());
	//Evaporation
	m_Timer->Clear();
	sEvaporate->Bind();
	sEvaporate->Dispatch(sImage->GetWidth() / 8, sImage->GetHeight() / 8);
	m_EvaporateTime = m_Timer->ElapsedMiliseconds();

}

void Simulation::OnRender()
{
	Renderer::DisplayImage(sImage);


	ImGui::Begin("Controls");

	if (ImGui::TreeNode("Rendering"))
	{
		ImGui::Text("FPS %d", m_FPS);
		ImGui::Text("Frame Time %f ms", m_FrameTime);
		ImGui::Text("Update Time %f ms", m_AgentTime);
		ImGui::Text("Difiuse Time %f ms", m_DifiuseTime);
		ImGui::Text("Evaporate Time %f ms", m_EvaporateTime);

		if (ImGui::Checkbox("VSync", &bVsync))
		{
			GetWindow().SetVSync(bVsync);
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Simulation parameters"))
	{

		ImGui::Text("Simulation space: %dx%d", sImage->GetWidth(), sImage->GetHeight());
		ImGui::Text("Agents: %d", AgentsSize);
		ImGui::DragScalar("Agents", ImGuiDataType_U32, &NumberOfAgents, 1000);
		if (ImGui::Button("Apply"))
		{
			AgentsSize = NumberOfAgents;
			InitSimulation();
		}

		ImGui::PushItemWidth(60);
		ImGui::Separator();
		bool bParametrChange = false;
		//ImGui::DragScalar("Number of agents", ImGuiDataType_U32, )
		bParametrChange |= ImGui::DragInt("Kernel size", &sParameters.KernelSize, 0.5,0, 10);
		bParametrChange |= ImGui::DragFloat("Evaporation factor", &sParameters.EvaporationFactor, 0.01, 0.f, 1.f, "%.2f");
		bParametrChange |= ImGui::DragFloat("Difiuson factor", &sParameters.DifiusonFactor, 0.01, 0.f, 1.f, "%.2f");
		if (bParametrChange)
		{
			sParamsBuffer->SetData(&sParameters, sizeof(SimulationParameters));
		}

		ImGui::Separator();
		bool bSettingsChange = false;
		static int32_t SpeciesIndex = 0;
		if (ImGui::Button("<"))
		{
			SpeciesIndex = std::max(--SpeciesIndex, 0);
		}
		ImGui::SameLine();
		ImGui::Text("Species: %d", SpeciesIndex);
		ImGui::SameLine();
		if (ImGui::Button(">"))
		{
			SpeciesIndex = std::min(++SpeciesIndex, 2);
		}
		SpeciesSettings& Settings = sSpecies[SpeciesIndex];
		float SensorDegrees = RadianToDegrees(Settings.SensorAngle);
		bSettingsChange |= ImGui::DragFloat("Sensor angle", &SensorDegrees, 1.f, 0.f, 360.f, "%.1f");
		Settings.SensorAngle = DegreesToRadian(SensorDegrees);
		bSettingsChange |= ImGui::DragFloat("Sensor offset", &Settings.SensorOffset, 0.01, 0.f, 100.f, "%.2f");
		bSettingsChange |= ImGui::DragInt("Sensor size", &Settings.SensorSize, 0.2, 0, 10);
		bSettingsChange |= ImGui::DragFloat("Agent speed", &Settings.AgentSpeed, 0.01, 0.f, 100.f, "%.2f");
		float RotationDegrees = RadianToDegrees(Settings.AgentRotation);
		bSettingsChange |= ImGui::DragFloat("Agent rotation", &RotationDegrees, 1.f, 0.f, 360.f, "%.1f");
		Settings.AgentRotation = DegreesToRadian(RotationDegrees);
		if (bSettingsChange)
		{
			sSpeciesBuffer->SetData(&Settings, sizeof(SpeciesSettings), sizeof(SpeciesSettings) * SpeciesIndex);
		}

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Simulation Controls"))
	{
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
	sImage->Resize(sParameters.Width, sParameters.Height);
	sImageDisplay->Resize(sParameters.Width, sParameters.Height);

	sProgram->Bind();
	sProgram->SetUniform1ui("Agents", AgentsSize);

	Agent* agents = new Agent[AgentsSize];
	InitAgents(agents);
	sBuffer = CreateScope<Buffer>(sizeof(Agent) * AgentsSize, 0);
	sBuffer->SetData(agents, sizeof(Agent) * AgentsSize);

	delete[] agents;
}
