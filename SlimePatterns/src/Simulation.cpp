
#include "Simulation.h"
#include <imgui/imgui.h>

#include "Renderer/Image.h"
#include "Renderer/Program.h"
#include "Renderer/Buffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"

#include "Core/FileDialog.h"

#include <stb_image.h>
#include <stb_image_write.h>
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
//Drawing
static Ref<Shader> sPostProces;
//Uniform Buffer
static Scope<Buffer> sParamsBuffer;
static Scope<Buffer> sSpeciesBuffer;

struct Gradient
{
	vec3 Low;
	vec3 High;
	std::string name;
};

static Gradient gradients[12];

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
	vec4 ActiveSpeciesMask = vec4(1.f);
	int32_t RandSeed = 0;
	float TimeStep = 1.f;
	int32_t Width = 400;
	int32_t Height = 400;
	//Pheromons params
	int32_t KernelSize = 1; //Haf size
	float EvaporationFactor = 0.1f;
	float DifiusonFactor = 1.f;
};

static SimulationParameters sParameters;

static float clamp(float val)
{
	return std::min(std::max(0.f, val), 1.f);
}

uint32_t nLehmer = 0;
const uint32_t lehmerMax = std::numeric_limits<uint32_t>::max();
uint32_t lehmer32()
{
	nLehmer += 0xe120fc15;
	uint64_t tmp;
	tmp = (uint64_t)nLehmer * 0x4a39b70d;
	uint32_t m1 = (tmp >> 32) ^ tmp;
	tmp = (uint64_t)m1 * 0x12fad5c9;
	uint32_t m2 = (tmp >> 32) ^ tmp;
	return m2;
}

float randomF()
{
	return (float)rand() / (float)RAND_MAX;
}

uint32_t randomRange(uint32_t min, uint32_t max)
{
	const uint32_t range = max - min;
	return  lehmer32() % range + min;
}

float RadianToDegrees(float angle)
{
	return (angle * 180.f / 3.1415f);
}

float DegreesToRadian(float angle)
{
	return (angle * 3.1415f / 180.f);
}

static void InitGradients()
{
	gradients[0].Low  = vec3(0.972, 0.607, 0.160);
	gradients[0].High = vec3(1.000, 0.058, 0.482);
	gradients[0].name = "gradient 1";

	gradients[1].Low  = vec3(0.894, 0.654, 0.772);
	gradients[1].High = vec3(0.427, 0.317, 0.647);
	gradients[1].name = "gradient 2";

	gradients[2].Low  = vec3(0.909, 0.109, 1.000);
	gradients[2].High = vec3(0.250, 0.788, 1.000);
	gradients[2].name = "gradient 3";

	gradients[3].Low  = vec3(0.000, 1.000, 0.529);
	gradients[3].High = vec3(0.376, 0.937, 1.000);
	gradients[3].name = "gradient 4";

	gradients[4].Low  = vec3(1.000, 0.105, 0.419);
	gradients[4].High = vec3(0.270, 0.792, 1.000);
	gradients[4].name = "gradient 5";

	gradients[5].Low  = vec3(0.078, 0.321, 0.466);
	gradients[5].High = vec3(0.513, 0.815, 0.796);
	gradients[5].name = "gradient 6";

	gradients[6].Low  = vec3(0.709, 0.776, 0.878);
	gradients[6].High = vec3(0.921, 0.956, 0.960);
	gradients[6].name = "gradient 7";

	gradients[7].Low  = vec3(0.901, 0.137, 0.078);
	gradients[7].High = vec3(0.945, 0.619, 0.094);
	gradients[7].name = "gradient 8";

	gradients[8].Low  = vec3(0.741, 0.654, 0.203);
	gradients[8].High = vec3(0.968, 0.949, 0.670);
	gradients[8].name = "gradient 9";

	gradients[9].Low  = vec3(0.007, 0.011, 0.266);
	gradients[9].High = vec3(0.156, 0.721, 0.835);
	gradients[9].name = "gradient 10";

	gradients[10].Low  = vec3(0.231, 0.341, 0.000);
	gradients[10].High = vec3(0.784, 1.000, 0.321);
	gradients[10].name = "gradient 11";

	gradients[11].Low = vec3(0, 0, 0);
	gradients[11].High = vec3(1, 1, 1);
	gradients[11].name = "Custom";
}

Simulation::Simulation(const ApplicationSpecification& spec)
	:Application(spec)
{
	bVsync = GetWindow().IsVSync();
	stbi_ldr_to_hdr_gamma(1.f);
	stbi_set_flip_vertically_on_load(1);

	sImage = CreateRef<Image>(sParameters.Width, sParameters.Height, 0);
	m_ImageWidth = sParameters.Width;
	m_ImageHeight = sParameters.Height;
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

	sPostProces = CreateRef<Shader>("src\\Shaders\\vertex.glsl", "src\\Shaders\\fragment.glsl");
	sPostProces->Bind();
	sPostProces->SetUniform1i("GrayScale", 1.f);
	sPostProces->SetUniform3f("Mask", 1.f, 1.f, 1.f);
	sPostProces->SetUniform1f("ActiveSpices", 3.f);
	sPostProces->SetUniform3f("ColorLow", m_ColorLow);
	sPostProces->SetUniform3f("ColorHigh", m_ColorHigh);

	InitSimulation(nullptr);

	Renderer::Resize(spec.Width, spec.Height);

	m_Timer = CreateScope<Timer>();

	//Style setup
	InitGradients();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_PickerHueWheel);
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
	sParameters.RandSeed++;
	sParamsBuffer->SetData(&sParameters.RandSeed, sizeof(int), sizeof(vec4));

	//Agents update
	m_Timer->Clear();
	sProgram->Bind();
	sProgram->Dispatch(AgentsSize / 64, 1);
	m_AgentTime = m_Timer->ElapsedMiliseconds();
	//Difiusion
	sDifiuse->Bind();
	sDifiuse->Dispatch(sImage->GetWidth() / 8 + 1, sImage->GetHeight() / 8 + 1);
	m_DifiuseTime = m_Timer->ElapsedMiliseconds();
	//swap images
	Image::SwapBinding(*sImage.get(), *sImageDisplay.get());
	//Evaporation
	m_Timer->Clear();
	sEvaporate->Bind();
	sEvaporate->Dispatch(sImage->GetWidth() / 8 + 1, sImage->GetHeight() / 8 + 1);
	m_EvaporateTime = m_Timer->ElapsedMiliseconds();

}

void Simulation::OnRender()
{
	static bool bGrayScale = true;

	Renderer::DisplayImage(sImage, sPostProces);
	sPostProces->SetUniform1f("ActiveSpices", ActiveSpecies);

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
		ImGui::Text("Simulation space: %dx%d", sParameters.Width, sParameters.Height);
		ImGui::Text("Agents: %d", AgentsSize);
		ImGui::Text("Species: %d", ActiveSpecies);
		ImGui::Text("Enviroment: %s", m_EnvironmentPath.empty() ? "None" : m_EnvironmentPath.c_str());


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
				m_ImageWidth = sParameters.Width = width;
				m_ImageHeight = sParameters.Height = height;

				InitSimulation(PixelBuffer);

				stbi_image_free(PixelBuffer);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			m_EnvironmentPath.clear();
			InitSimulation(nullptr);
		}

		ImGui::DragScalar("##Width", ImGuiDataType_U32, &m_ImageWidth);
		ImGui::SameLine();
		ImGui::DragScalar("Size", ImGuiDataType_U32, &m_ImageHeight);

		ImGui::DragScalar("Agents", ImGuiDataType_U32, &NumberOfAgents, 1000);
		ImGui::DragInt("Species", &sNumberOfSpecies, 0.2, 1, 3);

		int CurentMetod = static_cast<int>(sSpawnMetod);
		if (ImGui::Combo("Spawn metod", &CurentMetod, "Circle\0Full\0Custom\0\0"))
		{
			sSpawnMetod = static_cast<ESpawnMetod>(CurentMetod);
		}

		if (ImGui::Button("Apply"))
		{
			AgentsSize = NumberOfAgents;
			ActiveSpecies = sNumberOfSpecies;
			sParameters.ActiveSpeciesMask = vec4(1.f, ActiveSpecies >= 2 ? 1 : 0, ActiveSpecies >= 3 ? 1 : 0, 0.f);
			if (sParameters.Width != m_ImageWidth || sParameters.Height != m_ImageHeight)
			{
				sParameters.Width = m_ImageWidth;
				sParameters.Height = m_ImageHeight;
				m_EnvironmentPath = "";
				InitSimulation(nullptr);
			}
			else
			{
				int32_t width, height, BPP;
				float* PixelBuffer = nullptr;
				PixelBuffer = stbi_loadf(m_EnvironmentPath.c_str(), &width, &height, &BPP, 4);
				InitSimulation(PixelBuffer);
				stbi_image_free(PixelBuffer);
			}
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
			if (!m_EnvironmentPath.empty())
			{
				int32_t width, height, BPP;
				float* PixelBuffer = nullptr;
				PixelBuffer = stbi_loadf(m_EnvironmentPath.c_str(), &width, &height, &BPP, 4);
				sParameters.Width = width;
				sParameters.Height = height;

				InitSimulation(PixelBuffer);

				stbi_image_free(PixelBuffer);
			}
			else
			{
				InitSimulation(nullptr);
			}
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

		if (ImGui::Button("Save image"))
		{
			std::string path = FileDialog::SaveFile("Image (*.bmp)\0*.bmp\0");
			if (!path.empty())
			{
				Image::SaveImage(*sImage.get(), path);
			}

		}

		if (ImGui::Checkbox("Gradient", &bGrayScale))
		{
			sPostProces->SetUniform1i("GrayScale", bGrayScale);
		}

		static bool r = true, g = true, b = true;
		if (!bGrayScale)
		{
			ImGui::Text("Slime msak");
			bool maskChange = false;
			maskChange |= ImGui::Checkbox("Species 1", &r);
			maskChange |= ImGui::Checkbox("Species 2", &g);
			maskChange |= ImGui::Checkbox("Species 3", &b);
			if (maskChange)
			{
				sPostProces->SetUniform3f("Mask", r, g, b);
			}
		}
		else
		{
			ImGui::PushItemWidth(160);
			static int CurentGradient = 10;
			std::string CurentName = gradients[CurentGradient].name;
			if (ImGui::BeginCombo("Color", CurentName.c_str()))
			{
				for (int n = 0; n < 12; n++)
				{
					const bool bSelected = (CurentGradient == n);
					std::string name = gradients[n].name;

					if (ImGui::Selectable(name.c_str(), bSelected))
					{
						CurentGradient = n;
						m_ColorLow = gradients[n].Low;
						m_ColorHigh = gradients[n].High;
						sPostProces->SetUniform3f("ColorLow", m_ColorLow);
						sPostProces->SetUniform3f("ColorHigh", m_ColorHigh);
					}

					if (bSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::ColorEdit3("Low Intensity", &m_ColorLow.r))
			{
				CurentGradient = 11;
				gradients[CurentGradient].Low = m_ColorLow;
				gradients[CurentGradient].High = m_ColorHigh;
				sPostProces->SetUniform3f("ColorLow", m_ColorLow);
			}
			if (ImGui::ColorEdit3("High Intensity", &m_ColorHigh.r))
			{
				CurentGradient = 11;
				gradients[CurentGradient].Low = m_ColorLow;
				gradients[CurentGradient].High = m_ColorHigh;
				sPostProces->SetUniform3f("ColorHigh", m_ColorHigh);
			}

			ImGui::PopItemWidth();
		}

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
		if (sSpawnMetod == ESpawnMetod::Custom)
		{
			sSpawnMetod = ESpawnMetod::Circle;
		}
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
			uint8_t b = clamp(*(pixel + 2)) * 255.f;
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
					ValidPositions.push_back(vec3(x, y, 1));
					m_S2Points++;
				}
				if (b & uint8_t(32))
				{
					ValidPositions.push_back(vec3(x, y, 2));
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
	else
	{
		sSpawnMetod = ESpawnMetod::Custom;
	}
}

void Simulation::InitAgents()
{
	const float R = std::min(sImage->GetWidth(), sImage->GetHeight()) / 6.f;

	Agent* agents = new Agent[AgentsSize];

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
		agents[i].a = 0;

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

	}

	sBuffer = CreateScope<Buffer>(sizeof(Agent) * AgentsSize, 0);
	sBuffer->SetData(agents, sizeof(Agent) * AgentsSize);
	delete[] agents;
}
