#pragma once
#include <string>

enum class ArmyColor
{
	Light,
	Dark,
};

std::string ToString(const ArmyColor& color);