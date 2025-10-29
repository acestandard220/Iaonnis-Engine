#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"

namespace Iaonnis
{
	class Editor;
	class EditorPanel
	{
		public:
			EditorPanel(Editor* editor);
			virtual ~EditorPanel();
			
			bool& Active() { return active; }
			virtual void ToggleActive() { active = active ? false : true; }

			virtual std::string GetName() { return name; }

			virtual void OnUpdate(float dt) = 0;

		protected:
			std::string name;
			bool active;		

			Editor* editor;
			static int _id;
	};
}