#include "Renderer.h"

#include "Core/Base.h"
#include "VertexArray.h"
#include "Shader.h"

#include <GL/glew.h>
#include <stdio.h>

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
	}
}

struct RendererData
{
	//Screan Quad
	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
};

static RendererData s_Data;
static Ref<Shader> ScreenShder;

void Renderer::Init()
{
	if (glewInit() != GLEW_OK)
		return;

	//setup OpenGl Debug Callback
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	//glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.f);

	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	VertexBufferLayout Layout;
	Layout.Pushf(2);//Position
	Layout.Pushf(2);//Text Cord
	s_Data.QuadVertexBuffer = CreateRef<VertexBuffer>(quadVertices, sizeof(quadVertices), Layout);
	s_Data.QuadVertexArray = CreateRef<VertexArray>();
	s_Data.QuadVertexArray->AddBuffer(s_Data.QuadVertexBuffer);

	ScreenShder = CreateRef<Shader>("src\\Shaders\\vertex.glsl", "src\\Shaders\\fragment.glsl");

}

void Renderer::Resize(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DisplayImage()
{
	ScreenShder->Bind();
	s_Data.QuadVertexArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
