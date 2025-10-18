#include "InspectorPanel.h"
#include "../Editor.h"
#include "../Style.h"

namespace Iaonnis
{
	ResourceCache* cache = nullptr;
	Scene* scene = nullptr;

	UUID materialToInspect = UUIDFactory::getInvalidUUID();
	bool materialInspector = false;

	Inspector::Inspector(Editor* editor)
		: EditorPanel(editor)
	{
		name = "Inspector";
		active = true;
	}

	void Inspector::InspectSubEntity(Entity* entity, int index)
	{

		/*if(ImGui::TreeNodeEx())
		{
			ImGui::Text("Name");
		
			ImGui::TreePop();
		}*/

	}

	bool Iaonnis::Inspector::SubMeshMaterialTree(Entity& entt, int subMeshIndex)
	{
		float contentRegionAvaiX = ImGui::GetContentRegionAvail().x;
		//contentRegionAvaiX = 0.0f;

		auto materialID = entt.GetSubMeshMaterial(subMeshIndex);

		std::shared_ptr<Material> material = cache->GetByUUID<Material>(materialID);
		const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.5, 2.5));
		bool value = ImGui::TreeNodeEx(material->getName().c_str(), flags);
		if(value)
		{

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			float padding = ImGuiEx::tightButtonPadding.x;

			ImGui::SameLine(contentRegionAvaiX - lineHeight * 0.5f);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if (ImGuiEx::ImageButton("##RemoveMaterial", ResourceCache::GetIcon(IconType::Remove)->getTextureHandle().m_ID, ImVec2(lineHeight, lineHeight)))
			{
				scene->ResetMaterial(entt.GetUUID(), subMeshIndex);
				editor->getScene()->OnEntityRegisteryModified();
				IAONNIS_LOG_ERROR("Material removed. Sub Mesh is using default material.");
			}

			ImGui::SameLine(contentRegionAvaiX - lineHeight - ImGuiEx::tightButtonPadding.x - lineHeight * 0.5f);
			if (ImGuiEx::ImageButton("##DuplicateMaterial", ResourceCache::GetIcon(IconType::Duplicate)->getTextureHandle().m_ID, ImVec2(lineHeight, lineHeight)))
			{
				auto newMtlResource = cache->duplicate<Material>(materialID);
				if (!newMtlResource)
				{
					IAONNIS_LOG_ERROR("Failed duplicate material.");
				}
				else {
					scene->AssignMaterial(entt.GetUUID(), newMtlResource->GetID(), subMeshIndex);
					editor->getScene()->OnEntityRegisteryModified();
					IAONNIS_LOG_INFO("Material Duplicated Successfully");
				}
			}

			ImGui::SameLine(contentRegionAvaiX - (ImGuiEx::tightButtonPadding.x + lineHeight) * 2.0f - lineHeight * 0.5f);
			if (ImGuiEx::ImageButton("##OpenMaterial", ResourceCache::GetIcon(IconType::Open)->getTextureHandle().m_ID, ImVec2(lineHeight, lineHeight)))
			{
				std::string mtlFilePath = FileDialog::OpenFileDialog();

			}
			ImGui::SameLine(contentRegionAvaiX - (ImGuiEx::tightButtonPadding.x + lineHeight) * 3.0f - lineHeight * 0.5f);

			if (ImGuiEx::ImageButton("##NewMaterial", ResourceCache::GetIcon(IconType::New)->getTextureHandle().m_ID, ImVec2(lineHeight, lineHeight)))
			{
				auto newMtlResource = cache->CreateNewMaterial();
				scene->AssignMaterial(entt.GetUUID(), newMtlResource->GetID(), subMeshIndex);
				editor->getScene()->OnEntityRegisteryModified();
				IAONNIS_LOG_INFO("New Material Created Successfully. (UUID = %s)", UUIDFactory::uuidToString(newMtlResource->GetID()).c_str());
			}

			ImGui::PopStyleColor();
		}
		ImGui::PopStyleVar();
		return value;
	}

	bool Inspector::InspectTextureMap(std::shared_ptr<Material> material, TextureMapType type)
	{
		auto imageResource = cache->GetByUUID<ImageTexture>(material->GetMap(type));
		ImVec2 buttonSize = ImVec2(75, 75);

		std::string label = Material::GetMapTypeString(type);
		ImGui::PushID(label.c_str());

		bool value = ImGuiEx::ImageButton(label.c_str(), imageResource->getTextureHandle().m_ID, buttonSize);
		if (value)
		{
			UUID materialID = material->GetID();
			GeneralWindow::CacheViewer::SetActive([this, materialID, type](UUID id) -> void
				{
					auto mat = cache->GetByUUID<Material>(materialID);
					if (mat) {
						mat->SetMap(type, id);
						editor->getScene()->OnMaterialModified();
					}
				});
		}

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGuiEx::Button("Edit", ImVec2(70, 20), ImDrawFlags_RoundCornersTop);
		if (ImGuiEx::Button("New", ImVec2(70, 20)))
		{
			auto duplicateResource = cache->duplicate<ImageTexture>(cache->GetDefaultByTextureType(type)->GetID());
			material->SetMap(type, duplicateResource->GetID());
			editor->getScene()->OnMaterialModified();//OnMaterialModified
		}
		if (ImGuiEx::Button("Open", ImVec2(70, 20)))
		{
			filespace::filepath filePath = FileDialog::OpenFileDialog();
			if (!filePath.empty())
			{
				std::shared_ptr<ImageTexture> loadedTexture = cache->load<ImageTexture>(filePath);
				if (loadedTexture == nullptr)
				{
					IAONNIS_LOG_ERROR("No texture has been loaded");
				}
				else {
					material->SetMap(type, loadedTexture->GetID());
					editor->getScene()->OnMaterialModified();//OnMaterialModified
				}
			}
		}
		if (ImGuiEx::Button("Remove", ImVec2(70, 20), ImDrawFlags_RoundCornersBottom))
		{
			auto defaultTexture = cache->GetDefaultByTextureType(type);
			material->SetMap(type, defaultTexture->GetID());
			editor->getScene()->OnMaterialModified();//OnMaterialModified
		}
		ImGui::EndGroup();
		ImGui::Text(label.c_str());

		ImGui::PopID();
		return value;
	}

	void Iaonnis::Inspector::MaterialSelectionContext(Entity& entt, int index)
	{
		if(ImGui::BeginPopup("##MaterialSelection"))
		{
			for (auto& mtl : cache->getByType<Material>(ResourceType::Material))
			{
				if (ImGui::MenuItem(mtl->getName().c_str()))
				{
					scene->AssignMaterial(entt.GetUUID(), mtl->GetID(), index);
					editor->getScene()->OnEntityRegisteryModified();
				}
			}
			ImGui::EndPopup();
		}

		if(ImGui::BeginPopup("##MaterialSelectionGlobal"))
		{
			for (auto& mtl : cache->getByType<Material>(ResourceType::Material))
			{
				if (ImGui::MenuItem(mtl->getName().c_str()))
				{
					scene->AssigGlobalMaterial(entt.GetUUID(), mtl->GetID());
					editor->getScene()->OnEntityRegisteryModified();
				}
			}
			ImGui::EndPopup();
		}		
		return;
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity,UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;

		if (!entity.HasComponent<T>())
			return;
		
		auto& component = entity.GetComponent<T>();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
		if (ImGui::TreeNodeEx(name.c_str(),flags))
		{

			uiFunction(component);

			ImGui::TreePop();
		}
		ImGui::PopStyleVar(1);

		ImGui::Separator();
	}

	void Inspector::InspectEntity(Entity* entity)
	{
		auto cache = editor->getScene()->getCache();

		DrawComponent<TagComponent>("Entity", *entity, [&](auto& component)
		{
			if (ImGui::Checkbox(("##EntityActive" + UUIDFactory::uuidToString(entity->GetUUID())).c_str(), &entity->active))
			{
			}
			ImGui::SameLine();
			ImGui::Text(component.tag.c_str());
		});
		DrawComponent<TransformComponent>("Transform", *entity, [&](auto& component) 
		{
			if (ImGuiEx::InputFloat3("Position", &component.position[0], 0.0f) ||
				ImGuiEx::InputFloat3("Rotation", &component.rotation[0], 0.0f) ||
				ImGuiEx::InputFloat3("Scale", &component.scale[0], 1.0f))
			{
				editor->getScene()->OnEntityRegisteryModified();
			}
		});
		DrawComponent<MeshFilterComponent>("Mesh Filter", *entity, [&](auto& component)
		{
			MeshFilterComponent& meshFilter = component;

			ImGui::Text(entity->GetTag().c_str());

			const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;

			std::shared_ptr<Mesh> mesh = cache->GetByUUID<Mesh>(meshFilter.meshID);
			if (ImGui::TreeNodeEx("Materials", flags))
			{
				for (auto& [mtlID, mtlDependants] : meshFilter.materialIDMap)
				{
					std::shared_ptr<Material> material = cache->GetByUUID<Material>(mtlID);
					std::string label = material->getName() + "##02301";
					if (ImGui::TreeNodeEx(label.c_str(), flags))
					{
						static int mtlListCurrentItem = 0;
						if(ImGui::BeginListBox("##MtlDependantList",ImVec2(0,180)))
						{
							for (int i = 0; i < mtlDependants.size(); i++)
							{
								float lineHeight = GImGui->Font->FontSize;
								float padding = ImGuiEx::tightButtonPadding.x;

								SubMesh* submesh = mesh->getSubMesh(i);
								const bool isSelected = (mtlListCurrentItem == i);
								if (ImGui::Selectable(submesh->name.c_str(), isSelected))
								{
									mtlListCurrentItem = i;
								}

								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}

							ImGui::EndListBox();
						}


						ImGui::SameLine();

						float contentRegionAvail = ImGui::GetContentRegionAvail().x;


						ImGui::BeginGroup();
						if (ImGuiEx::Button("New", ImVec2(70, 25), ImDrawFlags_RoundCornersTop))
						{
							auto newMaterial = cache->duplicate<Material>(cache->GetDefaultMaterial()->GetID());
							scene->AssignMaterial(entity->GetUUID(), newMaterial->GetID(), mtlListCurrentItem);

							scene->OnEntityRegisteryModified();
							scene->OnMaterialModified();
						}
						if (ImGuiEx::Button("Open", ImVec2(70, 25)))
						{
							materialToInspect = mtlID;
							materialInspector = true;
						}

						if(materialInspector)
						{
							InspectMaterial(*entity, mtlID, mtlListCurrentItem);
						}

						if (ImGuiEx::Button("Remove", ImVec2(70, 25)))
						{
							scene->ResetMaterial(entity->GetUUID(), mtlListCurrentItem);
							editor->getScene()->OnEntityRegisteryModified(); //OnMaterialModified
						}

						if (ImGuiEx::Button("Reset All", ImVec2(70, 25)))
						{
							scene->ResetAllMaterial(entity->GetUUID());
							editor->getScene()->OnEntityRegisteryModified(); //OnMaterialModified
						}

						if (ImGuiEx::Button("Assign", ImVec2(70, 25)))
						{
							ImGui::OpenPopup("##MaterialSelection");
						}

						if (ImGuiEx::Button("Assign All", ImVec2(70, 25), ImDrawFlags_RoundCornersBottom))
						{
							ImGui::OpenPopup("##MaterialSelectionGlobal");
						}
						MaterialSelectionContext(*entity, mtlListCurrentItem);

						ImGui::EndGroup();
						ImGui::TreePop();
					}

					
				}
				entity->DettachUnusedMaterials();
				ImGui::TreePop();
			}

			//GeneralWindow::CacheViewer::OnRender<ImageTexture>();
		});

		DrawComponent<LightComponent>("Light", *entity, [&](auto& component) {
			static int selectedType = (int)component.type;

			ImGui::Text("Type"); ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

			if(ImGui::Combo("##Type", &selectedType, "Directional\0Point\0Spot", 3))
			{
				component.type = (LightType)((int)LightType::Directional + selectedType);
			}
			selectedType = (int)component.type; //Reset to the seleted comp. value

			ImGui::PopStyleVar();

			static ImVec4 color;
			ImGui::ColorEdit4("TexColor", (float*)&component.color);

			//Specific Light Params
			if (component.type == LightType::Spot)
			{
				static const float bias = 0.01f; 
				const ImGuiSliderFlags flags = ImGuiSliderFlags_ClampOnInput;
				ImGui::Text("Inner Radius"); ImGui::SameLine(); ImGui::SliderFloat("##SpotLightInnerRadius", &component.innerRadius, 0.001f, component.outerRadius - bias,"%.3f", flags);
				ImGui::Text("Outer Radius"); ImGui::SameLine(); ImGui::SliderFloat("##SpotLightOuterRadius", &component.outerRadius, 1.0f, 100.0f,"%.3f", flags);

				ImGui::Text("Spot Direction"); ImGui::SameLine(); ImGuiEx::InputFloat3("##SpotLightDirection", &component.spotDirection[0], 0.0f);
			}
		});

		DrawComponent<CameraComponent>("Camera", *entity, [&](auto& component) {
				std::shared_ptr<Camera> camera = component.camera;
				static int selectedType = (int)camera->getCameraType();

				if(ImGui::Combo("##CameraType", &selectedType, "Perspective\0Orthographic", 2))
				{
					camera->setType((CameraType)selectedType);
				}
				selectedType = (int)camera->getCameraType();

				
			});

	}

	void Inspector::InspectMaterial(Entity& entity, UUID materialID, int index)
	{
		if (ImGui::Begin("Inspect Material##InspectorChild",&materialInspector, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
		{
			auto material = cache->GetByUUID<Material>(materialID);
			if (SubMeshMaterialTree(entity, index))
			{
				InspectTextureMap(material, TextureMapType::Albedo);
				InspectTextureMap(material, TextureMapType::Normal);
				InspectTextureMap(material, TextureMapType::AO);
				InspectTextureMap(material, TextureMapType::Roughness);
				InspectTextureMap(material, TextureMapType::Metallic);

				ImGui::TreePop();
			}

			ImGui::End();
		}
	}

	void Inspector::OnUpdate(float dt)
	{
		SCOPE_TIMER(__FUNCTION__);
		scene = editor->getScene();
		cache = scene->getCache().get();


		auto selectionType = editor->GetSelectionType();
		if (selectionType == SelectionType::None)
			return;

		ImGui::Begin(name.c_str(), &active);

		if (selectionType == SelectionType::Entity)
			InspectEntity(editor->getSelectedEntity());
		else
			InspectSubEntity(editor->getSelectedEntity(),editor->GetSelectionIndex());

		ImGui::End();
	}

	
}
