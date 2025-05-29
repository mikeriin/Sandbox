#include "window.h"


#include <iostream>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>

#include "globals.h"
#include "event_dispatcher.h"


namespace mk
{
	int gfxApi = MK_GFXAPI_OPENGL;





	Window::~Window()
	{
		if (m_sdlGLContext)
		{
			SDL_GL_DestroyContext(m_sdlGLContext);
			m_sdlGLContext = nullptr;
		}

		if (m_sdlWindowHandle)
		{
			SDL_DestroyWindow(m_sdlWindowHandle);
			m_sdlWindowHandle = nullptr;
		}

		SDL_Quit();
	}



	bool Window::Create(const mk::WindowCreateInfo& windowInfo)
	{
		m_name = windowInfo.name;
		m_width = windowInfo.width;
		m_height = windowInfo.height;


		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			std::cerr << "Failed to init SDL3: " << SDL_GetError() << "\n";
			return false;
		}


		uint32_t windowFlags;
		
		switch (windowInfo.gfxApi)
		{
		case mk::GfxApi::Vulkan:
			std::cerr << "Vulkan is not supported for now\n";

			windowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;

			return false;
			break;

		case mk::GfxApi::OpenGL:
		default:
			mk::gfxApi = MK_GFXAPI_OPENGL;

			windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

#ifdef NDEBUG
			uint32_t ctxFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#else
			uint32_t ctxFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG;
#endif // NDEBUG

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, ctxFlags);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

			break;
		}

		m_sdlWindowHandle = SDL_CreateWindow(
			m_name.c_str(),
			m_width,
			m_height,
			windowFlags
		);

		if (!m_sdlWindowHandle)
		{
			std::cerr << "Failed to create sdl window: " << SDL_GetError() << "\n";
			return false;
		}


		switch (mk::gfxApi)
		{
		case MK_GFXAPI_OPENGL:
		default:

			m_sdlGLContext = SDL_GL_CreateContext(m_sdlWindowHandle);

			if (!m_sdlGLContext)
			{
				std::cerr << "Failed to create gl context: " << SDL_GetError() << "\n";
				return false;
			}

			SDL_GL_MakeCurrent(m_sdlWindowHandle, m_sdlGLContext);

			break;
		}

		m_isOpen = true;

		return true;
	}

	void Window::PollEvents(EventDispatcher& dispatcher)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_EVENT_QUIT:
			{
				mk::QuitEvent quitEvent{};
				dispatcher.Dispatch(quitEvent);
			}
			break;

			case SDL_EVENT_WINDOW_RESIZED:
			{
				mk::WindowResizedEvent wndResizedEvent{ (uint32_t)e.window.data1, (uint32_t)e.window.data2 };
				dispatcher.Dispatch(wndResizedEvent);
			}
			break;

			case SDL_EVENT_KEY_DOWN:
			{
				if (e.key.repeat) break;

				mk::KeyDownEvent keyDownEvent{ e.key.key };
				dispatcher.Dispatch(keyDownEvent);
			}
			break;

			case SDL_EVENT_KEY_UP:
			{
				mk::KeyUpEvent keyUpEvent{ e.key.key };
				dispatcher.Dispatch(keyUpEvent);
			}
			break;

			case SDL_EVENT_MOUSE_MOTION:
			{
				mk::MouseMotionEvent mouseMotionEvent{
					e.motion.x,
					e.motion.y,
					e.motion.xrel,
					e.motion.yrel,
					e.motion.state
				};
				dispatcher.Dispatch(mouseMotionEvent);
			}
			break;

			default:
				break;
			}
		}

	}

	void Window::SwapBuffers() const
	{
		SDL_GL_SwapWindow(m_sdlWindowHandle);
	}

	void Window::OnWindowResizedEvent(const mk::Event& e)
	{
		const mk::WindowResizedEvent& wndResizedEvent = static_cast<const mk::WindowResizedEvent&>(e);
		m_width = wndResizedEvent.width;
		m_height = wndResizedEvent.height;
	}

}