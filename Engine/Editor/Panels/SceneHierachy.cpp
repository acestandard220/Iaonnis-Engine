#include "SceneHierachy.h"
#include "../Editor.h"

namespace Iaonnis
{

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
		SCOPE_TIMER(__FUNCTION__);

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
			auto& entts = editor->GetScene()->GetEntities();
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
					{
						
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

		}
		ImGui::End();
	}

	void SceneHierachy::OnPopUpContext()
	{
		if (ImGui::BeginPopupContextWindow("##RightClickContext"))
		{
			if (ImGui::BeginMenu("Add Mesh"))
			{
				if (ImGui::MenuItem("Custom Mesh"))
				{
					//Job
					std::string meshPath = FileDialog::OpenFileDialog();
					if (!meshPath.empty())
					{
						std::shared_ptr<Mesh> newResource = editor->GetScene()->getCache()->load<Mesh>(meshPath);
						if (!newResource)
						{
							IAONNIS_LOG_ERROR("Failed to custom mesh.");
						}
						else {
							editor->GetScene()->addMesh(newResource->GetID());
						}
					}
				}
				if (ImGui::MenuItem("Empty Entity"))
				{
					auto& newEntt = editor->GetScene()->CreateEntity("Entity");
					editor->Deselect();
				}
				if (ImGui::MenuItem("Cube"))
				{
					auto& newEntt = editor->GetScene()->AddCube("Cube");
					editor->Deselect();
					editor->SelectEntt(&newEntt);
					editor->SetSelectionType(SelectionType::Entity);
				}
				if (ImGui::MenuItem("Plane"))
				{
					auto& newEntt = editor->GetScene()->AddPlane("Plane");
					editor->Deselect();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Add Light"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					auto& newEntt = editor->GetScene()->addDirectionalLight();
					editor->Deselect();
				}
				if (ImGui::MenuItem("Point Light"))
				{
					auto& newEntt = editor->GetScene()->AddPointLight();
					editor->Deselect();
				}
				if (ImGui::MenuItem("Spot Light"))
				{
					auto& newEntt = editor->GetScene()->addSpotLight();
					editor->Deselect();
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

	void SceneHierachy::OnItemPopUpContext()
	{
		if (ImGui::BeginPopupContextItem("##SceneHierarchyItemPopUp"))
		{
			if (ImGui::MenuItem("Remove"))
			{
				Entity* entity = editor->getSelectedEntity();
				editor->GetScene()->RemoveEntity(*entity);

				editor->Deselect();
			}

			if (ImGui::MenuItem("Hide"))
			{

			}

			ImGui::EndPopup();
		}
	}
}
