#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"
#include "Panels.h"
#include "../Scene/Entity.h"

namespace Iaonnis
{
	namespace GeneralWindow
	{
		void Initialize(Editor* editor);

		namespace CacheViewer {
			template <typename T>
			void OnRender();

			void SetActive(std::function<void(UUID id)>);
			void SetInactive();

		}
	}

}