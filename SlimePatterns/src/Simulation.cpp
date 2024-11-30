
#include "Simulation.h"
#include <imgui/imgui.h>

#include "Renderer/Image.h"
#include "Renderer/Program.h"

static Scope<Image> sImage;
static Scope<Program> sProgram;

Simulation::Simulation(const ApplicationSpecification& spec)
	:Application(spec)
{
	sImage = CreateScope<Image>(80, 80);
	sProgram = CreateScope<Program>("src\\Shaders\\Test.glsl");
	sProgram->Bind();
}

void Simulation::OnEvent(Event& e)
{
}

void Simulation::OnUpdate(float DeltaTime)
{
	sProgram->Dispatch(10, 10);
}

void Simulation::OnRender()
{
	ImGui::Begin("Controls");
	ImGui::Text("Hello!");
	ImGui::End();

	ImGui::Begin("VP");
	uint32_t textureID = sImage->GetRendererID();
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(800,800), ImVec2{0,1}, ImVec2{1,0});
	ImGui::End();
}
