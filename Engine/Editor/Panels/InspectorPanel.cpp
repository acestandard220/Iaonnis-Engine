#include "InspectorPanel.h"
#include "../Editor.h"

namespace Iaonnis
{

	Inspector::Inspector(Editor* editor)
		: EditorPanel(editor)
	{
		name = "Inspector";
		active = true;
	}

	void InspectSubEntity()
	{
		if(ImGui::TreeNodeEx("Sub Entity"))
		{
			ImGui::Text("Name");
			//ImGuiEx::ImageButton("Diffuse Texture", 0,ImVec2(50, 50), ImDrawFlags_RoundCornersAll);
			ImGui::TreePop();
		}

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

	const char* TypeStrings[] = {
		"Directional","Point","Spot"
	};

	void Inspector::InspectEntity(Entity* entity)
	{
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



		//MeshFilterComponent& meshFilter = entity->G
	}

	void Inspector::OnUpdate(float dt)
	{
		auto selectionType = editor->GetSelectionType();
		if (selectionType == SelectionType::None)
			return;

		ImGui::Begin(name.c_str(), &active);

		if (selectionType == SelectionType::Entity)
			InspectEntity(editor->getSelectedEntity());
		else
			InspectSubEntity();

		ImGui::End();
	}

	
}
