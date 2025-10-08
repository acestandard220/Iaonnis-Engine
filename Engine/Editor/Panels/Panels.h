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
			
			virtual void OnUpdate(float dt) = 0;

		protected:
			std::string name;
			bool active;		

			Editor* editor;
			static int _id;
	};
}