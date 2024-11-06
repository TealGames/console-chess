#include <iostream>
#include <string>
#include <unordered_set>
#include <format>
#include "GameOption.hpp"
#include "HelperFunctions.hpp"
#include "StringUtil.hpp"

std::unordered_set<std::string> GameOption::_allOptions;

GameOption::GameOption(const std::string& opt) : _option(opt), option(_option)
{
	if (_allOptions.size() == 0 || _allOptions.find(_option) == _allOptions.end())
	{
		GameOption::_allOptions.insert(_option);
	}
	else
	{
		std::string message = std::format("Tried to create game option with"
			" duplicate option: {} size {}", _option, _allOptions.size());
		Utils::Log(Utils::LogType::Error, message);
	}
}

GameOption::~GameOption()
{
	auto it = _allOptions.find(_option);
	if (it != _allOptions.end())
	{
		GameOption::_allOptions.erase(it);
	}
}

std::string GameOption::GetCommandFromOption(const std::string& option, bool convertToLower)
{
	std::string command(1, option[0]);
	if (convertToLower)
	{
		Utils::StringUtil strUtil(command);
		command = static_cast<std::string>(strUtil.ToLowerCase());
	}
	return command;
}

std::string GameOption::GetCommand(const GameOption& option, bool convertToLower)
{
	return GameOption::GetCommandFromOption(option._option, convertToLower);
}

std::string GameOption::GetCommand(bool convertToLower) const
{
	return GetCommand(*this, convertToLower);
}

std::unordered_set<std::string> GameOption::GetAllCommands()
{
	return GameOption::_allOptions;
}

bool GameOption::IsValidCommand(const std::string& command, bool caseSensitive)
{
	std::string commandMutable = command;
	Utils::StringUtil strUtil(commandMutable);
	const std::string& commandCheck = strUtil.Trim().ToLowerCase().ToString();

	for (const std::string& str : GameOption::_allOptions)
	{
		if (str == commandCheck)
			return true;
	}
	return false;
}

std::string GameOption::ToString() const
{
	return _option + std::format("({})", GetCommand());
}

std::string GameOption::AllOptionsToString()
{
	std::string result;
	std::string currentStr;
	int index = 0;
	for (const auto& option : _allOptions)
	{
		currentStr = std::format("{}({})", option, GameOption::GetCommandFromOption(option));
		result += currentStr;
		if (index < _allOptions.size() - 1)
		{
			result += ", ";
		}
		index++;
	}
	return result;
}