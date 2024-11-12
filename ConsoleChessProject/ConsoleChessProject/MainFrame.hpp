#pragma once
#include <vector>
#include <wx/wx.h>
#include <wx/simplebook.h>
#include "UIGlobals.hpp"

class MainFrame : public wxFrame
{
private:
	enum class Page : int
	{
		MainMenu = 0,
		Game = 1,
	};

	wxPanel* root = nullptr;
	wxSimplebook* pages = nullptr;

	void DrawStatic();
	void DrawMainMenu();
	void DrawGame();

	void TogglePage(const Page& page);

public:
	MainFrame(const wxString& title);
};

