#ifndef MK_RENDERER_H
#define MK_RENDERER_H


#include "window.h"


namespace mk
{


	struct RendererCreateInfo
	{
		mk::Window* pWindow;
	};



	class Renderer
	{
	private:
		mk::Window* m_WindowHandle = nullptr;


	public:

		Renderer() = default;
		~Renderer();


		bool Create(const mk::RendererCreateInfo& rendererInfo);

		void OnWindowResizedEvent(const mk::Event& e);
		void OnKeyDownEvent(const mk::Event& e) const;
	};


}



#endif // !MK_RENDERER_H
