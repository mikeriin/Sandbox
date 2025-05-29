#ifndef MK_EVENT_H
#define MK_EVENT_H


#include <cstdint>



namespace mk
{


	struct Event
	{
		enum class Type
		{
			KeyDown,
			KeyUp,
			WindowResized,
			MouseMotion,
			Quit,
		};

		virtual Type GetType() const = 0;
		virtual ~Event() = default;
	};


	struct MouseMotionEvent : public mk::Event
	{
		float x;
		float y;
		float xRel;
		float yRel;
		uint32_t state;

		MouseMotionEvent(float _x, float _y, float _xRel, float _yRel, uint32_t _state)
			: x(_x),
			y(_y),
			xRel(_xRel),
			yRel(_yRel),
			state(_state)
		{}

		Type GetType() const override { return Type::MouseMotion; }
	};


	struct WindowResizedEvent : public mk::Event
	{
		uint32_t width;
		uint32_t height;

		WindowResizedEvent(uint32_t _width, uint32_t _height)
			:	width(_width),
			height(_height)
		{}

		Type GetType() const override { return Type::WindowResized; }
	};


	struct QuitEvent : public mk::Event
	{
		QuitEvent() {}

		Type GetType() const override { return Type::Quit; }
	};


	struct KeyDownEvent : public mk::Event
	{
		uint32_t keycode;

		KeyDownEvent(uint32_t _keycode)
			:	keycode(_keycode) 
		{}

		Type GetType() const override { return Type::KeyDown; }
	};


	struct KeyUpEvent : public mk::Event
	{
		uint32_t keycode;

		KeyUpEvent(uint32_t _keycode)
			: keycode(_keycode)
		{}

		Type GetType() const override { return Type::KeyUp; }
	};


}




#endif // !MK_EVENT_H
