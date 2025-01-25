
#include "Simulation.h"
#include <imgui/imgui.h>

#include "Renderer/Image.h"
#include "Renderer/Program.h"
#include "Renderer/Buffer.h"
#include "Renderer/Renderer.h"

#include "Core/FileDialog.h"

#include <stb_image.h>
#include <iostream>
#include <algorithm>

//Images
static Ref<Image> sImage;
static Ref<Image> sImageDisplay;
static Ref<Image> sImageBoundry;
//Agenst arrays
static uint32_t NumberOfAgents = 500000;
static uint32_t AgentsSize = NumberOfAgents;
static int32_t sNumberOfSpecies = 3;
static int32_t ActiveSpecies = sNumberOfSpecies;
static Scope<Buffer> sBuffer;
//Steps
static Scope<Program> sProgram;
static Scope<Program> sEvaporate;
static Scope<Program> sDifiuse;
static Scope<Program> sFood;
//Uniform Buffer
static Scope<Buffer> sParamsBuffer;
static Scope<Buffer> sSpeciesBuffer;

enum class ESpawnMetod : int
{
	Circle = 0,
	Full = 1,
	Custom = 2
};

static ESpawnMetod sSpawnMetod = ESpawnMetod::Circle;

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
	vec4 SpeciesMask = vec4(1.f);
	float SensorAngle = 3.1415f / 8.f;
	float SensorOffset = 9.f;
	int32_t SensorSize = 1; ///Haf size
	float AgentSpeed = 1.f;
	float AgentRotation = 3.1415f / 8.f;
	float Deposit = 1.f;
	float __Padding1;
	float __Padding2;
};

static SpeciesSettings sSpecies[3];

struct SimulationParameters
{
	float TimeStep = 1.f;
	int32_t Width = 400;
	int32_t Height = 400;
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

uint32_t randomRange(uint32_t min, uint32_t max)
{
	const uint32_t range = max - min;
	return  rand() % range + min;
}

float RadianToDegrees(float angle)
{
	return (angle * 180.f / 3.1415f);
}

float DegreesToRadian(float angle)
{
	return (angle * 3.1415f / 180.f);
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

	//float* PixelBuffer = nullptr;
	//int32_t width, height, BPP;

	stbi_ldr_to_hdr_gamma(1.f);
	stbi_set_flip_vertically_on_load(1);
	//PixelBuffer = stbi_loadf("map.png", &width, &height, &BPP, 4);

	sImage = CreateRef<Image>(sParameters.Width, sParameters.Height, 0);
	sImageDisplay = CreateRef<Image>(sParameters.Width, sParameters.Height, 1);
	sImageBoundry = CreateRef<Image>(sParameters.Width, sParameters.Height, 2);
	sImageBoundry->Bind(1);
	sEvaporate = CreateScope<Program>("Src\\Shaders\\Evaporate.glsl");
	sDifiuse = CreateScope<Program>("Src\\Shaders\\Difiuse.glsl");
	sFood = CreateScope<Program>("Src\\Shaders\\Food.glsl");
	sProgram = CreateScope<Program>("src\\Shaders\\TestAgents.glsl");
	sParamsBuffer = CreateScope<Buffer>(sizeof(SimulationParameters), 1);
	sParamsBuffer->SetData(&sParameters, sizeof(SimulationParameters));
	sSpeciesBuffer = CreateScope<Buffer>(sizeof(sSpecies), 2);
	sSpeciesBuffer->SetData(&sSpecies, sizeof(sSpecies));

	InitSimulation(nullptr);

	//stbi_image_free(PixelBuffer);

	Renderer::Resize(spec.Width, spec.Height);

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
	//FoodFeromons
	//sFood->Bind();
	//sFood->Dispatch(1, 1);
	//m_Timer->Clear();
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
	static bool bGrayScale = true;
	Renderer::DisplayImage(sImage, ImageMask, bGrayScale);

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

		ImGui::PushItemWidth(70);

		if (ImGui::Button("Open"))
		{
			std::string path = FileDialog::OpenFile("Enviroment (*.png)\0*.png\0");
			if (!path.empty())
			{
				int32_t width, height, BPP;
				float* PixelBuffer = nullptr;
				m_EnvironmentPath = path;
				PixelBuffer = stbi_loadf(m_EnvironmentPath.c_str(), &width, &height, &BPP, 4);
				sParameters.Width = width;
				sParameters.Height = height;

				InitSimulation(PixelBuffer);

				stbi_image_free(PixelBuffer);
			}
		}

		ImGui::Text("Simulation space: %dx%d", sImage->GetWidth(), sImage->GetHeight());
		ImGui::Text("Agents: %d", AgentsSize);
		ImGui::Text("Species: %d", ActiveSpecies);
		static uint32_t ImageWidth = sImage->GetWidth();
		static uint32_t ImageHeight = sImage->GetHeight();
		ImGui::DragScalar("##Width", ImGuiDataType_U32, &ImageWidth);
		ImGui::SameLine();
		ImGui::DragScalar("Size", ImGuiDataType_U32, &ImageHeight);

		ImGui::DragScalar("Agents", ImGuiDataType_U32, &NumberOfAgents, 1000);
		ImGui::DragInt("Species", &sNumberOfSpecies, 0.2, 1, 3);

		static int CurentMetod = static_cast<int>(sSpawnMetod);
		if (ValidPositions.empty())
		{
			CurentMetod = static_cast<int>(ESpawnMetod::Circle);
		}
		if (ImGui::Combo("Spawn metod", &CurentMetod, "Circle\0Full\0Custom\0\0"))
		{
			sSpawnMetod = static_cast<ESpawnMetod>(CurentMetod);
		}


		if (ImGui::Button("Apply"))
		{
			AgentsSize = NumberOfAgents;
			ActiveSpecies = sNumberOfSpecies;
			sParameters.Width = ImageWidth;
			sParameters.Height = ImageHeight;
			m_EnvironmentPath = "";
			InitSimulation(nullptr);
		}

		ImGui::Separator();
		bool bParametrChange = false;
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
			SpeciesIndex = std::min(++SpeciesIndex, ActiveSpecies-1);
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
		bSettingsChange |= ImGui::DragFloat("Deposit", &Settings.Deposit, 0.1);
		ImGui::PopItemWidth();
		constexpr float SMin = -1.f;
		constexpr float SMax =  1.f;
		bSettingsChange |= ImGui::DragScalarN("Mask", ImGuiDataType_Float, Settings.SpeciesMask.v, ActiveSpecies, 0.1f, &SMin, &SMax, "%.2f");
		if (bSettingsChange)
		{
			sSpeciesBuffer->SetData(&Settings, sizeof(SpeciesSettings), sizeof(SpeciesSettings) * SpeciesIndex);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Simulation Controls"))
	{
		if (ImGui::Button("Reset"))
		{
			InitSimulation(nullptr);
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

		ImGui::Checkbox("GrayScale", &bGrayScale);
		static bool r = true, g = true, b = true;
		if (!bGrayScale)
		{
			ImGui::Text("Slime msak");
			ImGui::Checkbox("Species 1", &r);
			ImGui::Checkbox("Species 2", &g);
			ImGui::Checkbox("Species 3", &b);
			ImageMask = vec3(r, g, b);
		}

		static vec3 color;
		ImGui::ColorEdit3("SlimeColor", &color.r);

		ImGui::TreePop();
	}
	
	ImGui::End();

}

void Simulation::InitSimulation(float* EnvironmentData)
{
	ParseImage(EnvironmentData);

	sParamsBuffer->SetData(&sParameters, sizeof(SimulationParameters));

	sImage->Resize(sParameters.Width, sParameters.Height);
	sImageDisplay->Resize(sParameters.Width, sParameters.Height);

	sImageBoundry->Resize(sParameters.Width, sParameters.Height, EnvironmentData);
	sImageBoundry->Bind(1);

	sProgram->Bind();
	sProgram->SetUniform1ui("Agents", AgentsSize);

	InitAgents();
}

bool ComperPositions(const vec3& a, const vec3& b)
{
	return a.b < b.b;
}

void Simulation::ParseImage(float* EnvironmentData)
{
	ValidPositions.clear();
	if (EnvironmentData == nullptr)
	{
		sSpawnMetod = ESpawnMetod::Circle;
		return;
	}


	float* pixel = EnvironmentData;
	m_S1Points = m_S2Points = m_S3Points = 0;

	for (uint32_t y = 0; y < sParameters.Height; y++)
	{
		for (uint32_t x = 0; x < sParameters.Width; x++)
		{
			//uint8_t r = (*(pixel)) * 255;
			//uint8_t g = *(pixel + 1) * 255;
			uint8_t b = *(pixel + 2) * 255;
			// 1 1 1 11111
			// 1 0 0 00000 - species 1 (128)
			// 0 1 0 00000 - species 2 ( 64)
			// 0 0 1 00000 - species 3 ( 32)
			// 
			// 1 0 1 00000 - species 1 | 3 (160)
			if (b > 0)
			{
				if (b & uint8_t(128))
				{
					ValidPositions.push_back(vec3(x, y, 0));
					m_S1Points++;

				}
				if (b & uint8_t(64))
				{
					ValidPositions.push_back(vec3(x, y, 2));
					m_S2Points++;
				}
				if (b & uint8_t(32))
				{
					ValidPositions.push_back(vec3(x, y, 3));
					m_S3Points++;
				}
			}

			pixel += 4;
		}
	}

	std::sort(ValidPositions.begin(), ValidPositions.end(), ComperPositions);
	if (ValidPositions.size() <= 0)
	{
		sSpawnMetod = ESpawnMetod::Circle;
	}
}

void Simulation::InitAgents()
{
	const float R = std::min(sImage->GetWidth(), sImage->GetHeight()) / 2.f;

	Agent* agents = new Agent[AgentsSize];

	if (!ValidPositions.empty())
	{
		sSpawnMetod = ESpawnMetod::Custom;
	}

	for (uint32_t i = 0; i < AgentsSize; i++)
	{
		//SetUp agent species
		switch (rand() % ActiveSpecies)
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

		//SetUp agent positions
		switch (sSpawnMetod)
		{
		case ESpawnMetod::Circle:
		{
			float r = R * std::sqrt(randomF());
			float theta = randomF() * 2 * 3.14;
			agents[i].x = sImage->GetWidth() / 2.f + r * std::cos(theta);
			agents[i].y = sImage->GetHeight() / 2.f + r * std::sin(theta);
			agents[i].angle = randomF() * 2 * 3.14;
		}
		break;
		case ESpawnMetod::Full:
		{
			agents[i].x = rand() % sImage->GetWidth();
			agents[i].y = rand() % sImage->GetHeight();
			agents[i].angle = randomF() * 3.14 * 2.f;
		}
		break;
		case ESpawnMetod::Custom:
		{
			uint32_t index = 0;
			switch (agents[i].SpeciesIndex)
			{
			case 0: 
				index = randomRange(0,m_S1Points);
				break;
			case 1:
				index = randomRange(m_S1Points, m_S1Points + m_S2Points);
				break;
			case 2:
				index = randomRange(m_S1Points + m_S2Points, ValidPositions.size());
			}
			vec3 position = ValidPositions[index];

			agents[i].x = position.r;
			agents[i].y = position.g;
			agents[i].angle = randomF() * 3.14 * 2.f;
		}
		default:
			break;
		}


		//agents[i].r = 1;
		//agents[i].g = 1;
		//agents[i].b = 1;
		agents[i].a = 0;
	}

	sBuffer = CreateScope<Buffer>(sizeof(Agent) * AgentsSize, 0);
	sBuffer->SetData(agents, sizeof(Agent) * AgentsSize);
	delete[] agents;
}
