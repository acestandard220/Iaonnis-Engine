#include "CommandPalette.h"
#include "../Editor.h"

namespace Iaonnis
{
	Editor* gEditor = nullptr;

	CommandPalette::CommandPalette(Editor* editor)
		:EditorPanel(editor)
	{
		gEditor = editor;
	}

	inline void Iaonnis::CommandPalette::OnUpdate(float dt)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_P) && ImGui::GetIO().KeyCtrl)
			showCommandPalette = !showCommandPalette;


		if (showCommandPalette)
		{
			ImGui::SetNextWindowSize(ImVec2(400, 30));
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			static const ImGuiWindowFlags commandPaletteFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize;

			if (ImGui::Begin("Command Palette", &showCommandPalette, commandPaletteFlags))
			{
				auto inputTextWidth = ImGui::GetContentRegionAvail().x - 10.0f;
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
				ImGui::SetNextItemWidth(inputTextWidth);

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
				if (ImGui::InputTextWithHint("##CommandInput", "Type a command... Enter to Execute... Ctrl + P to hide...", commandPaletteBuffer, 127,
					ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll))
				{
					Command cmd(commandPaletteBuffer);
					CommandSystem::Execute(cmd);
				}
				ImGui::PopStyleVar();

				ImGui::End();
			}
		}
	}


	std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> CommandSystem::cmdRegistry{
				{
					"log", [](const std::vector<std::string>& args)
					{
						if (args.empty())
						{
							std::cerr << "[Error] log command requires an argument.\n";
							return;
						}

						try
						{
							int logLevel = std::stoi(args[0]);
							std::cerr << "Level:: " << logLevel << std::endl;
						}
						catch (const std::exception& e)
						{
							std::cerr << "[Error] Invalid argument for log command: " << e.what() << std::endl;
						}
					}
				},
				{
					"force_kill",[](const std::vector<std::string>& args) {
						exit(-4);
					}
				},
				{
					"google_me",[](const std::vector<std::string>& args) {

					}
				},
				{
					"add_cube",[](const std::vector<std::string>& args) {
						if (args.empty())
						{
							gEditor->GetScene()->AddCube("Cube");
							return;
						}

						try {
							int inst = std::stoi(args[0]);
							for (int i = 1; i <= inst; i++)
							{
								std::string name = "Cube " + std::to_string(i);
								gEditor->GetScene()->AddCube(name);
							}
						}
						catch (const std::exception& e)
						{
							std::cerr << "Invalid argument provided\n";
						}
					}
				},
				{
					"add_light",[](const std::vector<std::string>& args) {
						if (args.empty() || args[0] == "Directional")
						{
							gEditor->GetScene()->addDirectionalLight();
						}
						else if(args[0] == "Point")
							gEditor->GetScene()->AddPointLight();
						else if(args[0] == "Spot")
							gEditor->GetScene()->addSpotLight();
					}
				}
	};

	bool CommandSystem::Execute(const Command& cmd)
	{
		auto it = cmdRegistry.find(cmd.inputCmd);
		if (it == cmdRegistry.end())
		{
			std::cout << "Unknown Command\n";
			return false;
		}

		cmdRegistry[cmd.inputCmd](Command::GetArgs(cmd));
		return true;
	}
}

