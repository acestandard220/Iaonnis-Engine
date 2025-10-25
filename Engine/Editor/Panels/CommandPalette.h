#pragma once
#include "../ImGuiEx/ImGui_Extensions.h"
#include "Panels.h"
#include "../Scene/Entity.h"
#include "GeneralWindow.h"
namespace Iaonnis
{
	struct Command
	{
		Command(const std::string& cmd)
		{
			size_t start = cmd.find(argBegin);
			size_t end = cmd.find(argEnd);

			if (start != std::string::npos && end != std::string::npos && end > start)
			{
				hasArgs = true;
				inputCmd = cmd.substr(0, start);
				args = cmd.substr(start + 1, end - start - 1);
			}
			else
			{
				inputCmd = cmd;
			}
		}

		std::string inputCmd;
		std::string args;
		bool hasArgs = false;

		inline static const char argBegin = '<';
		inline static const char argEnd = '>';
		inline static const char argDelimiter = ',';

		static std::vector<std::string> GetArgs(const Command& cmd)
		{
			std::vector<std::string> arguments;
			std::stringstream sstr(cmd.args);
			std::string value;

			while (std::getline(sstr, value, argDelimiter))
				arguments.push_back(Trim(value));

			return arguments;
		}

	private:
		static std::string Trim(const std::string& s)
		{
			size_t start = s.find_first_not_of(" \t");
			size_t end = s.find_last_not_of(" \t");
			return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
		}
	};

	class CommandSystem
	{
	public:
		static std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> cmdRegistry;

		inline static bool Execute(const Command& cmd);
	};

	class CommandPalette : public EditorPanel
	{
	public:
		CommandPalette(Editor* editor);

		virtual void OnUpdate(float dt)override;

		char commandPaletteBuffer[128];
		bool showCommandPalette = false;
	};
}