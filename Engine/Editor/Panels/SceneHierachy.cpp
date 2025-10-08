#include "SceneHierachy.h"
#include "../Editor.h"

namespace Iaonnis
{
	//int selectedIndex = -1;

	SceneHierachy::SceneHierachy(Editor* editor)
		:EditorPanel(editor)
	{
		name = "Hierarchy##" + std::to_string(_id++);
		active = true;
	}

	SceneHierachy::~SceneHierachy()
	{

	}

	void SceneHierachy::OnUpdate(float dt)
	{
		if(ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_HorizontalScrollbar))
		{
			auto plusIcon = ResourceCache::GetIcon(IconType::Plus);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
			if (ImGuiEx::ImageButton("ImageButton", plusIcon->getTextureHandle().m_ID, ImVec2(25, 25), ImDrawFlags_RoundCornersAll))
			{
				ImGui::OpenPopup("##RightClickContext");
				OnPopUpContext();
			}
			ImGui::PopStyleVar();
			ImGui::Separator();

			int index = 0;
			auto& entts = editor->getScene()->GetEntities();
			for (auto& entt : entts)
			{
				int flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

				if (editor->GetSelectionIndex() != -1)
				{
					if (index == editor->GetSelectionIndex())
						flag |= ImGuiTreeNodeFlags_Selected;
				}

				auto tag = entt.GetTag();
				if (ImGui::TreeNodeEx((tag + "##Parent" + UUIDFactory::uuidToString(entt.GetUUID())).c_str(),flag))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						editor->SetSelectionIndex(index);
						editor->SelectEntt(&entt);
						editor->SetSelectionType(SelectionType::Entity);
					}


					index++;
					if (entt.HasComponent<MeshFilterComponent>())
					{
						auto meshFilter = entt.GetComponent<MeshFilterComponent>();
						for (auto& name : meshFilter.names)
						{
							int childFlag = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
							if (editor->GetSelectionIndex() != -1)
							{
								if (index == editor->GetSelectionIndex())
									childFlag |= ImGuiTreeNodeFlags_Selected;
							}

							if (ImGui::TreeNodeEx((name + " ##Child" + UUIDFactory::uuidToString(entt.GetUUID())).c_str(),childFlag))
							{

								if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
								{
									editor->SetSelectionIndex(index);
									editor->SelectEntt(nullptr);
									editor->SetSelectionType(SelectionType::SubEntity);
								}
								
								ImGui::TreePop();
							}
							index++;
						}
					}
					ImGui::TreePop();
				}
			}

			OnPopUpContext();
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered())
			{
				editor->SetSelectionIndex(-1);
				editor->SetSelectionType(SelectionType::None);
				editor->SelectEntt(nullptr);
				editor->SetSelectionType(SelectionType::None);
			}

			if(ImGui::IsMouseClicked(1))
			{
				//ImGui::OpenPopup("##RightClickContext");
			}
		}
		ImGui::End();
	}

	void SceneHierachy::OnPopUpContext()
	{
		if (ImGui::BeginPopupContextWindow("##RightClickContext"))
		{
			if (ImGui::BeginMenu("Add Mesh"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					auto& newEntt = editor->getScene()->addCube("Cube");
					editor->DeselectAll();
				}
				if (ImGui::MenuItem("Plane"))
				{
					auto& newEntt = editor->getScene()->addPlane("Plane");
					editor->DeselectAll();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Add Light"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					auto& newEntt = editor->getScene()->addDirectionalLight();
					editor->DeselectAll();
				}
				if (ImGui::MenuItem("Point Light"))
				{
					auto& newEntt = editor->getScene()->addPointLight();
					editor->DeselectAll();
				}
				if (ImGui::MenuItem("Spot Light"))
				{
					auto& newEntt = editor->getScene()->addSpotLight();
					editor->DeselectAll();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Add Camera"))
			{
				if (ImGui::MenuItem("Perpspective"))
				{
					
				}
				if (ImGui::MenuItem("Orthographic"))
				{

				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}
}
