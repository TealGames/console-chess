#pragma once
#include <unordered_map>
#include <wx/wx.h>
#include <string>
#include <functional>
#include "Color.hpp" 

using TeamColorType = std::unordered_map<ArmyColor, wxColour>;
struct Theme
{
	std::string ThemeName = "";
	TeamColorType CellDefaultColors = {};
	TeamColorType CellHoverColors = {};
	TeamColorType CellSelectedColors = {};
	TeamColorType CellMoveColors = {};
	TeamColorType CellCaptureMoveColors = {};
	TeamColorType CellPreviousMoveColors = {};
};

void CreateThemeController(wxWindow* parent);
const Theme* TryGetCurrentTheme();

using ThemeUpdatedCallbackType = std::function<void(const Theme& theme)>;
void AddThemeUpdatedCallback(const ThemeUpdatedCallbackType& callback);
