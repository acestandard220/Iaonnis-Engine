#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"
#include "Panels.h"
#include "../Scene/Entity.h"
#include "GeneralWindow.h"

namespace Iaonnis
{
	class Inspector : public EditorPanel
	{
	public:
		Inspector(Editor* editor);




		virtual void OnUpdate(float dt)override;
	private:
		enum class InspectionType
		{
			Entity,Material
		};

		void InspectEntity(Entity* entity);
		//Might Not be needed still considering
		void InspectSubEntity(Entity* entity, int index);


		//Material Inspection
		void InspectMaterial(Entity& entity, UUID materialID, int index);
		bool SubMeshMaterialTree(Entity& entt, int subMeshIndex);
		bool InspectTextureMap(std::shared_ptr<Material> material, TextureMapType type);

		void MaterialSelectionContext(Entity& entt, int index);
	};
}