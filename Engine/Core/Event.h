#pragma once
#include "pch.h"

namespace Iaonnis {

#define IAONNISE_PUBLISH_EVENT(event) EventBus::publish(event);
#define IAONNIS_SUBSCRIBE_EVENT(event_type,func) EventBus::subscribe(event_type,func);
	enum class EventType
	{
		LOG_EVENT,
		PROMPT_EVENT,
		RESIZE_EVENT,
		EXIT_APP_EVENT,
		KEY_PRESS_EVENT,
		MOUSE_MOVE_EVENT,
		MOUSE_CLICKED_EVENT,
		MOUSE_SCROLLED_EVENT,
	};

	struct Event
	{
		EventType eventType;
		bool handled = false;

		Event(EventType type)
			:eventType(type)
		{

		}
		Event(const Event& other)
			:eventType(other.eventType), handled(other.handled)
		{
		}
	};

	struct KeyPressEvent : public Event
	{
		KeyPressEvent()
			: Event(EventType::KEY_PRESS_EVENT)
		{

		}
		int keyCode;
		int scanCode;
		int action;
		int mods;
	};

	struct MouseScrollEvent : public Event
	{
		MouseScrollEvent()
			:Event(EventType::MOUSE_SCROLLED_EVENT)
		{

		}
		glm::vec2 offset;
	};

	struct MouseMoveEvent : public Event
	{
		MouseMoveEvent()
			:Event(EventType::MOUSE_MOVE_EVENT)
		{

		}

		glm::vec2 position;
		glm::vec2 delta;
	};

	struct MouseClickedEvent : public Event
	{
		MouseClickedEvent()
			:Event(EventType::MOUSE_CLICKED_EVENT)
		{

		}

		int button;
		int action;
		int mods;
	};

	struct LogEvent :public Event
	{
		std::string message;
		LogEvent(const char* msg)
			:Event(EventType::LOG_EVENT)
		{
			message = msg;
		}
	};

	struct FrameResizeEvent : public Event
	{
		float frameSizeX;
		float frameSizeY;

		FrameResizeEvent()
			:Event(EventType::RESIZE_EVENT)
		{

		}
	};

	enum PROMPT_FLAGS : uint32_t
	{
		PROMPT_FLAG_DEFAULT = 0,
		PROMPT_FLAG_YES = 1 << 0,
		PROMPT_FLAG_OKAY = 1 << 1,
		PROMPT_FLAG_ABORT = 1 << 2,
		PROMPT_FLAG_RETRY = 1 << 3,
		PROMPT_FLAG_CLOSE = 1 << 4,
		PROMPT_FLAG_CANCEL = 1 << 5,
		PROMPT_FLAG_IGNORE = 1 << 6,
		PROMPT_FLAG_CONTINUE = 1 << 7,
		PROMPT_FLAG_YES_TO_ALL = 1 << 8,
		PROMPT_FLAG_NO = 1 << 9,

		PROMPT_FLAG_MODAL = 1 << 10,
		PROMPT_FLAG_TEXT_CENTERED = 1 << 11,
		PROMPT_FLAG_BUTTON_HORIZONTAL_ALIGN = 1 << 12,
	};

	struct PromptEvent : public Event
	{
		std::string message;
		std::string name;

		PromptEvent(const char* promptName, const char* msg)
			:Event(EventType::PROMPT_EVENT)
		{
			message = msg;
			name = promptName;
			promptType = PROMPT_FLAG_OKAY;
		}

		EventType responseEventType;

		using PromptType = int;
		PromptType promptType;

	};


	struct Feedback : public Event
	{
		Feedback(EventType type)
			:Event(type)
		{

		}
		PROMPT_FLAGS promptStatus;
		Feedback(const Feedback& other)
			:Event(other.eventType), promptStatus(other.promptStatus)
		{

		}
	};

	struct ExitAppEvent : public Feedback
	{
		ExitAppEvent()
			:Feedback(EventType::EXIT_APP_EVENT)
		{

		}
	};

	class EventBus
	{
	public:
		using Listener = std::function<void(Event&)>;

		static void subscribe(EventType eventType, Listener listener);
		static void publish(Event& event);
		static void publish(Feedback& event);

	private:
		static std::unordered_map<EventType, std::vector<Listener>>m_Listeners;
	};

	class EventFeedback
	{
	public:

		static Feedback createFeedBack(EventType type)
		{
			switch (type)
			{
			case EventType::EXIT_APP_EVENT:
			{
				return ExitAppEvent();
			}
			default: return ExitAppEvent();
			}
		}
	};
}