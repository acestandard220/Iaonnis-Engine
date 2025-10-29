#include "ContralBar.h"
#include "../Editor.h"
#include "../Style.h"

namespace Iaonnis
{
	bool temp = false;

	ControlBar::ControlBar(Editor* editr)
		:EditorPanel(editr)
	{
		name = "Control Panel";
		active = true;

		auto solidModeIcon = ResourceCache::GetIcon(IconType::SolidMode)->getTextureHandle().m_ID;
		auto renderModeIcon = ResourceCache::GetIcon(IconType::RenderMode)->getTextureHandle().m_ID;

		renderToggleState = std::make_unique<ImGuiEx::ToggleState2>(solidModeIcon, renderModeIcon, ImDrawFlags_RoundCornersLeft, ImDrawFlags_RoundCornersRight);
		sceneToggleGroup  = std::make_unique<ImGuiEx::ToggleGroup>("##SceneToggles", 2);

		toggleStates.resize(9, false);
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

		ImGuiEx::OnWindowHoverHightlight();

		ImVec2 buttonSize = ImVec2(40, 35);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImGuiEx::tightButtonPadding);

		//Projec Tab
		{
			if (ImGuiEx::ImageButton("##NewButton", ResourceCache::GetIcon(IconType::New)->getTextureHandle().m_ID,
				buttonSize, 7.0, ImDrawFlags_RoundCornersLeft))
			{
				Scene* scene = editor->GetScene();
				scene->Save(scene->getName() + ".yaml");
				editor->CreateScene();
			}

			ImGui::SameLine();
			if (ImGuiEx::ImageButton("##OpenButton", ResourceCache::GetIcon(IconType::Open)->getTextureHandle().m_ID,
				buttonSize,7.0))
			{


			}

			ImGui::SameLine();
			if (ImGuiEx::ImageButton("##SaveButton", ResourceCache::GetIcon(IconType::Save)->getTextureHandle().m_ID,
				buttonSize,7.0, ImDrawFlags_RoundCornersRight))
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
			auto entitiesIcon = ResourceCache::GetIcon(IconType::Entities)->getTextureHandle().m_ID;
			auto resourcesIcon = ResourceCache::GetIcon(IconType::Resources)->getTextureHandle().m_ID;

			sceneToggleGroup->OnRender(0, "##EntityViewerToggle", &editor->GetSceneHierarchyActive(), entitiesIcon, buttonSize, 7.0f, ImDrawFlags_RoundCornersLeft);
			ImGui::SameLine();
			sceneToggleGroup->OnRender(0, "##ResourceViewerToggle", &editor->GetResourceViewerActive(), resourcesIcon, buttonSize, 7.0f, ImDrawFlags_RoundCornersRight);
		}

		ImGuiEx::VSeparator();

		ImGuiEx::ImageButton("##CaptureButton", ResourceCache::GetIcon(IconType::Capture)->getTextureHandle().m_ID,
			buttonSize, 7.0, ImDrawFlags_RoundCornersAll);

		ImGuiEx::VSeparator();

		//Modes
		{
			renderToggleState->OnRender("##RenderStateToggle", buttonSize);

			ImGui::SameLine();
			if (ImGuiEx::ImageButton("##RenderOptionsButton", ResourceCache::GetIcon(IconType::RenderOption)->getTextureHandle().m_ID,
				buttonSize, 7.0, ImDrawFlags_RoundCornersAll))
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

	bool ControlBar::Toggle(int index)
	{
		if (currentToggle >= 0)
		{
			toggleStates[currentToggle] = false;
		}

		toggleStates[index] = toggleStates[index] ? false : true;
		currentToggle = toggleStates[index] ? -1 : index;


		return toggleStates[index];
	}
}
