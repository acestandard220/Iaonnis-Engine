#pragma once
#include "Core/Core.h"
#include "Core/pch.h"


#include "Editor/editor.h"
#include "Renderer/Renderer.h"


namespace Iaonnis
{
	struct InputState
	{
		glm::vec2 lastMousePosition = glm::vec2(0.0f, 0.0f);
		glm::vec2 viewPortSize = glm::vec2(0.0f, 0.0f);
	};

	class Application
	{
	public:
		void InitializeApplication();
		void OnUpdate();
		void Shutdown();
	private:
		void closeApp(Event& event);
		static void window_resize_callback(GLFWwindow* window, int x, int y);
		static void window_close_callback(GLFWwindow* window);
		static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

		void onMouseScrollDispatch(Event& event);
		void onMouseClickDispatch(Event& event);
		void onMouseMoveDispatch(Event& event);
		void onKeyPressedEvent(Event& event);
	private:
		InputState inputState;
	private:
		std::shared_ptr<Editor> editor;
		std::shared_ptr<Scene> scene;
		uint32_t program;
	private:
		GLFWwindow* window;
		int windowWidth;
		int windowHeight;

		static Application* self;

	};

}
