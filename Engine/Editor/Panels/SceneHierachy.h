#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"
#include "Panels.h"
#include "../Scene/Entity.h"

namespace Iaonnis
{
	class SceneHierachy : public EditorPanel
	{
		public:
			SceneHierachy(Editor* editor);
			virtual ~SceneHierachy();

			void function1(Entity entity);

			virtual void OnUpdate(float dt)override;
		private:
			void OnPopUpContext();

	};

}
