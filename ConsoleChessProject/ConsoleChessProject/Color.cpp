#include "Color.hpp"

std::string ToString(const ColorTheme& color) const
{
	switch (color)
	{
	case ColorTheme::Light:
		return "Light";
	case ColorTheme::Dark:
		return "Dark";
	default:
		return "NULL";
	}
	return "";
}