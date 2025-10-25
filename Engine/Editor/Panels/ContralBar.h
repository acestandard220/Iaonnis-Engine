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

	private:
		

	};
}
