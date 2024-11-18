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

	std::optional<GameState> _currentState;

public:

	enum class Page : int
	{
		MainMenu = 0,
		Game = 1,
	};

	const std::string WindowName;
	GameManagement::GameManager& manager;

private:
	void DrawStatic();
	void DrawMainMenu();
	void DrawGame();

	void TogglePage(const Page& page);
	bool TryUpdateBoard();

	void StartGame();

public:
	MainFrame(GameManagement::GameManager& gameManager, const wxString& title);
};

