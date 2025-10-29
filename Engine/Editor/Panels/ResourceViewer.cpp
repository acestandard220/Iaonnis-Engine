#include "ResourceViewer.h"
#include "../Editor.h"

namespace Iaonnis
{
    int tempInt = 0;
    bool tempBool = 0;

	ResourceViewer::ResourceViewer(Editor* editor)
		:EditorPanel(editor)
	{
		active = false;
		name = "ResourceViewer";

        auto gridLayoutIcon = ResourceCache::GetIcon(IconType::GridLayout)->getTextureHandle().m_ID;
        auto listLayoutIcon = ResourceCache::GetIcon(IconType::ListLayout)->getTextureHandle().m_ID;

        viewLayoutToggle = std::make_unique<ImGuiEx::ToggleState2>(gridLayoutIcon, listLayoutIcon, ImDrawFlags_RoundCornersLeft, ImDrawFlags_RoundCornersRight);
   	}

	void ResourceViewer::OnUpdate(float dt)
    {
        if (!active)
            return;

        ImGui::SetNextWindowSizeConstraints(ImVec2(1000, 750), ImVec2(1400, 1400));
        ImGui::Begin("MyWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking);

        ImVec2 subPanelRatio = ImVec2(0.2, 0.8);

        //Cache pointers to the resource type selected.
        if (syncResources)
        {
            GetResources();
            syncResources = false;
        }

        RenderTop(subPanelRatio);
               
        ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

        float sidePanelWidth = contentRegionAvail.x * subPanelRatio.x;
        float miniInfoHeight = contentRegionAvail.y * 0.05f;
        ImVec2 workAreaRect = ImVec2(contentRegionAvail.x - sidePanelWidth - (editActive * sidePanelWidth), 
            contentRegionAvail.y - miniInfoHeight);

        ImVec2 sidePanelButtonSize = ImVec2(sidePanelWidth * 0.8, 30.0f);
        float sidePanelBlankWidth = sidePanelWidth - sidePanelButtonSize.x;

        {
            ImGui::BeginGroup();

            static const char ll[] = { "All Resources\0Image Textures\0Materials\0Meshes\0" };
            static ImDrawFlags drawFlags[] = { ImDrawFlags_RoundCornersTop, 256, 256, ImDrawFlags_RoundCornersBottom };

            ResourceType oldDisplayType = displayType;
            ImGuiEx::ToggleTextButtonGroup(ll, (int*)&displayType, 4, sidePanelButtonSize, 5.0f, drawFlags);
            if (oldDisplayType != displayType)
            {
                syncResources = true;
                //sIndex = 0;
                currentResource = nullptr;
            }

            ImGui::Dummy(ImVec2(0.0, 10.0f));

            ImGui::Text("Filters");

            float comboWidth = sidePanelButtonSize.x;
            //Set Item Width
            ImGui::Text("Date Modified");
            if (ImGuiEx::Combo("##DateModified", &dateFilter, "Any Time\0Today\0Last Week\0Last Month\0Beyond Three Months\0", comboWidth, true))
                syncResources = true;

            ImGui::Text("File Size");
            if (ImGuiEx::Combo("##FileSize", &sizeFilter, "Any Size\0Bytes(B)\0Kilobytes(KB)\0Megabytes(MB)\0Gigabytes(GB)\0", comboWidth))
                syncResources = true;

            ImGui::Text("Resource State");
            if (ImGuiEx::Combo("##ResourceState", &stateFilter, "Any Sate\0No Disk Write\0On Disk\0", comboWidth, true))
                syncResources = true;

            ImGui::EndGroup();
        }

        ImGui::SameLine(sidePanelButtonSize.x, sidePanelBlankWidth);

        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg]);

            ImGui::BeginChild("##ResourceLister", workAreaRect, ImGuiChildFlags_FrameStyle);
            
            if (currentLayout == Grid)
                DisplayAsGrid();
            else if (currentLayout == List)
                DisplayAsList();

            MakeResourceSelection(resources);

            ImGui::EndChild();

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        ImVec2 editRect = ImVec2(editActive * sidePanelWidth + 0.1, contentRegionAvail.y - miniInfoHeight);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);

            ImGui::BeginChild("##EditArea", editRect);

            DisplayDetails();

            ImGui::EndChild();

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }

        {


        }

        ImGui::End();
    }

    void ResourceViewer::RenderTop(ImVec2 subPanelRatio)
    {
        ImGui::BeginGroup();

        ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
        ImVec2 buttonSize = ImVec2(25, 25);

        float sidePanelWidth = contentRegionAvail.x * subPanelRatio.x;
        float resourceListerWidth = contentRegionAvail.x * subPanelRatio.y;

        float miniButtonsStart  = (sidePanelWidth) - (4.0f + (2.0f + buttonSize.x) * 4.0);
        float largeButtonsWidth = (miniButtonsStart - 4.0f) / 2.2;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + sidePanelWidth - ImGui::GetStyle().WindowPadding.x);

        static char buffer[64];
        ImGui::SetNextItemWidth(resourceListerWidth - sidePanelWidth);
        ImGui::InputTextWithHint("##Search Resource", "Search Resource", buffer, 64, 
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll);
        ImGui::GetFrameHeight();

        ImGui::SameLine();
        ImGuiEx::Button("Import", ImVec2(largeButtonsWidth, 25));
        ImGui::SameLine();
        if (ImGuiEx::Button("Export", ImVec2(largeButtonsWidth, 25), 5.0f, 256, resources.size() || !currentResource))
        {
            std::thread([&]() {
                std::string savePath = FileDialog::SaveFileDialog();
                currentResource->save(savePath);
            }).detach();
        }

        ImGui::SameLine(0.0f, miniButtonsStart - 4.0f - (largeButtonsWidth * 2.0f));      

        auto syncIcon = ResourceCache::GetIcon(IconType::Sync)->getTextureHandle().m_ID;
        if (ImGuiEx::ImageButton("##SyncResourceViewer", syncIcon, buttonSize, 5.0f, ImDrawFlags_RoundCornersAll, ImVec2(1.5f, 1.5f)))
            syncResources = true;

        ImGui::SameLine();

        viewLayoutToggle->OnRender("##LayoutToggle", buttonSize, 5.0f, false, ImVec2(1.5f, 1.5f));
        LayoutType newLayout = (LayoutType)viewLayoutToggle->GetToggleState();
        if((currentLayout != newLayout))
            currentLayout = newLayout;

        auto editIcon = ResourceCache::GetIcon(IconType::Edit)->getTextureHandle().m_ID;
        ImGuiEx::ToggleImageButton("##DetailsEditToggle", &editActive, editIcon, buttonSize, 5.0f, ImDrawFlags_RoundCornersAll,
            ImVec2(1.5f, 1.5f));

        ImGui::EndGroup();
    }

    void ResourceViewer::DisplayAsGrid()
    {
        static float padding = 4.0f;
        static float thumbnailSize = 128;

        float cellingSize = padding + thumbnailSize;

        float panelWidth = ImGui::GetContentRegionAvail().x + 1.0;
        int cellCount = int(panelWidth / cellingSize);

        if (cellCount)
        {
            ImGui::Columns(cellCount, 0, false);
        }

        for (int i = 0; i < resources.size(); i++)
        {
            std::string label = "##Icon" + std::to_string(i);
            auto icon = ResourceCache::GetIcon((IconType::Unknown))->getTextureHandle().m_ID;

            bool selected = (resources[i].get() == currentResource);

            if (ImGuiEx::ImageSelectable(label.c_str(), icon, ImVec2(thumbnailSize, thumbnailSize), selected))
            {
                //sIndex = i;
                currentResource = resources[i].get();
            }

            const char* _text = resources[i]->getName().c_str();
            float textWidth = ImGui::CalcTextSize(_text).x;
            float textStart = (thumbnailSize - textWidth) * 0.5;

            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + textStart, ImGui::GetCursorPos().y));
            ImGui::Text(_text);
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }

    void ResourceViewer::DisplayAsList()
    {
        static float padding = 4.0f;
        static float thumbnailSize = 128;

        const ImGuiTableFlags tableFlags = ImGuiTableFlags_Sortable | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;
        if (ImGui::BeginTable("##ResourceListLayoutTable", 4, tableFlags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Date",  ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type",  ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Size",  ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();

            for (int i = 0; i < resources.size(); i++)
            {
                ImGui::TableNextRow();

                std::string label = "##Icon" + std::to_string(i);
                auto icon = ResourceCache::GetIcon((IconType::Unknown))->getTextureHandle().m_ID;

                bool selected = (resources[i].get() == currentResource);

                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable(resources[i]->getName().c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
                    currentResource = resources[i].get();


                ImGui::TableSetColumnIndex(1);
                ImGui::Text(DecodeFileTime(resources[i]->GetLastWrite()).c_str());

                ImGui::TableSetColumnIndex(2);
                std::string typeLabel = Resource::getTypeString(resources[i]->getType()) + " (" + resources[i]->GetExtension() + ")";
                ImGui::Text(typeLabel.c_str());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text(FormatBytes(resources[i]->GetByteSize()).c_str());
            }

            ImGui::TableNextRow();

            ImGui::EndTable();
        }
    }

    void ResourceViewer::DisplayDetails()
    {
        if (!currentResource)
            return ImGui::TextWrapped("No Resource Has Been Selected!");

        ImGui::BeginGroup();

        detailArea.contentRegionAvail = ImGui::GetContentRegionAvail();

        ImVec2 previewImageSize = ImVec2(detailArea.contentRegionAvail.x, detailArea.contentRegionAvail.y * detailArea.previewImageSizeFactor);
        detailArea.labelFieldWidth = detailArea.contentRegionAvail.x * detailArea.tableRatio.x;
        detailArea.inputFieldWidth = detailArea.contentRegionAvail.x - detailArea.labelFieldWidth - ImGui::GetStyle().WindowPadding.x;

        auto icon = ResourceCache::GetIcon((IconType::Unknown))->getTextureHandle().m_ID;

        if (ImGuiEx::ImageButton("##ResourcePreview", icon, previewImageSize, 5.0f))
        {

        }

        ImGuiEx::OnItemHovered([&]() {
            ImGui::Text("Resource Preview");
            ImGui::Separator();
            ImGui::Text(detailsCache.name.c_str());
        });

        ImGui::BeginTable("DetailsTable", 2, ImGuiTableFlags_SizingStretchSame);
        
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, detailArea.tableRatio.x); // 30%
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, detailArea.tableRatio.y); // 70%
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0); ImGui::Text("Name");
        ImGui::TableSetColumnIndex(1);
        ImGuiEx::InputText("##Name", (char*)detailsCache.name.c_str(), detailsCache.name.size() + 1, detailArea.inputFieldWidth, false);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Type");
        ImGui::TableSetColumnIndex(1);
        ImGuiEx::InputText("##Type4040", (char*)detailsCache.type.c_str(), detailsCache.type.size() + 1, detailArea.inputFieldWidth, false);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Source");
        ImGui::TableSetColumnIndex(1);
        ImGuiEx::InputText("##Source", (char*)detailsCache.source.c_str(), detailsCache.source.size() + 1, detailArea.inputFieldWidth, false);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("Disk Size");
        ImGui::TableSetColumnIndex(1);
        ImGuiEx::InputText("##DiskSize4040", detailsCache.diskSize.data(), detailsCache.diskSize.size() + 1, detailArea.inputFieldWidth, false);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("UUID");
        ImGui::TableSetColumnIndex(1);
        ImGuiEx::InputText("##UUID4040", (char*)detailsCache.uuid.c_str(), detailsCache.uuid.size() + 1, detailArea.inputFieldWidth, false);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("References");
        ImGui::TableSetColumnIndex(1);
        ImGuiEx::InputText("##RefCount4040", (char*)detailsCache.refCount.c_str(), detailsCache.refCount.size() + 1, detailArea.inputFieldWidth, false);

        ImGui::EndTable();

        DisplaySpecificDetails();
 
        ImGui::EndGroup();
    }

    void ResourceViewer::DisplaySpecificDetails()
    {
        

        switch (currentResource->getType())
        {
            case ResourceType::ImageTexture:
            {              
                ImGui::BeginTable("DetailsTable", 2, ImGuiTableFlags_SizingStretchSame);

                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, detailArea.tableRatio.x); // 30%
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, detailArea.tableRatio.y); // 70%
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::Text("Dimension");
                ImGui::TableSetColumnIndex(1);
                ImGuiEx::InputText("##FileSizeAndDimensions", detailsCache.dimension.data(),
                    detailsCache.dimension.size() + 1, detailArea.inputFieldWidth, false);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::Text("Channel");
                ImGui::TableSetColumnIndex(1);
                
                ImGuiEx::InputText("##Channel Data", detailsCache.channelData.data(),
                    detailsCache.channelData.size() + 1, detailArea.inputFieldWidth, false);
                ImGui::TableNextRow();

                ImGui::EndTable();

                break;
            }

            case ResourceType::Material:
            {
                //Dangerous don't try this at home...
                UUID* start = &detailsCache.diffuse;
                static int selectedMap;
                for (int i = 0; i < 5; i++)
                {
                    bool selected = (i == selectedMap);
                    auto res = editor->GetScene()->getCache()->GetByUUID<ImageTexture>(start[i]);
                    auto textureID = res->getTextureHandle().m_ID;
                    std::string _label = "##ImageTextureMap" + std::to_string(i);
                    if (ImGuiEx::ImageSelectable(_label.c_str(), textureID, ImVec2(30, 30), selected, 5.0f))
                    {
                        selectedMap = i;
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        displayType = ResourceType::ImageTexture;
                        syncResources = true;
                        currentResource = res.get();
                    }
                }

                break;
            }

            case ResourceType::Mesh:
            {
                ImGui::BeginTable("DetailsTable", 2, ImGuiTableFlags_SizingStretchSame);

                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.3f); // 30%
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.7f); // 70%
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::Text("Geometry");
                ImGui::TableSetColumnIndex(1);
                
                ImGuiEx::InputText("##GeometryData4042", detailsCache.geomtry.data(),
                    detailsCache.geomtry.size() + 1, detailArea.inputFieldWidth, false);

                ImGui::EndTable();

                break;
            }
        }
    }

    int GetNumberOfDays(int index)
    {
        switch (index)
        {
            case 1:return 1;
            case 2:return 7;
            case 3:return 31;
            case 4:return 93;
        }
    }

    bool Iaonnis::ResourceViewer::ApplyDateFilter(Resource* res, uint64_t dateRef)
    {
        if (!dateRef)
            return false;
        if (!dateFilter)
            return true;

        auto fileLastWrite = res->GetLastWrite();

        return fileLastWrite >= dateRef;
    }

    bool ResourceViewer::ApplyStateFilter(Resource* resource)
    {
        if (!resource)
            return false;
        if (!stateFilter)
            return true;

        int refValue = stateFilter - 1;

        return  (refValue && resource->GetByteSize()) || (!refValue && !resource->GetByteSize());
    }

    uint64_t GetRefSize(int index)
    {
        switch (index)
        {
            case 1:return 1023;
            case 2:return (1024 * 1024)-1;
            case 3:return (1024 * 1024 * 1024) - 1;
            case 4:return (1024 * 1024 * 1024 * 1024) - 1;
        }
    }

    bool Iaonnis::ResourceViewer::ApplySizeFilter(Resource* resource, uint64_t sizeRef)
    {
        if (!resource)
            return false;
        if (!sizeFilter)
            return true;
        

        return resource->GetByteSize() <= sizeRef;
    }

    void ResourceViewer::GetResources()
    {
        resources.clear();
        auto tempResources = editor->GetScene()->getCache()->getByType<Resource>((ResourceType)displayType);

        if (!dateFilter && !stateFilter && !sizeFilter)
        {
            resources = tempResources;
            return; 
        }

        uint64_t sizeRefValue = GetRefSize(sizeFilter);
        uint64_t dateRefValue = GetEpoch(GetNumberOfDays(dateFilter));

        for (auto& res : tempResources)
        {
            if (ApplyDateFilter(res.get(), dateRefValue) && ApplyStateFilter(res.get()) && ApplySizeFilter(res.get(), sizeRefValue))
                resources.push_back(res);
        }
    }

    void ResourceViewer::CacheDetails()
    {
        if (!currentResource)
            return;

        detailsCache.name = currentResource->GetName();
        detailsCache.source = currentResource->getPath().string();
        detailsCache.type = Resource::getTypeString(currentResource->getType()) + " (" + currentResource->GetExtension() + ")";
        detailsCache.uuid = UUIDFactory::uuidToString(currentResource->GetID());
        detailsCache.diskSize = FormatBytes(currentResource->GetByteSize());
        detailsCache.refCount = std::to_string(currentResource->GetRefCount()) + " objects";

        switch(currentResourceType)
        {
            case ResourceType::ImageTexture:
            {
                ImageTexture* resource = (ImageTexture*)currentResource;
                int width = resource->getWidth();
                int height = resource->getHeight();
                int bitPerChannel = resource->getBitPerChannel();
                int nChannel = resource->getChannelCount();

                detailsCache.dimension = std::to_string(width) + "x" + std::to_string(height) + " px";
                detailsCache.channelData = std::string("RGBA").substr(0, nChannel) + " (" +
                    std::to_string(bitPerChannel) + "-bit)";
                break;
            }
            case ResourceType::Mesh:
            {
                Mesh* resource = (Mesh*)currentResource;
                int nVertices = resource->getVertices().size();
                int nIndice = resource->getIndices().size();
                int nTriangles = nIndice / 3;
                int nSubMeshes = resource->getSubMeshCount();

                detailsCache.geomtry = std::string(128, ' ');
                int written = sprintf_s(detailsCache.geomtry.data(), 128, "v%d | i%d | t%d |s%d", 
                    nVertices, nIndice, nTriangles, nSubMeshes);
                if (written > 0)
                    detailsCache.geomtry.resize(written);
                break;
            }
            case ResourceType::Material:
            {
                Material* material = (Material*)currentResource;

                detailsCache.diffuse = material->GetDiffuseID();
                detailsCache.ao = material->GetAoID();
                detailsCache.normal = material->GetNormalID();
                detailsCache.roughness = material->GetRoughnessID();
                detailsCache.metallic = material->getMetallicID();
            }
        }

    }
      

    void ResourceViewer::MakeResourceSelection(std::vector<std::shared_ptr<Resource>>& resources)
    {
        if (currentResource)
        {
            currentResourceType = currentResource->getType();
        }

        CacheDetails();        
    }

}