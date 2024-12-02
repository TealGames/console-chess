#pragma once
#include <vector>
#include <optional>
#include <wx/wx.h>
#include <wx/simplebook.h>
#include "UIGlobals.hpp"
#include "GameManager.hpp"
#include "GameState.hpp"
#include "ConfirmPopup.hpp"

class MainFrame : public wxFrame
{
private:
	wxSimplebook* m_pages;
	wxPanel* m_cellParent;

	const GameState* m_currentState;
	Core::GameManager& m_manager;
	ConfirmPopup m_popup;

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

