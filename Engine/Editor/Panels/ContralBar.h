#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"
#include "Panels.h"
#include "../Scene/Entity.h"
#include "GeneralWindow.h"

namespace Iaonnis
{



	class ControlBar : public EditorPanel
	{
	public:
		ControlBar(Editor* editr);

		virtual void OnUpdate(float dt)override;

	private:
		void OnProjectTab();
		bool Toggle(int index);

	private:
		std::unique_ptr<ImGuiEx::ToggleState2> renderToggleState;
        std::unique_ptr<ImGuiEx::ToggleGroup> sceneToggleGroup;

		int currentToggle = -1;
		std::vector<bool> toggleStates;

	};
}
