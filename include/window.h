#ifndef MK_WINDOW_H
#define MK_WINDOW_H


#include <cstdint>
#include <string>

#include <SDL3/SDL_video.h>
#include <glm.hpp>

#include "event.h"


namespace mk
{


	class EventDispatcher;


	enum class GfxApi
	{
		OpenGL,
		Vulkan
	};


	struct WindowCreateInfo
	{
		std::string name;
		uint32_t width;
		uint32_t height;
		mk::GfxApi gfxApi;
	};



	class Window
	{
	private:
		SDL_Window* m_sdlWindowHandle = nullptr;
		SDL_GLContext m_sdlGLContext = nullptr;
		std::string m_name = "";
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		bool m_isOpen = false;


	public:
		Window() = default;
		~Window();


		bool Create(const mk::WindowCreateInfo& windowInfo);

		void PollEvents(EventDispatcher& dispatcher);
		void SwapBuffers() const;

		inline bool IsOpen() const { return m_isOpen; }
		inline glm::vec2 GetExtent() const { return { (float)m_width, (float)m_height }; }

		inline void OnQuitEvent(const mk::Event& e) { m_isOpen = false; }
		void OnWindowResizedEvent(const mk::Event& e);
	};



}




#endif // !MK_WINDOW_H
