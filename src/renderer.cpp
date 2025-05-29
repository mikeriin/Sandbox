#include "renderer.h"


#include <iostream>
#include <format>

#include <glad/glad.h>
#include <SDL3/SDL_keycode.h>

#include "globals.h"



// use nvdia gfx card only
extern "C" {
  __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}



static void APIENTRY glDebugOutput(
  GLenum source, 
  GLenum type, 
  GLuint id, 
  GLenum severity, 
  GLsizei length, 
  const GLchar* message, 
  const void* userParam
)
{
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  std::cerr << "---------------" << std::endl;
  std::cerr << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
  case GL_DEBUG_SOURCE_API:               std::cerr << "Source: API"; break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     std::cerr << "Source: Window System"; break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:   std::cerr << "Source: Shader Compiler"; break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:       std::cerr << "Source: Third Party"; break;
  case GL_DEBUG_SOURCE_APPLICATION:       std::cerr << "Source: Application"; break;
  case GL_DEBUG_SOURCE_OTHER:             std::cerr << "Source: Other"; break;
  } std::cerr << std::endl;

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error"; break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behaviour"; break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined Behaviour"; break;
  case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability"; break;
  case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance"; break;
  case GL_DEBUG_TYPE_MARKER:              std::cerr << "Type: Marker"; break;
  case GL_DEBUG_TYPE_PUSH_GROUP:          std::cerr << "Type: Push Group"; break;
  case GL_DEBUG_TYPE_POP_GROUP:           std::cerr << "Type: Pop Group"; break;
  case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other"; break;
  } std::cerr << std::endl;

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:            std::cerr << "Severity: high"; break;
  case GL_DEBUG_SEVERITY_MEDIUM:          std::cerr << "Severity: medium"; break;
  case GL_DEBUG_SEVERITY_LOW:             std::cerr << "Severity: low"; break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:    std::cerr << "Severity: notification"; break;
  } std::cerr << std::endl;
  std::cerr << std::endl;
}




namespace mk
{




	Renderer::~Renderer()
	{
		m_WindowHandle = nullptr;
	}


	bool Renderer::Create(const mk::RendererCreateInfo& rendererInfo)
	{
		m_WindowHandle = rendererInfo.pWindow;

		switch (mk::gfxApi)
		{
		case MK_GFXAPI_OPENGL:
		default:

			if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
			{
				std::cerr << "Failed to load gl\n";
				return false;
			}

#ifndef NDEBUG

      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(glDebugOutput, nullptr);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

#endif // !NDEBUG

      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			break;
		}

		
#ifndef NDEBUG
		const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		std::cout << std::format("OpenGL version {}\n", glVersion);
#endif // !NDEBUG


		return true;
	}

  void Renderer::OnWindowResizedEvent(const mk::Event& e)
  {
    const mk::WindowResizedEvent& wndResizedEvent = static_cast<const mk::WindowResizedEvent&>(e);
    glViewport(0, 0, wndResizedEvent.width, wndResizedEvent.height);
  }

  void Renderer::OnKeyDownEvent(const mk::Event& e) const
  {
    const mk::KeyDownEvent& keyDownEvent = static_cast<const mk::KeyDownEvent&>(e);
    if (keyDownEvent.keycode == SDLK_TAB)
    {
      static bool isWireframe = false;
      isWireframe = !isWireframe;
      glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
      //glLineWidth(isWireframe ? 2.0f : 1.0f); // only works on legacy opengl
    }
  }

}