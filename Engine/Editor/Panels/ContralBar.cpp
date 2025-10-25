#include "ContralBar.h"
#include "../Editor.h"
#include "../Style.h"

namespace Iaonnis
{
	ControlBar::ControlBar(Editor* editr)
		:EditorPanel(editr)
	{
		name = "Control Panel";
		active = true;
	}

	void Iaonnis::ControlBar::OnUpdate(float dt)
	{
		auto io = ImGui::GetIO();

		ImGuiWindowFlags fl = 0;
		fl |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;

		ImVec2 regionAvail = ImGui::GetContentRegionAvail();
		ImVec2 padding = ImVec2(1200.0f, 20.0f);

		ImVec2 barSize = ImVec2(regionAvail.x - padding.x, regionAvail.y - padding.y);

		ImGui::SetNextWindowSize(barSize);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5);


		ImGui::Begin("##ToolControl", nullptr, fl);

		if (ImGui::IsWindowHovered())
		{
			auto windowRounding = ImGui::GetStyle().WindowRounding;
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->Flags &= ~ImDrawListFlags_AntiAliasedLines;
			ImVec2 min = window->Pos;
			ImVec2 max = ImVec2(min.x + window->Size.x, min.y + window->Size.y);
			draw_list->AddRect(min, max, IM_COL32(255, 255, 255, 90),windowRounding, ImDrawFlags_RoundCornersAll, 2.0f);
		}

		ImVec2 buttonSize = ImVec2(40, 35);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImGuiEx::tightButtonPadding);

		//Projec Tab
		{
			if (ImGuiEx::ImageButton("##NewButton", ResourceCache::GetIcon(IconType::New)->getTextureHandle().m_ID,
				buttonSize, ImDrawFlags_RoundCornersLeft))
			{
				Scene* scene = editor->GetScene();
				scene->Save(scene->getName() + ".yaml");
				editor->CreateScene();
			}

			ImGui::SameLine();
			if (ImGuiEx::ImageButton("##OpenButton", ResourceCache::GetIcon(IconType::Open)->getTextureHandle().m_ID,
				buttonSize))
			{


			}

			ImGui::SameLine();
			if (ImGuiEx::ImageButton("##SaveButton", ResourceCache::GetIcon(IconType::Save)->getTextureHandle().m_ID,
				buttonSize, ImDrawFlags_RoundCornersRight))
			{
				std::string returnPath = FileDialog::SaveFileDialog();
				if (!returnPath.empty())
				{
					editor->GetScene()->Save(returnPath);
				}
			}
		}

		ImGuiEx::VSeparator();

		//Scene Managerment
		{
			if (ImGuiEx::ImageButton("##EntityViewerButton", ResourceCache::GetIcon(IconType::Entities)->getTextureHandle().m_ID,
				buttonSize, ImDrawFlags_RoundCornersLeft))
			{
			}
			ImGui::SameLine();
			if (ImGuiEx::ImageButton("##ResourceViewerButton", ResourceCache::GetIcon(IconType::Resources)->getTextureHandle().m_ID,
				buttonSize, ImDrawFlags_RoundCornersRight))
			{

			}
		}

		ImGuiEx::VSeparator();

		ImGuiEx::ImageButton("##CaptureButton", ResourceCache::GetIcon(IconType::Capture)->getTextureHandle().m_ID,
			buttonSize, ImDrawFlags_RoundCornersAll);

		ImGuiEx::VSeparator();

		//Modes
		{
			if (ImGuiEx::ImageButton("##SolidModeButton", ResourceCache::GetIcon(IconType::SolidMode)->getTextureHandle().m_ID,
				buttonSize, ImDrawFlags_RoundCornersLeft))
			{

			}
			ImGui::SameLine();
			if (ImGuiEx::ImageButton("##RenderModeButton", ResourceCache::GetIcon(IconType::RenderMode)->getTextureHandle().m_ID,
				buttonSize))
			{

			}
			ImGui::SameLine();
			if (ImGuiEx::ImageButton("##RenderOptionsButton", ResourceCache::GetIcon(IconType::RenderOption)->getTextureHandle().m_ID,
				buttonSize, ImDrawFlags_RoundCornersRight))
			{

			}
		}

		ImGuiEx::VSeparator();

		ImGui::BeginGroup();
		ImGui::Text("Info...");
		if (ImGui::IsItemHovered())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0, 5.0));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);

			ImGui::BeginTooltip();
			ImGui::Text("[Iaonnis Dev v0.1]");
			ImGui::Separator();
			ImGui::Text("FPS: %.3f", io.Framerate);
			ImGui::Text("Delta Time: %.3f XX", io.DeltaTime);
			ImGui::EndTooltip();
			ImGui::PopStyleVar(3);
		}
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::PopStyleVar();

		
		ImGui::End();

		ImGui::PopStyleVar();

	
	}

	void ControlBar::OnProjectTab()
	{
		if(ImGui::BeginPopup("##ProjectTabPopUp"))
		{
			if (ImGui::BeginMenu("Flame"))
			{
				ImGui::MenuItem("Fale");
				ImGui::MenuItem("Flame");
				ImGui::MenuItem("Felame");

				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}
