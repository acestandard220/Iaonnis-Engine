#include "MenuBar.h"
#include "Editor.h"

namespace Iaonnis
{
	MenuBar::MenuBar(Editor* editor)
		:editor(editor)
	{

	}

	MenuBar::~MenuBar()
	{

	}

	void MenuBar::OnUpdate()
	{

		if(ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl + N"))
				{
					Scene* scene = editor->getScene();
					scene->save(scene->getName() + ".yaml");
					editor->CreateScene();
				}
				if (ImGui::MenuItem("Open Scene", "Ctrl + O"))
				{

				}

				ImGui::Separator();
				if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
				{
					std::string returnPath = FileDialog::SaveFileDialog();
					if (!returnPath.empty())
					{
						editor->getScene()->save(returnPath);
					}
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Sync"))
				{
					editor->getScene()->OnEntityRegisteryModified(); //Todo: Trigger somthing more global
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt + F4"))
				{

				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About Iaonnis"))
				{

				}
				ImGui::Separator();
				if (ImGui::MenuItem("Documentation"))
				{

				}

				if (ImGui::MenuItem("Report Bug"))
				{

				}
				ImGui::Separator();

				if (ImGui::MenuItem("GitHub"))
				{

				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}