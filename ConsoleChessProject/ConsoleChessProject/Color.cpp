#include "Color.hpp"

std::string ToString(const ArmyColor& color)
{
	switch (color)
	{
	case ArmyColor::Light:
		return "Light";
	case ArmyColor::Dark:
		return "Dark";
	default:
		return "NULL";
	}
	return "";
}