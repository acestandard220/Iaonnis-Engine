#include "GeneralWindow.h"
#include "../Editor.h"

namespace Iaonnis
{
	namespace GeneralWindow
	{
		Editor* editor;

		void Initialize(Editor* editor)
		{
			GeneralWindow::editor = editor;
		}

		namespace CacheViewer
		{
			bool active = false;
			std::string name = "Cache Viewer";
			std::function<void(UUID id)> callback = nullptr;

			template<>
			void OnRender<ImageTexture>()
			{
				if (!active)
					return;

				ImGui::Begin(name.c_str(), &active);
				auto cache = editor->getScene()->getCache();
				
				std::vector<std::shared_ptr<ImageTexture>> imageTextureCache = cache->getByType<ImageTexture>(ResourceType::ImageTexture);
				
				static float thumbnailSize = 64;
				static float padding = 6.0f;
				
				float availableWidth = ImGui::GetContentRegionAvail().x;
				float cellSize = thumbnailSize + padding;

				int columnCount = int(availableWidth / cellSize);

				if (columnCount)
				{
					ImGui::Columns(columnCount, "##CacheViewerColumns", false);
				}

				ImGuiContext* con = ImGui::GetCurrentContext();
				ImGuiWindow* currentWindow = con->CurrentWindow;

				static UUID selectedItem = UUIDFactory::getInvalidUUID();
				for (auto& imageTexture : imageTextureCache)
				{
					std::string label = imageTexture->getName() + "##" + UUIDFactory::uuidToString(imageTexture->GetID());

					//int flag = /*ImGuiButtonFlags_*/
					bool clicked = false;

					if (imageTexture->GetID() == selectedItem)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
						
						ImGui::ImageButtonEx(currentWindow->GetID(label.c_str()), imageTexture->getTextureHandle().m_ID,
							ImVec2(thumbnailSize, thumbnailSize), ImVec2(1, 0),
							ImVec2(0, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));

						ImGui::PopStyleColor();
					}
					else {
						ImGui::ImageButtonEx(currentWindow->GetID(label.c_str()), imageTexture->getTextureHandle().m_ID,
							ImVec2(thumbnailSize, thumbnailSize), ImVec2(1, 0),
							ImVec2(0, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
					}
					

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
						selectedItem = imageTexture->GetID();

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						
						callback(imageTexture->GetID());
						SetInactive();
						break;
					}
					ImGui::NextColumn();
				}

				ImGui::Columns(1);
				ImGui::End();
			}

			void SetActive(std::function<void(UUID id)> callbak)
			{
				callback = callbak;
				active = true;
			}
			void SetInactive()
			{
				callback = nullptr;
				active = false;
			}
		}
	}
}


