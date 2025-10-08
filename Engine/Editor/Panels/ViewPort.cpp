#include "ViewPort.h"
#include "../Editor.h"

namespace Iaonnis
{
	ViewPort::ViewPort(Editor* editor)
		:EditorPanel(editor)
	{
		name = "Viewport";
		active = true;
	}

	void ViewPort::OnUpdate(float dt)
	{
		ImGui::Begin(name.c_str(), &active);

		ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
		if (viewPortSize.x != lastViewPortSize.x || viewPortSize.y != lastViewPortSize.y)
		{
			lastViewPortSize = viewPortSize;
			FrameResizeEvent frameResizeEvent;
			frameResizeEvent.frameSizeX = viewPortSize.x;
			frameResizeEvent.frameSizeY = viewPortSize.y;

			EventBus::publish(frameResizeEvent);
		}

		ImGui::Image(editor->renderOut, lastViewPortSize, ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::IsWindowHovered())
		{
			ImGuiIO& io = ImGui::GetIO();

			if (io.MouseDown[ImGuiMouseButton_Middle])
			{
				editor->SetViewPortAction(ViewPortAction::Pan);
			}
			else if (io.MouseDown[ImGuiMouseButton_Right])
			{
				editor->SetViewPortAction(ViewPortAction::Orbit);
			}
			else
			{
				editor->SetViewPortAction(ViewPortAction::Idle);
			}
		}
		else
		{
			editor->SetViewPortAction(ViewPortAction::Inactive);
		}

		ImGui::End();
	}
}
