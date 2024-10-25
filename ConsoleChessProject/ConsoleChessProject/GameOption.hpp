#pragma once
#include <string>
#include <vector>
#include <unordered_set>

class GameOption
{
private:
	std::string _option;
	static std::unordered_set<std::string> _allOptions;

	static std::string GetCommandFromOption(const std::string&, bool convertToLower = false);
public:
	const std::string& option;

	GameOption(const std::string& opt);
	~GameOption();

	static std::string GetCommand(const GameOption&, bool caseSensitive = false);
	std::string GetCommand(bool caseSensitive = false) const;
	static std::unordered_set<std::string> GetAllCommands();

	static bool IsValidCommand(const std::string&, bool caseSensitive = false);
	std::string ToString() const;
	static std::string AllOptionsToString();
};