#include "application.h"

#include <Windows.h>
#include <commdlg.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>

namespace Iaonnis
{

	GLuint CreateShaderProgram(const char* vertexPath, const char* fragmentPath)
	{
		// Helper to read file contents
		auto readFile = [](const char* path) -> std::string {
			std::ifstream file(path, std::ios::in | std::ios::binary);
			if (!file) return "";
			std::string contents;
			file.seekg(0, std::ios::end);
			contents.resize(static_cast<size_t>(file.tellg()));
			file.seekg(0, std::ios::beg);
			file.read(&contents[0], contents.size());
			file.close();
			return contents;
			};

		std::string vertexCode = readFile(vertexPath);
		std::string fragmentCode = readFile(fragmentPath);

		if (vertexCode.empty() || fragmentCode.empty())
			return 0;

		auto checkShader = [](GLuint shader, const char* type) {
			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				GLint logLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
				std::string infoLog(logLength, ' ');
				glGetShaderInfoLog(shader, logLength, nullptr, &infoLog[0]);
				fprintf(stderr, "%s shader compilation failed:\n%s\n", type, infoLog.c_str());
			}
			};

		auto checkProgram = [](GLuint program) {
			GLint success;
			glGetProgramiv(program, GL_LINK_STATUS, &success);
			if (!success) {
				GLint logLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
				std::string infoLog(logLength, ' ');
				glGetProgramInfoLog(program, logLength, nullptr, &infoLog[0]);
				fprintf(stderr, "Program linking failed:\n%s\n", infoLog.c_str());
			}
			};

		// Compile vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const char* vSource = vertexCode.c_str();
		glShaderSource(vertexShader, 1, &vSource, nullptr);
		glCompileShader(vertexShader);
		checkShader(vertexShader, "Vertex");

		// Compile fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fSource = fragmentCode.c_str();
		glShaderSource(fragmentShader, 1, &fSource, nullptr);
		glCompileShader(fragmentShader);
		checkShader(fragmentShader, "Fragment");

		// Create shader program
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		checkProgram(shaderProgram);

		// Cleanup shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Check for link errors and return 0 if failed
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			glDeleteProgram(shaderProgram);
			return 0;
		}

		return shaderProgram;
	}

	Application* Application::self = nullptr;

	void Iaonnis::Application::InitializeApplication()
	{
		if (glfwInit() != GLFW_TRUE)
			return;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

		auto monitor = glfwGetPrimaryMonitor();
		auto viewMode = glfwGetVideoMode(monitor);

		windowWidth = viewMode->width;
		windowHeight = viewMode->height;

		window = glfwCreateWindow(windowWidth, windowHeight, "Engine", nullptr, nullptr);

		HWND hwnd = glfwGetWin32Window(window);
		BOOL value = TRUE;
		DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));


		if (!window)
			return;
		glfwMakeContextCurrent(window);

#ifdef NDEBUG
		glfwSwapInterval(1);
#else
		glfwSwapInterval(0);
#endif

		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			const auto& bytes = glewGetErrorString(err);
			std::string errorString = std::string((const char*) &bytes);
			IAONNIS_LOG_ERROR("Failed to initialize GLEW. Error: %s\n", errorString.c_str());
		}

		const auto& bytes = glewGetString(GLEW_VERSION);
		std::string glewVersion = std::string((const char*)&bytes);
		IAONNIS_LOG_INFO("Using GLEW: %s", glewVersion.c_str());

		IAONNIS_ASSERT(GLEW_ARB_bindless_texture, "Bindless Textures are not supported on device.");
		IAONNIS_ASSERT(GLEW_ARB_shader_draw_parameters, "MultiDrawIndirect is not supported on device.");
		

		glfwSetFramebufferSizeCallback(window, window_resize_callback);
		glfwSetWindowCloseCallback(window, window_close_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetScrollCallback(window, scroll_callback);

		EventBus::subscribe(EventType::MOUSE_CLICKED_EVENT, std::bind(&Application::onMouseClickDispatch, this, std::placeholders::_1));
		EventBus::subscribe(EventType::MOUSE_MOVE_EVENT, std::bind(&Application::onMouseMoveDispatch, this, std::placeholders::_1));
		EventBus::subscribe(EventType::KEY_PRESS_EVENT, std::bind(&Application::onKeyPressedEvent, this, std::placeholders::_1));
		EventBus::subscribe(EventType::MOUSE_SCROLLED_EVENT, std::bind(&Application::onMouseScrollDispatch, this, std::placeholders::_1));

		program = CreateShaderProgram("Assets/Shaders/vertex.glsl", "Assets/Shaders/fragment.glsl");
		glUseProgram(program);

		scene = std::make_shared<Scene>("Scene");
		editor = std::make_shared<Editor>(window, scene);

		Iaonnis::Renderer3D::Initialize(program);

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_FRAMEBUFFER_SRGB);

		//glEnable(GL_CULL_FACE);         // Enable face culling
		//glCullFace(GL_BACK);            // Cull back faces (default)
		//glFrontFace(GL_CCW);
		//

		self = this;
	}

	void Iaonnis::Application::OnUpdate()
	{
		while (!glfwWindowShouldClose(window))
		{
			//DISABLE_SCOPE_TIMER_PRINT
			{
				SCOPE_TIMER("SCENE_ONUPDATE");
				scene->OnUpdate(0.2f);
			}

			{
				SCOPE_TIMER("RENDER_ONLY");
				Renderer3D::RenderScene(scene.get(), program);
			}

			//ENABLE_SCOPE_TIMER_PRINT
			{
				SCOPE_TIMER("EDITOR_UPDATE");
				editor->OnUpdate(Renderer3D::GetRenderStats(), Renderer3D::GetRenderOutput());
			}

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}


	void Iaonnis::Application::Shutdown()
	{
		editor->ShutDown();
		Iaonnis::Renderer3D::Shutdown();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Application::window_resize_callback(GLFWwindow* window, int x, int y)
	{

	}

	void Application::closeApp(Event& event)
	{
		ExitAppEvent* exitEvent = (ExitAppEvent*)&event;
		if (exitEvent->promptStatus & PROMPT_FLAG_YES)
			glfwSetWindowShouldClose(window, GL_TRUE);
		else
			glfwSetWindowShouldClose(window, GL_FALSE);
	}
	void Application::window_close_callback(GLFWwindow* window)
	{
		PromptEvent closePrompt("Iaonnis Engine", "Are you sure you want to close the application?");
		closePrompt.promptType = PROMPT_FLAG_DEFAULT;
		closePrompt.promptType |= PROMPT_FLAG_YES | PROMPT_FLAG_BUTTON_HORIZONTAL_ALIGN | PROMPT_FLAG_NO
			| PROMPT_FLAG_CANCEL | PROMPT_FLAG_MODAL | PROMPT_FLAG_TEXT_CENTERED;
		closePrompt.responseEventType = EventType::EXIT_APP_EVENT;

		EventBus::subscribe(EventType::EXIT_APP_EVENT, std::bind(&Application::closeApp, self, std::placeholders::_1));
		EventBus::publish(closePrompt);

		//glfwSetWindowShouldClose(window, GL_FALSE);
	}

	void Application::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		MouseMoveEvent event;
		event.position = { (float)xpos,(float)ypos };
		event.delta = event.position - self->inputState.lastMousePosition;

		EventBus::publish(event);
	}

	void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		KeyPressEvent event;
		event.keyCode = key;
		event.scanCode = scancode;
		event.action = action;
		event.mods = mods;


		auto camera = self->scene->GetSceneCamera();
		auto frustrum = camera->getFrustrum();

		float cameraSpeed = 0.5f;
		glm::vec3 direction = glm::normalize(frustrum.target - frustrum.position);
		glm::vec3 right = glm::normalize(glm::cross(direction, frustrum.up));
		glm::vec3 strafe = right * cameraSpeed;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->updatePosition(direction * cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->updatePosition(-direction * cameraSpeed);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera->setPosition(frustrum.position - strafe);
			camera->setTarget(frustrum.target - strafe);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera->setPosition(frustrum.position + strafe);
			camera->setTarget(frustrum.target + strafe);
		}

		IAONNIS_LOG_INFO("Camera Location:: %d ,%d, %d", frustrum.position.x, frustrum.position.g, frustrum.position.z);

		EventBus::publish(event);
	}

	void Application::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		MouseClickedEvent mouseEvent;
		mouseEvent.button = button;
		mouseEvent.action = action;
		mouseEvent.mods = mods;

		EventBus::publish(mouseEvent);
	}

	void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		MouseScrollEvent event;
		event.offset = { (float)xoffset,(float)yoffset };

		EventBus::publish(event);
	}

	void Application::onMouseScrollDispatch(Event& event)
	{
		MouseScrollEvent* mouseScroll = (MouseScrollEvent*)&event;
		if (editor->GetViewPortAction() == ViewPortAction::Idle)
		{
			auto camera = scene->GetSceneCamera();
			Frustrum frustrum = camera->getFrustrum();

			glm::vec3 viewDirection = camera->getViewDirection();
			glm::vec3 currentPosition = frustrum.position;
			glm::vec3 newPosition = currentPosition + viewDirection * mouseScroll->offset.y * 0.1f;
			camera->setPosition(newPosition);
		}
	}

	void Application::onMouseClickDispatch(Event& event)
	{
		

		MouseClickedEvent* mouseEvent = (MouseClickedEvent*)&event;
		
	}

	void Application::onMouseMoveDispatch(Event& event)
	{
		MouseMoveEvent* mouseEvent = (MouseMoveEvent*)&event;

		if (editor->GetViewPortAction() == ViewPortAction::Orbit)
		{
			auto camera = scene->GetSceneCamera();
			auto frustrum = camera->getFrustrum();

			float delta_angleX = (2 * glm::pi<float>()) / 1600.0f;
			float delta_angleY = glm::pi<float>() / 900.0f;

			float angleX = (inputState.lastMousePosition.x - mouseEvent->position.x) * delta_angleX;
			float angleY = (inputState.lastMousePosition.y - mouseEvent->position.y) * delta_angleY;

			// Clamp Y rotation to prevent flipping
			float cosAngle = glm::dot(camera->getViewDirection(), glm::vec3(0.0, 1.0, 0.0));
			if (cosAngle * glm::sign<float>(angleY) > 0.99f)
			{
				angleY = 0.0f; // Prevent rotation instead of setting to 1.0f
			}

			// Calculate position relative to target
			glm::vec4 relativePosition = glm::vec4(frustrum.position, 1.0f) - glm::vec4(frustrum.target, 1.0f);

			// Apply Y rotation (around right vector) first
			glm::mat4 rotation_matY = glm::rotate(glm::mat4(1.0f), angleY, camera->getRightVector());
			relativePosition = rotation_matY * relativePosition;

			glm::mat4 rotation_matX = glm::rotate(glm::mat4(1.0f), angleX, frustrum.up); 
			relativePosition = rotation_matX * relativePosition;

			// Set final position
			glm::vec3 newPosition = glm::vec3(relativePosition) + frustrum.target;
			camera->setPosition(newPosition);

			inputState.lastMousePosition.x = mouseEvent->position.x;
			inputState.lastMousePosition.y = mouseEvent->position.y;
		}

		if (editor->GetViewPortAction() == ViewPortAction::Pan)
		{
			auto camera = scene->GetSceneCamera();
			auto frustrum = camera->getFrustrum();

			float diffX = mouseEvent->position.x - inputState.lastMousePosition.x;
			float diffY = mouseEvent->position.y - inputState.lastMousePosition.y;

			inputState.lastMousePosition = mouseEvent->position;

			// Scale panning speed relative to camera distance from target
			float distance = glm::length(frustrum.position - frustrum.target);
			float panSpeed = distance * 0.001f; // smaller = slower pan

			glm::vec3 direction = glm::normalize(frustrum.target - frustrum.position);
			glm::vec3 camera_right = glm::normalize(glm::cross(frustrum.target - frustrum.position, frustrum.up));
			glm::vec3 camera_up = -glm::normalize(glm::cross(camera_right, direction));
			glm::vec3 displacement = -(camera_right * diffX + camera_up * diffY) * panSpeed;

			camera->updatePosition(displacement);
			camera->updateTarget(displacement);
		}

		inputState.lastMousePosition.x = mouseEvent->position.x;
		inputState.lastMousePosition.y = mouseEvent->position.y;
	}

	void Application::onKeyPressedEvent(Event& event)
	{
		KeyPressEvent* keyEvent = (KeyPressEvent*)&event;
		
		auto camera = scene->GetSceneCamera();
		auto frustrum = camera->getFrustrum();
		
		/*float cameraSpeed = 0.0000005f;
		if (keyEvent->keyCode == GLFW_KEY_W && keyEvent->action == GLFW_PRESS)
			camera->updatePosition(frustrum.target * cameraSpeed);
		if (keyEvent->keyCode == GLFW_KEY_S && keyEvent->action == GLFW_PRESS)
			camera->updatePosition(-frustrum.target * cameraSpeed);
		if (keyEvent->keyCode == GLFW_KEY_A && keyEvent->action == GLFW_PRESS)
			camera->updatePosition(-glm::normalize(glm::cross(frustrum.target, frustrum.up)) * cameraSpeed);
		if (keyEvent->keyCode == GLFW_KEY_D && keyEvent->action == GLFW_PRESS)
			camera->updatePosition(glm::normalize(glm::cross(frustrum.target, frustrum.up)) * cameraSpeed);*/
	}
}
