#include <wx/wx.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "ThemeControllerUI.hpp"
#include "CButton.hpp"
#include "CStepper.hpp"
#include "Cell.hpp"
#include "UIGlobals.hpp"

static CStepper* OptionsStepper;

static int StartThemeIndex = 0;
static const Theme* CurrentTheme = nullptr;

static const std::vector<Theme> AllThemes=
{
	Theme{
		"Default",
		TeamColorType{{ArmyColor::Light, LIGHT_CELL_COLOR},					{ArmyColor::Dark, DARK_CELL_COLOR}},
		TeamColorType{{ArmyColor::Light, LIGHT_CELL_HOVER_COLOR},			{ArmyColor::Dark, DARK_CELL_HOVER_COLOR}},
		TeamColorType{{ArmyColor::Light, LIGHT_CELL_SELECTED_COLOR},		{ArmyColor::Dark, DARK_CELL_SELECTED_COLOR}},
		TeamColorType{{ArmyColor::Light, LIGHT_CELL_MOVE_COLOR},			{ArmyColor::Dark, DARK_CELL_MOVE_COLOR}},
		TeamColorType{{ArmyColor::Light, LIGHT_CELL_CAPTURE_MOVE_COLOR},	{ArmyColor::Dark, DARK_CELL_CAPTURE_MOVE_COLOR}},
		TeamColorType{{ArmyColor::Light, LIGHT_CELL_PREVIOUS_MOVE_COLOR},	{ArmyColor::Dark, DARK_CELL_PREVIOUS_MOVE_COLOR}}
	},
};

static std::vector<ThemeUpdatedCallbackType> themeUpdatedEventListeners;
static void InvokeThemeUpdatedCallback(const Theme& updatedTheme)
{
	for (const auto& callback : themeUpdatedEventListeners)
	{
		callback(updatedTheme);
	}
}

void AddThemeUpdatedCallback(const ThemeUpdatedCallbackType& callback)
{
	themeUpdatedEventListeners.push_back(callback);
}

static void SetTheme(const std::string& themeName)
{
	for (const auto& theme : AllThemes)
	{
		if (theme.ThemeName == themeName)
		{
			CurrentTheme = &theme;
			return;
		}
	}
}

static void SetTheme(const int& index)
{
	if (index < 0 || index >= AllThemes.size()) return;
	CurrentTheme = &(AllThemes.at(index));

	if (CurrentTheme == nullptr) return;
	InvokeThemeUpdatedCallback(*CurrentTheme);
}

void CreateThemeController(wxWindow* parent)
{
	std::vector<CStepperOption> options = {};
	options.reserve(AllThemes.size());
	for (const auto& theme : AllThemes)
	{
		options.push_back({ theme.ThemeName, [](const std::string& theme) -> void 
			{
				SetTheme(theme);
			}});
	}

	CurrentTheme = &(AllThemes.at(StartThemeIndex));
	const wxSize stepperSize = wxSize(parent->GetSize().x, 0.05 * parent->GetSize().y);
	OptionsStepper = new CStepper(parent, options, wxDefaultPosition, stepperSize);
}

const Theme* TryGetCurrentTheme()
{
	if (CurrentTheme == nullptr) CurrentTheme = &(AllThemes.at(StartThemeIndex));
	return CurrentTheme;
}
