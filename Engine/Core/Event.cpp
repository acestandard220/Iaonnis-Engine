#include "event.h"

namespace Iaonnis {

	std::unordered_map<EventType, std::vector<EventBus::Listener>>EventBus::m_Listeners;

	void EventBus::subscribe(EventType eventType, Listener listener)
	{
		m_Listeners[eventType].push_back(listener);
	}


	void EventBus::publish(Event& event)
	{
		auto subs = m_Listeners[event.eventType];
		for (auto& listener : subs)
		{
			listener(event);

			if (event.handled)break;
		}
		return;
	}

	void EventBus::publish(Feedback& event)
	{
		auto subs = m_Listeners[event.eventType];
		for (auto& listener : subs)
		{
			listener(event);

			if (event.handled)break;
		}
		return;
	}
}