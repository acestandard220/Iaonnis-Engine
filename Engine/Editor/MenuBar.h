#pragma once
#include "ImGuiEx/ImGui_Extensions.h"

namespace Iaonnis
{
	class Editor;
	class MenuBar
	{

	public:
		MenuBar(Editor* editor);
		~MenuBar();

		void OnUpdate();

	private:
		Editor* editor;
	};

}
