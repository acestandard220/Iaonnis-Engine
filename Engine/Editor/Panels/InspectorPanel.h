#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"
#include "Panels.h"
#include "../Scene/Entity.h"

namespace Iaonnis
{
	class Inspector : public EditorPanel
	{
	public:
		Inspector(Editor* editor);

		void InspectEntity(Entity* entity);

		virtual void OnUpdate(float dt)override;
	};
}