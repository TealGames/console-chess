#pragma once
#include <string>

enum class ColorTheme
{
	Light,
	Dark,
};

std::string ToString(const ColorTheme& color);