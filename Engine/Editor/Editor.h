#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Scene/Components.h"

#include "ImGuiEx/ImGui_Extensions.h"
#include "Panels/FileDialog.h"
#include "MenuBar.h"

#include "Panels/SceneHierachy.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ViewPort.h"

#include "../Renderer/Renderer.h"

namespace Iaonnis
{
	enum class SelectionType
	{
		Entity, //Meshes, Lights & Cameras
		SubEntity, //SubMeshes
		None
	};

	enum class ViewPortAction
	{
		Orbit,
		Pan,
		Idle,
		Inactive
	};

	class Editor
	{
	public:
		Editor(GLFWwindow* window, std::shared_ptr<Scene> activeScene);

		void OnUpdate(Renderer3D::RendererStatistics stats, uint32_t r);
		void ShutDown();

		Entity* getSelectedEntity() { return SelectionData.selectedEntt; }
		void SelectEntt(Entity* entt);
		bool isEnttSelectionMade();
		SelectionType GetSelectionType() { return SelectionData.selectionType; }

		void SetSelectionType(SelectionType type) { SelectionData.selectionType = type; }
		void SetSelectionIndex(int index) { SelectionData.selectionIndex = index; }
		int  GetSelectionIndex() { return SelectionData.selectionIndex; }
		void DeselectAll();

		void SetViewPortAction(ViewPortAction action) { viewPortAction = action;}
		ViewPortAction GetViewPortAction() { return viewPortAction; }

		void CreatePopUp(Event& event);
		Scene* getScene() { return scene.get(); }
		void CreateScene();


		uint32_t renderOut; //temp
	private:
		void InitializeDefaultPanels();

	private:
		std::unique_ptr<MenuBar> menubar;
		std::vector<std::unique_ptr<EditorPanel>> panels;

		struct
		{
			int selectionIndex = -1;
			SelectionType selectionType = SelectionType::None;
			Entity* selectedEntt = nullptr;
		}SelectionData;

		ViewPortAction viewPortAction = ViewPortAction::Inactive;

		

	private:
		std::shared_ptr<Scene> scene;
		GLFWwindow* window;
	};
}

