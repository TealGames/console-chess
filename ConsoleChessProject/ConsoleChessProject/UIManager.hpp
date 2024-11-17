#pragma once
#include <wx/wx.h>
#include "MainFrame.hpp"

class UIManager
{
private:
	wxSimplebook* _pages;

public:
	MainFrame* RootFrame;
	enum class Page : int
	{
		MainMenu = 0,
		Game = 1,
	};
	
private:
	void DrawStatic();
	void DrawMainMenu();
	void DrawGame();

	void TogglePage(const Page& page);

public:
	UIManager(MainFrame* root);
};

