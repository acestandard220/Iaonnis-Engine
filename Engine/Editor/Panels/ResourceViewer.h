#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"
#include "Panels.h"
#include "../Scene/Entity.h"
#include "GeneralWindow.h"

namespace Iaonnis
{
	class ResourceViewer : public EditorPanel
	{

		enum LayoutType
		{
			Grid,List
		};

		struct DetailsCache
		{
			std::string name;
			std::string type;
			std::string source;
			std::string diskSize;
			std::string uuid;
			std::string refCount;

			//Specifics
			std::string geomtry;

			std::string dimension;
			std::string channelData;

			UUID diffuse;
			UUID normal;
			UUID ao;
			UUID roughness;
			UUID metallic;

		}detailsCache;

		struct DetailAreaValues
		{
			ImVec2 contentRegionAvail;
			ImVec2 tableRatio = ImVec2(0.3, 0.7);

			float previewImageSizeFactor = 0.4f;
			float labelFieldWidth;
			float inputFieldWidth;

		}detailArea;

	public:
		ResourceViewer(Editor* editor);
		virtual ~ResourceViewer() = default;

		virtual void OnUpdate(float dt)override;

	private:
		void RenderTop(ImVec2 subPanelRatio);

		void DisplayAsGrid();
		void DisplayAsList();

		void DisplayDetails();
		void DisplaySpecificDetails();

		void GetResources();
		void CacheDetails();

		bool ApplyDateFilter(Resource* res, uint64_t dateRef);
		bool ApplyStateFilter(Resource* resource);
		bool ApplySizeFilter(Resource* resource, uint64_t sizeRef);

		void MakeResourceSelection(std::vector<std::shared_ptr<Resource>>& resources);

	private:
		std::unique_ptr<ImGuiEx::ToggleState2> viewLayoutToggle;

		std::vector<std::shared_ptr<Resource>> resources;

		ResourceType displayType = ResourceType::Unknown;
		LayoutType currentLayout = Grid;
		Resource* currentResource = nullptr;
		ResourceType currentResourceType;

		bool syncResources = true;

		bool editActive = true;

		//Filters
		int dateFilter = 0;
		int sizeFilter = 0;
		int stateFilter = 0;
	};

}
