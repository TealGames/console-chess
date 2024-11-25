#pragma once
#include <vector>
#include <optional>
#include <wx/wx.h>
#include <wx/simplebook.h>
#include "UIGlobals.hpp"
#include "GameManager.hpp"
#include "GameState.hpp"

class MainFrame : public wxFrame
{
private:
	wxSimplebook* _pages;
	wxPanel* _cellParent;

	const GameState* _currentState;
	Core::GameManager& _manager;

public:

	enum class Page : int
	{
		MainMenu = 0,
		Game = 1,
	};

	const std::string WindowName;
	

private:
	void DrawStatic();
	void DrawMainMenu();
	void DrawGame();

	void TogglePage(const Page& page);
	bool TryUpdateBoard();

	void StartGame();

public:
	MainFrame(Core::GameManager& gameManager, const wxString& title);
};

