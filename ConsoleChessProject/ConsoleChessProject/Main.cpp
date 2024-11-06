#include <iostream>
#include <unordered_set>
#include <string>
#include <array>
#include "HelperFunctions.hpp"
#include "JSONUtils.hpp"
#include "Vector2D.hpp"
#include "GameOption.hpp"
#include "json.hpp"
#include "fstream"

void PromptGameOption(std::string&);

int main()
{
	//const auto json = Utils::JSON::TryGetJSONFromFile("StartBoard.json");
	//std::cout << json.value().ToString() << std::endl;

	/*std::ifstream f("StartBoard.json");
	nlohmann::json data= nlohmann::json::parse(f);
	std::cout << data;*/

	const GameOption options[3] = { GameOption("Play"), GameOption("Restart"), GameOption("Quit") };

	std::cout << "Welcome to Chess!" << std::endl;

	std::string command;
	std::unordered_set<std::string> allCommands = GameOption::GetAllCommands();

	PromptGameOption(command);
	while (!GameOption::IsValidCommand(command)) {
		std::cout << "Invalid option! Try again" << std::endl;
		PromptGameOption(command);
	}
}

void PromptGameOption(std::string& command)
{
	std::cout << "What would you like to do?" << std::endl;
	std::string commandsMessage = "Here are possible commands: " + GameOption::AllOptionsToString();
	std::cout << commandsMessage << std::endl;
	std::cin >> command;
}
