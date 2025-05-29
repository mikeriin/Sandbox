#ifndef MK_EVENT_DISPATCHER_H
#define MK_EVENT_DISPATCHER_H



#include <functional>
#include <unordered_map>
#include <vector>

#include "event.h"



namespace mk
{


	


	class EventDispatcher
	{
	private:
		using EventHandler = std::function<void(const mk::Event&)>;
		std::unordered_map<mk::Event::Type, std::vector<EventHandler>> m_handlers;


	public:
		static EventDispatcher& GetInstance();

		void RegisterHandler(mk::Event::Type type, EventHandler handler);
		void Dispatch(const mk::Event& e) const;


	private:
		EventDispatcher() {}

		EventDispatcher(const EventDispatcher&) = delete;
		EventDispatcher& operator=(const EventDispatcher&) = delete;
	};


}




#endif // !MK_EVENT_DISPATCHER_H

