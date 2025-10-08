#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"
#include "Panels.h"

namespace Iaonnis
{
	class ViewPort :public EditorPanel
	{
	public:
		ViewPort(Editor* editor);

		virtual void OnUpdate(float dt)override;
	private:
		ImVec2 lastViewPortSize;
	};
}