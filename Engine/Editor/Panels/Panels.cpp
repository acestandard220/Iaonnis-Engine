#include "Panels.h"
#include "../Editor.h"

namespace Iaonnis
{
	int EditorPanel::_id = 1;

	EditorPanel::EditorPanel(Editor* editor)
		:editor(editor)
	{
		name = "Editor Panel";
		active = false;
		
	}

	EditorPanel::~EditorPanel()
	{

	}
}
