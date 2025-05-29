#include "event_dispatcher.h"


namespace mk
{

  EventDispatcher& EventDispatcher::GetInstance()
  {
    static EventDispatcher instance;
    return instance;
  }


  void EventDispatcher::RegisterHandler(mk::Event::Type type, EventHandler handler)
  {
    m_handlers[type].push_back(handler);
  }


  void EventDispatcher::Dispatch(const mk::Event& e) const
  {
    auto it = m_handlers.find(e.GetType());

    if (it != m_handlers.end())
    {
      for (const auto& handler : it->second) handler(e);
    }
  }

}